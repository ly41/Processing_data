//*****************************************************************************
//
// usb_sound.c - USB host audio handling functions.
//
// Copyright (c) 2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6075 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "drivers/usb_sound.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbmsc.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhaudio.h"

//*****************************************************************************
//
// The size of the host controller's memory pool in bytes.
//
//*****************************************************************************
#define HCD_MEMORY_SIZE         768

//*****************************************************************************
//
// The memory pool to provide to the Host controller driver.
//
//*****************************************************************************
unsigned char g_pHCDPool[HCD_MEMORY_SIZE];

//*****************************************************************************
//
// The instance data for the USB host audio driver.
//
//*****************************************************************************
unsigned long g_ulAudioInstance = 0;

//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

//*****************************************************************************
//
// This structure holds the state information for the USB audio device.
//
//*****************************************************************************
static struct
{
    //
    // Save the application provided callback function.
    //
    tUSBBufferCallback pfnCallbackOut;

    //
    // Save the application provided callback function.
    //
    tUSBBufferCallback pfnCallbackIn;

    //
    // The event callback for the application.
    //
    tEventCallback pfnCallbackEvent;

    //
    // Volume control multipliers calculated from the information received
    // from the audio device.
    //
    unsigned long pulSteps[3];

    //
    // The currently pending audio device events.
    //
    unsigned long ulEventFlags;

    //
    // The current state for the audio device.
    //
    volatile enum
    {
        //
        // No device is present.
        //
        STATE_NO_DEVICE,

        //
        // Audio device is ready.
        //
        STATE_DEVICE_READY,

        //
        // An unsupported device has been attached.
        //
        STATE_UNKNOWN_DEVICE,

        //
        // A power fault has occurred.
        //
        STATE_POWER_FAULT
    } eState;

} g_sAudioState;

//*****************************************************************************
//
// These defines are used with the ulEventFlags in the g_sAudioState structure.
//
//*****************************************************************************
#define EVENT_OPEN              0x00000001
#define EVENT_CLOSE             0x00000002

//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the host audio class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_USBHostAudioClassDriver
    ,&g_sUSBEventDriver
};

//*****************************************************************************
//
// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
//
//*****************************************************************************
static const unsigned long g_ulNumHostClassDrivers =
    sizeof(g_ppHostClassDrivers) / sizeof(tUSBHostClassDriver *);

//*****************************************************************************
//
// The control table used by the uDMA controller.  This table must be aligned
// to a 1024 byte boundary.  In this application uDMA is only used for USB,
// so only the first 6 channels are needed.
//
//*****************************************************************************
#if defined(ewarm)
#pragma data_alignment=1024
tDMAControlTable g_sDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(g_sDMAControlTable, 1024)
tDMAControlTable g_sDMAControlTable[64];
#else
tDMAControlTable g_sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

//*****************************************************************************
//
// This function was the callback function registered with the USB host audio
// class driver.  The only two events that are handled at this point are the
// USBH_AUDIO_EVENT_OPEN and USBH_AUDIO_EVENT_CLOSE which indicate that a new
// audio device has been found or that an existing audio device has been
// disconnected.
//
//*****************************************************************************
static void
AudioCallback(void *pvBuffer, unsigned long ulParam, unsigned long ulEvent)
{
    switch(ulEvent)
    {
        //
        // New USB audio device has been enabled.
        //
        case USBH_AUDIO_EVENT_OPEN:
        {
            //
            // Set the EVENT_OPEN flag and let the main routine handle it.
            //
            HWREGBITW(&g_sAudioState.ulEventFlags, EVENT_OPEN) = 1;

            break;
        }

        //
        // USB audio device has been removed.
        //
        case USBH_AUDIO_EVENT_CLOSE:
        {
            //
            // Set the EVENT_CLOSE flag and let the main routine handle it.
            //
            HWREGBITW(&g_sAudioState.ulEventFlags, EVENT_CLOSE) = 1;

            break;
        }
        default:
        {
        }
    }
}

//*****************************************************************************
//
//! Initializes the sound output.
//!
//! \param ulFlags is unused as this point but is included for future
//! functionality.
//! \param pfnCallback is the event callback function for audio devices.
//!
//! This function prepares the sound driver to enumerate an audio device and
//! prepares to play audio once a valid audio device is detected.  The
//! \e pfnCallback function can be used to receive callbacks when there are
//! changes related to the audio device.  The ulEvent parameter to the callback
//! will be one of the SOUND_EVENT_* values.
//!
//! \return None
//
//*****************************************************************************
void
USBSoundInit(unsigned long ulFlags, tEventCallback pfnCallback)
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Set the USB power pins to be controlled by the USB controller.
    //
    GPIOPinTypeUSBDigital(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //
    // Enable the uDMA controller and set up the control table base.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    uDMAEnable();
    uDMAControlBaseSet(g_sDMAControlTable);

    //
    // Initialize the USB stack mode to OTG.
    //
    USBStackModeSet(0, USB_MODE_OTG, 0);

    //
    // Register the host class drivers.
    //
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, g_ulNumHostClassDrivers);

    //
    // Open an instance of the audio class driver.
    //
    g_ulAudioInstance = USBHostAudioOpen(0, AudioCallback);

    //
    // Initialize the power configuration. This sets the power enable signal
    // to be active high and does not enable the power fault.
    //
    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);

    //
    // Initialize the USB controller for OTG operation with a 2ms polling
    // rate.
    //
    USBOTGModeInit(0, 2000, g_pHCDPool, HCD_MEMORY_SIZE);

    //
    // Save the event callback function.
    //
    g_sAudioState.pfnCallbackEvent = pfnCallback;
}

//*****************************************************************************
//
//! Sets the volume of the audio device.
//!
//! \param ulPercent is the volume percentage, which must be between 0%
//! (silence) and 100% (full volume), inclusive.
//!
//! This function sets the volume of the sound output to a value between
//! silence (0%) and full volume (100%).
//!
//! \return None.
//
//*****************************************************************************
void
USBSoundVolumeSet(unsigned long ulPercent)
{
    unsigned long ulValue;

    //
    // Ignore volume changes if there is no device present.
    //
    if(g_sAudioState.eState == STATE_DEVICE_READY)
    {
        //
        // Scale the voltage percentage to the decibel range provided by the
        // USB audio device.
        //
        ulValue = (g_sAudioState.pulSteps[1] * ulPercent) / 100;
        USBHostAudioVolumeSet(g_ulAudioInstance, 0, 1, ulValue);

        ulValue = (g_sAudioState.pulSteps[2] * ulPercent) / 100;
        USBHostAudioVolumeSet(g_ulAudioInstance, 0, 2, ulValue);
    }
}

//*****************************************************************************
//
//! Returns the current volume level.
//!
//! This function returns the current volume, specified as a percentage between
//! 0% (silence) and 100% (full volume), inclusive.
//!
//! \return Returns the current volume.
//
//*****************************************************************************
unsigned long
USBSoundVolumeGet(unsigned long ulChannel)
{
    unsigned long ulVolume;

    //
    // Initialize the return value in case there is no USB device present.
    //
    ulVolume = 0xffffffff;

    //
    // Ignore volume request if there is no device present.
    //
    if(g_sAudioState.eState == STATE_DEVICE_READY)
    {
        ulVolume = USBHostAudioVolumeGet(g_ulAudioInstance, 0, ulChannel);
    }

    return(ulVolume);
}

//*****************************************************************************
//
//! This will set the current output audio format of the USB audio device.
//!
//! \param ulSampleRate is the sample rate.
//! \param ulBits is the number of bits per sample.
//! \param ulChannels is the number of channels.
//!
//! This sets the current audio format for the USB device that is currently
//! connected.  If there is no USB device connected or the format is not
//! supported then the function will return a non-zero value.  The function
//! will return zero if the USB audio device was successfully configured to the
//! requested audio format.
//!
//! \return Returns zero if the format was successfully set or returns an
//! non-zero value if the format was not able to be set.
//
//*****************************************************************************
unsigned long
USBSoundOutputFormatSet(unsigned long ulSampleRate, unsigned long ulBitsPerSample,
                     unsigned long ulChannels)
{
    //
    // Just return if there is no device at this time.
    //
    if(g_sAudioState.eState != STATE_DEVICE_READY)
    {
        return(1);
    }

    //
    // Call the USB Host Audio function to set the format.
    //
    return(USBHostAudioFormatSet(g_ulAudioInstance, ulSampleRate,
                                 ulBitsPerSample, ulChannels,
                                 USBH_AUDIO_FORMAT_OUT));
}

//*****************************************************************************
//
//! This will set the current input audio format of the USB audio device
//!
//! \param ulSampleRate is the sample rate.
//! \param ulBits is the number of bits per sample.
//! \param ulChannels is the number of channels.
//!
//! This sets the current format for the USB device that is currently connect.
//! If there is no USB device connected or the format is not supported then the
//! function will return 0.  The function will return 1 if the USB audio device
//! was successfully configured to the requested format.
//!
//! \return Returns 1 if the format was successfully set or returns 0 if the
//!         format was not changed.
//
//*****************************************************************************
unsigned long
USBSoundInputFormatSet(unsigned long ulSampleRate, unsigned long ulBitsPerSample,
                     unsigned long ulChannels)
{
    //
    // Just return if there is no device at this time.
    //
    if(g_sAudioState.eState != STATE_DEVICE_READY)
    {
        return(0);
    }

    return(USBHostAudioFormatSet(g_ulAudioInstance, ulSampleRate,
                                 ulBitsPerSample, ulChannels,
                                 USBH_AUDIO_FORMAT_IN));
}

//*****************************************************************************
//
// Returns the current sample rate.
//
// This function returns the sample rate that was set by a call to
// USBSoundSetFormat().  This is needed to retrieve the exact sample rate that is
// in use in case the requested rate could not be matched exactly.
//
// \return The current sample rate in samples/second.
//
//*****************************************************************************
unsigned long
USBSoundOutputFormatGet(unsigned long ulSampleRate, unsigned long ulBits,
                     unsigned long ulChannels)
{
    //
    // Just return if there is no device at this time.
    //
    if(g_sAudioState.eState != STATE_DEVICE_READY)
    {
        return(0);
    }

    return(USBHostAudioFormatGet(g_ulAudioInstance, ulSampleRate, ulBits,
                                 ulChannels, USBH_AUDIO_FORMAT_OUT));
}

//*****************************************************************************
//
// Returns the current sample rate.
//
// This function returns the sample rate that was set by a call to
// USBSoundSetFormat().  This is needed to retrieve the exact sample rate that is
// in use in case the requested rate could not be matched exactly.
//
// \return The current sample rate in samples/second.
//
//*****************************************************************************
unsigned long
USBSoundInputFormatGet(unsigned long ulSampleRate, unsigned long ulBits,
                    unsigned long ulChannels)
{
    //
    // Just return if there is no device at this time.
    //
    if(g_sAudioState.eState != STATE_DEVICE_READY)
    {
        return(0);
    }

    return(USBHostAudioFormatGet(g_ulAudioInstance, ulSampleRate, ulBits,
                                 ulChannels, USBH_AUDIO_FORMAT_IN));
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// \param pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the audio device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
// \return None.
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ulEvent)
    {
        //
        // Unknown device detected.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // An unknown device was detected.
            //
            g_sAudioState.eState = STATE_UNKNOWN_DEVICE;

            //
            // Call the general event handler if present.
            //
            if(g_sAudioState.pfnCallbackEvent)
            {
                g_sAudioState.pfnCallbackEvent(SOUND_EVENT_UNKNOWN_DEV, 0);
            }

            break;
        }

        //
        // Device unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Unknown device has been removed.
            //
            g_sAudioState.eState = STATE_NO_DEVICE;

            //
            // Call the general event handler if present.
            //
            if(g_sAudioState.pfnCallbackEvent)
            {
                g_sAudioState.pfnCallbackEvent(SOUND_EVENT_DISCONNECT, 0);
            }

            break;
        }

        //
        // A power fault has occurred.
        //
        case USB_EVENT_POWER_FAULT:
        {
            //
            // No power means no device is present.
            //
            g_sAudioState.eState = STATE_POWER_FAULT;

            break;
        }
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function passes along a buffer callback from the USB host audio driver
// so that the application can process or release the buffers.
//
//*****************************************************************************
void
USBHostAudioCallback(void *pvBuffer, unsigned long ulParam,
                     unsigned long ulEvent)
{
    //
    // Only call the callback if it is actually present.
    //
    if(g_sAudioState.pfnCallbackOut)
    {
        g_sAudioState.pfnCallbackOut(pvBuffer, ulEvent);
    }

    //
    // Only call the callback if it is actually present.
    //
    if(g_sAudioState.pfnCallbackIn)
    {
        g_sAudioState.pfnCallbackIn(pvBuffer, ulEvent);
    }
}

//*****************************************************************************
//
//! Starts output of a block of PCM audio samples.
//!
//! \param pvData is a pointer to the audio data to play.
//! \param ulLength is the length of the data in bytes.
//! \param pfnCallback is a function to call when this buffer has be played.
//!
//! This function starts the output of a block of PCM audio samples.
//!
//! \return This function returns a non-zero value if the buffer was accepted,
//! and returns zero if the buffer was not accepted.
//
//*****************************************************************************
unsigned long
USBSoundBufferOut(const void *pvBuffer, unsigned long ulSize,
                  tUSBBufferCallback pfnCallback)
{
    //
    // If there is no device present or there is a pending buffer then just
    // return with a failure.
    //
    if(g_sAudioState.eState != STATE_DEVICE_READY)
    {
        return(0);
    }

    //
    // Save this buffer callback.
    //
    g_sAudioState.pfnCallbackOut = pfnCallback;

    //
    // Pass the buffer along to the USB host audio driver for playback.
    //
    return(USBHostAudioPlay(g_ulAudioInstance, (void *)pvBuffer, ulSize,
                            USBHostAudioCallback));
}

//*****************************************************************************
//
//! Requests a new block of PCM audio samples from a USB audio device.
//!
//! \param pvData is a pointer to a location to store the audio data.
//! \param ulLength is the size of the pvData buffer in bytes.
//! \param pfnCallback is a function to call when this buffer has new data.
//!
//! This function request a new block of PCM audio samples from a USB audio
//! device.
//!
//! \return This function returns a non-zero value if the buffer was accepted,
//! and returns zero if the buffer was not accepted.
//
//*****************************************************************************
unsigned long
USBSoundBufferIn(const void *pvBuffer, unsigned long ulSize,
                 tUSBBufferCallback pfnCallback)
{
    //
    // If there is no device present or there is a pending buffer then just
    // return with a failure.
    //
    if(g_sAudioState.eState != STATE_DEVICE_READY)
    {
        return(0);
    }

    //
    // Save this buffer callback.
    //
    g_sAudioState.pfnCallbackIn = pfnCallback;

    //
    // Pass the buffer along to the USB host audio driver for input.
    //
    return(USBHostAudioRecord(g_ulAudioInstance, (void *)pvBuffer, ulSize,
                              USBHostAudioCallback));
}

//*****************************************************************************
//
// This function reads the audio volume settings for the USB audio device and
// saves them so that the volume can be scaled correctly.
//
//*****************************************************************************
static void
GetVolumeParameters(void)
{
    unsigned long ulMax, ulMin, ulRes, ulChannel;

    for(ulChannel = 0; ulChannel < 3; ulChannel++)
    {
        ulMax = USBHostAudioVolumeMaxGet(g_ulAudioInstance, 0, ulChannel);
        ulMin = USBHostAudioVolumeMinGet(g_ulAudioInstance, 0, ulChannel);
        ulRes = USBHostAudioVolumeResGet(g_ulAudioInstance, 0, ulChannel);

        g_sAudioState.pulSteps[ulChannel] = (ulMax - ulMin) / ulRes;
    }
}

//*****************************************************************************
//
// The main routine for handling USB audio, this should be called periodically
// by the main program and pass in the amount of time in milliseconds that has
// elapsed since the last call.
//
//*****************************************************************************
void
USBMain(unsigned long ulTicks)
{
    //
    // Tell the OTG library code how much time has passed in
    // milliseconds since the last call.
    //
    USBOTGMain(ulTicks);

    switch(g_sAudioState.eState)
    {
        //
        // This is the running state where buttons are checked and the
        // screen is updated.
        //
        case STATE_DEVICE_READY:
        {
            if(HWREGBITW(&g_sAudioState.ulEventFlags, EVENT_CLOSE))
            {
                HWREGBITW(&g_sAudioState.ulEventFlags, EVENT_CLOSE) = 0;
                g_sAudioState.eState = STATE_NO_DEVICE;

                //
                // Call the general event handler if present.
                //
                if(g_sAudioState.pfnCallbackEvent)
                {
                    g_sAudioState.pfnCallbackEvent(SOUND_EVENT_DISCONNECT, 0);
                }
            }
            break;
        }

        //
        // If there is no device then just wait for one.
        //
        case STATE_NO_DEVICE:
        {
            if(HWREGBITW(&g_sAudioState.ulEventFlags, EVENT_OPEN))
            {
                g_sAudioState.eState = STATE_DEVICE_READY;

                HWREGBITW(&g_sAudioState.ulEventFlags, EVENT_OPEN) = 0;

                GetVolumeParameters();

                //
                // Call the general event handler if present.
                //
                if(g_sAudioState.pfnCallbackEvent)
                {
                    g_sAudioState.pfnCallbackEvent(SOUND_EVENT_READY, 0);
                }
            }
            break;
        }

        //
        // An unknown device was connected.
        //
        case STATE_UNKNOWN_DEVICE:
        {
            break;
        }

        //
        // Something has caused a power fault.
        //
        case STATE_POWER_FAULT:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}
