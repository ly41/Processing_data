/*
  V0.1 Initial Release   10/July/2006  RBR

  V0.1.2.1
  7/17/2006 Fixed problem with maximum length packet (0x7F)
    reception in interrupt service function. Was checking for overflow,
  when I should not have been, since the RX fifo is flushed after
  reception anyway.  RBR.

  V0.2.2.
  8/2/2006 Fixed problem with checking of CRC byte.  RBR

  V0.2.3
   8/15/2006 Changed halSendPacket() packet so that TX FIFO is loaded
 before STXONCCA is done.

 V.02.3.4
 Made change in halInitRadio() with regards to the CC2420_IOCFG0 if
 dynamic PANIDs are used to accept all BCN frames

*/
//below func should be programmed in the code
#include "compiler.h"
#include "lrwpan_common_types.h"   //types common acrosss most files
#include "ieee_lrwpan_defs.h"
#include "hal.h"
#include "halstack.h"
#include "debug.h"
#include "evboard.h"
#include "evbRadio.h"
#include "memalloc.h"
#include "console.h"
#include "phy.h"
#include "mac.h"
#include "neighbor.h"
#include "debug.h"
#include "evbconfig.h"
#include <hw_timer.h>
#include <timer.h>

//全局变量
RADIO_FLAGS local_radio_flags;
EVB_SW_STATE sw_state;
static UINT16 random_seed;
/*******************************************
SSI接口与无线模块引脚配置初始化
********************************************/
void  ssiInit(void)
{
    unsigned long  ulBitRate  =  SysCtlClockGet() / 3;

    SysCtlPeriEnable(SSICLK_PERIPH);                      //  使能SSI模块所在的GPIO端口
    SysCtlPeriEnable(SYSCTL_PERIPH_SSI);                        //  使能SSI模块

    GPIOPinTypeSSI(SSICLK_PORT , SSICLK_PIN |               //  将相关GPIO设置为SSI功能
                                  SSIFSS_PIN |
                                  SSIRX_PIN |
                                  SSITX_PIN);

    SSIConfig(SSI_BASE ,                                        //  配置SSI模块
              SSI_FRF_MOTO_MODE_0 ,                             //  Freescale格式，模式0
              SSI_MODE_MASTER ,                                 //  主模式
              ulBitRate ,                                       //  设置位速率
              8);                                               //  设置数据宽度

    SysCtlPeripheralEnable(VREGEN_PERIPH);                  //设置VREGEN管脚为输出
    GPIOPinTypeGPIOOutput(VREGEN_PORT,VREGEN_PIN);

    SysCtlPeripheralEnable(RST_PERIPH);          //设置RESET管脚为输出
    GPIOPinTypeGPIOOutput(RST_PORT,RST_PIN);

    SysCtlPeripheralEnable(FIFO_PERIPH);          //设置FIFO管脚为输入
    GPIOPinTypeGPIOInput(FIFO_PORT,FIFO_PIN);

    SysCtlPeripheralEnable(FIFOP_PERIPH);          //设置FIFOP管脚为输入
    GPIOPinTypeGPIOInput(FIFOP_PORT,FIFOP_PIN);

    SysCtlPeripheralEnable(SFD_PERIPH);          //设置SFD管脚为输入
    GPIOPinTypeGPIOInput(SFD_PORT,SFD_PIN);

    SysCtlPeripheralEnable(CCA_PERIPH);          //设置CCA管脚为输入
    GPIOPinTypeGPIOInput(CCA_PORT,CCA_PIN);

    SysCtlPeripheralEnable(CSN_PERIPH);          //设置CSN管脚为输出
    GPIOPinTypeGPIOOutput(CSN_PORT,CSN_PIN);

    SSIEnable(SSI_BASE);                                        //  使能SSI收发

}

/*******************************************
FIFOP_PIN中断初始化
********************************************/
void  GPIOJ_IntInit(void)
{
    GPIOIntTypeSet(FIFOP_PORT, FIFOP_PIN, GPIO_HIGH_LEVEL);     //  设置PC5的中断类型
    GPIOPinIntEnable(FIFOP_PORT, FIFOP_PIN);                    //  使能PC5管脚中断
    IntEnable(FIFOP_INT);
}

/*******************************************
FIFOP_PORT中断服务函数
********************************************/
void  GPIO_Port_J_ISR(void)
{
    unsigned long  ulStatus;
    ulStatus  =  GPIOPinIntStatus(FIFOP_PORT, true);      //  读取中断状态
    GPIOPinIntClear(FIFOP_PORT , ulStatus);                //  清除中断状态，重要

    if ( ulStatus & FIFOP_PIN )                                //  如果PC5的中断状态有效
    {
      //  UARTCharPut(UART0_BASE, 'G');                       //  通过UART0发送
    }
   usrIntCallback();
   evbRadioIntCallback();
}
/*******************************************
硬件层开关初始化函数
********************************************/
void  SW_Init(void)
{
       SysCtlPeriEnable(SW1_PERIPH);                  //  使能GPIOD端口
       GPIOPinTypeIn(SW1_PORT , SW1_PIN);            //  设置PD1为输入类型
       SysCtlPeriEnable(SW2_PERIPH);                  //  使能GPIOG端口
       GPIOPinTypeIn(SW2_PORT, SW2_PIN);            //  设置PG5输入类型
}
 /*******************************************
硬件层LED灯初始化函数
********************************************/
void LED_Init(void)
{
     SysCtlPeriEnable(LED1_PERIPH);                  //  使能GPIOD端口
     GPIOPinTypeOut(LED1_PORT, LED1_PIN);           //  设置PD0为输出类型

     SysCtlPeriEnable(LED2_PERIPH);                  //  使能GPIOG端口
     GPIOPinTypeOut(LED2_PORT, LED2_PIN);           //  设置PG2输出类型
}

/*******************************************
考虑到开发板无法产生随机数所以需要以下函数
********************************************/
//concerning some developed board  architecture has no good way of doing RANDOM nums, 
//put this in the evboard file.
//random_seed variable is unitialized on purpose in RAM
//personalize random_seed by MAC address and TIMER0 value
void evbInitRandomSeed(void) {
  BYTE TMR0L,TMR0H;
  TMR0L = TimerValueGet(TIMER0_BASE , TIMER_B) & 0x00FF;
  TMR0H = TimerValueGet(TIMER0_BASE , TIMER_B) & 0xFF00;
  //for get the lower byte of TIMERB
  random_seed = random_seed ^ TMR0L ^  aExtendedAddress_B0;
  random_seed = random_seed << 8;
  random_seed = random_seed ^ TMR0L ^ aExtendedAddress_B1;
 if (random_seed == 0) {
   random_seed = TMR0H;
   random_seed = random_seed << 8;
   random_seed = random_seed | TMR0L;
}
}
/*******************************************
开发板中断回调函数，开关状态处理
********************************************/
void evbIntCallback(void){
//poll the switches
 sw_state.bits.s1_last_val = sw_state.bits.s1_val;
 sw_state.bits.s2_last_val = sw_state.bits.s2_val;
 sw_state.bits.s1_val = !(SW1_INPUT_VALUE()); //low true switch, so invert
 sw_state.bits.s2_val = !(SW2_INPUT_VALUE());//low true switch, so invert
 if (sw_state.bits.s1_last_val != sw_state.bits.s1_val) {
       sw_state.bits.s1_tgl = 1;
 }
 if (sw_state.bits.s2_last_val != sw_state.bits.s2_val) {
       sw_state.bits.s2_tgl = 1;
 }

 }

/*******************************************
板级自动调用函数，当用户已定义低定时器时，
可通过定时中断调用板级回调函数，此时函数内容为空
********************************************/
#define SW_POLL_TIME   MSECS_TO_MACTICKS(100)
UINT32 last_switch_poll = 0;
void evbPoll(void){
//only do this if the slow timer not enabled since
//the slowtimer interrupt will handle the polling
#ifndef LRWPAN_ENABLE_SLOW_TIMER
// poll the switches
if ( halMACTimerNowDelta(last_switch_poll) > SW_POLL_TIME) {
    evbIntCallback();
    last_switch_poll = halGetMACTimer();
  }
#endif
}

/*******************************************
开发板底层初始化
********************************************/
void evbInit(void){
   local_radio_flags.val = 0;
   sw_state.val = 0;
   last_switch_poll = halGetMACTimer();
   evbInitRandomSeed();
    //configure SW1, SW2
   SW_Init();
   LED_Init();

   LED1_OFF();
   LED2_OFF();
   ssiInit();
   //中断引脚初始化
   GPIOJ_IntInit();
}
/*******************************************
LED操作函数，控制LED灯的明灭
********************************************/
void evbLedSet(BYTE lednum, BOOL state) {
    switch(lednum) {
       case 1:    if (state) LED1_ON(); else LED1_OFF(); break;
       case 2:    if (state) LED2_ON(); else LED2_OFF(); break;
    }
}
/*******************************************
获取LED灯状态
********************************************/
BOOL evbLedGet(BYTE lednum){
 switch(lednum) {
       case 1:    return(LED1_STATE());
       case 2:    return(LED2_STATE());
    }
  return FALSE;
}
/*******************************************
获取随机数函数
********************************************/
UINT8 halGetRandomByte(void) {
     BYTE bit;

     bit = 0;
     if (random_seed & 0x8000) bit = bit ^ 1;
     if (random_seed &  0x4000) bit = bit ^ 1;
     if (random_seed &  0x1000) bit = bit ^ 1;
     if (random_seed &  0x0008) bit = bit ^ 1;
    random_seed = random_seed << 1;
    if (bit) random_seed++;
    return(random_seed & 0xFF);
}
/*******************************************
设置无线设备的网络标识符，PANID
********************************************/
void halSetRadioPANID(UINT16 panid){
   unsigned char PAN_id[2]={0x00,0x00};  //目标地址的PAN标识符
   PAN_id[0] =  panid&0xFF ;
   PAN_id[1] = (panid>>8)&0xFF ;
   DISABLE_GLOBAL_INTERRUPT();
   //FASTSPI_WRITE_RAM_LE(&panid, CC2420RAM_PANID, 2, n);   // 向 CC2420写入PANID
   //Write_RAM( CC2420RAM_PANID , 2 , PAN_id);    //向RAM中写入地址，用于地址识别
   Write_RAM(0x68,0x02,2,PAN_id);
   ENABLE_GLOBAL_INTERRUPT();
   DEBUG_STRING(DBG_INFO,"RadioPanID: ");
   DEBUG_UINT16(DBG_INFO,panid);
   DEBUG_STRING(DBG_INFO,"\n");
}
/*******************************************
设置无线设备的短地址
********************************************/
void halSetRadioShortAddr(SADDR saddr){
   unsigned char short_addr[2]={0x00,0x00};    //目标地址
   //发送字节的次序
   short_addr[0] =  saddr&0xFF ;
   short_addr[1] =  (saddr>>8)&0xFF ;
   DISABLE_GLOBAL_INTERRUPT();
   //FASTSPI_WRITE_RAM_LE(&saddr, CC2420RAM_SHORTADDR, 2, n);
   //Write_RAM( CC2420RAM_SHORTADDR , 2 , short_addr);
   Write_RAM(0x6a,0x02,2,short_addr);
   ENABLE_GLOBAL_INTERRUPT();
}
/*******************************************
设置无线设备的长地址——IEEE64扩展地址
********************************************/
void halSetRadioIEEEAddress(void) {
 //UINT8 intStatus;
 BYTE buf[8];
 halGetProcessorIEEEAddress(buf);
 DISABLE_GLOBAL_INTERRUPT();
 //FASTSPI_WRITE_RAM_LE(&buf[0], CC2420RAM_IEEEADDR, 8, n);
 Write_RAM( 0x60,0x02, 8 , buf);
 ENABLE_GLOBAL_INTERRUPT();
}
/*******************************************
等待开发板晶振起振函数
********************************************/
void halRfWaitForCrystalOscillator(void) {
    //BYTE spiStatusByte,intStatus;
} // halRfWaitForCrystalOscillator

/*******************************************
设置无线设备的短地址
********************************************/
void halDisableRadio(void) {
  DISABLE_FIFOP_INT();
  SET_VREG_INACTIVE();
  SET_RESET_ACTIVE();
  halWaitMs(10);
}
/*******************************************
设置无线设备物理接收频道2.4GHZ,可选频道11-26
********************************************/
LRWPAN_STATUS_ENUM halSetChannel(BYTE channel){
    UINT16 f;
	// Derive frequency programming from the given channel number
	f = (UINT16) (channel - 11); // Subtract the base channel
	f = f + (f << 2);    		 // Multiply with 5, which is the channel spacing
	f = f + 357 + 0x4000;		 // 357 is 2405-2048, 0x4000 is LOCK_THR = 1
	
    // Write it to the CC2420
    DISABLE_GLOBAL_INTERRUPT() ;
    //FASTSPI_SETREG(CC2420_FSCTRL, f);
    Write_ConfigureWord(CC2420_FSCTRL, (f) >> 8, f);
    ENABLE_GLOBAL_INTERRUPT();
  return(LRWPAN_STATUS_SUCCESS);
}

/*******************************************
对无线模块进行复位
********************************************/
void halResetRadio(void){
  SET_VREG_INACTIVE();
  halWaitMs(10);
  halInitRadio(phy_pib.phyCurrentFrequency, phy_pib.phyCurrentChannel, local_radio_flags);
 //write our PANID, our short address
 halSetRadioPANID(mac_pib.macPANID);
 halSetRadioShortAddr(macGetShortAddr());
}
/*******************************************
初始化射频无线模块
********************************************/
#define CRYSTAL_TIMEOUT 100  //in ms
LRWPAN_STATUS_ENUM halInitRadio(PHY_FREQ_ENUM frequency, BYTE channel, RADIO_FLAGS radio_flags)
{
     //BYTE intStatus;
     UINT8 tmp;
     UINT8  dataH,dataL;
     UINT32 start_tick;
     BYTE spiStatusByte;
     conPrintROMString("halInitRadio");
  	 // Make sure that the voltage regulator is on, and that the reset pin is inactive
     SET_RESET_ACTIVE();
     SET_VREG_ACTIVE();
     halWaitMs(1);
     SET_RESET_INACTIVE();
     halWaitUs(10);
     DISABLE_GLOBAL_INTERRUPT() ;
     SPI_ENABLE();
     // Register modifications
     // Poll the SPI status byte until the crystal oscillator is stable
     start_tick = halGetMACTimer();
     do {
          halGetRandomByte(); //shift the LFSR to try to make PANID more random
	      //FASTSPI_STROBE(CC2420_SXOSCON);
	      Write_Command(CC2420_SXOSCON);
	      //FASTSPI_UPD_STATUS(spiStatusByte);
	      spiStatusByte = Read_Status();
          if (! (!(spiStatusByte & (BM(CC2420_XOSC16M_STABLE))))) break;	
     } while (halMACTimerNowDelta(start_tick) < MSECS_TO_MACTICKS(CRYSTAL_TIMEOUT ) );

     if (!(spiStatusByte & (BM(CC2420_XOSC16M_STABLE)))) {
          DEBUG_STRING(DBG_ERR,"halInitRadio: Crystal failed to stabilize\n");
          ENABLE_GLOBAL_INTERRUPT();
           return(LRWPAN_STATUS_PHY_RADIO_INIT_FAILED);
        }

#define PAN_COORDINATOR     0x10
#define ADR_DECODE          0x08
#define AUTO_CRC            0x20
#define AUTO_ACK            0x10
      	// set some registers
  		SPI_ENABLE();
 		//with auto ack, auto_crc, pan_coor, the MDMCTRL0 reg should be a value of 0x1AF2
  		// high byte MDCTRL0
        //FASTSPI_TX_ADDR(CC2420_MDMCTRL0);
      	 //first, MSB
        tmp = 0x02;      //CCA hystersis, mid range
        if (radio_flags.bits.pan_coordinator) {
           tmp  =   tmp | PAN_COORDINATOR;
         }
      	if (!radio_flags.bits.listen_mode) {
          // Turning on Address Decoding
         tmp = tmp | ADR_DECODE;
      	 }
      	//FASTSPI_TX(tmp);
      	dataH = tmp;
      	//now, LSB
      	tmp = 0xC2;
    	if (!radio_flags.bits.listen_mode) {
       	//turn on autoCRC and autoACK, address decode
       		tmp = tmp | AUTO_CRC | AUTO_ACK ;
     	 }
    	 // FASTSPI_TX(tmp);
     	dataL = tmp;
        //配置MDMCTRL0寄存器
     	Write_ConfigureWord(CC2420_MDMCTRL0,dataH,dataL);
		//SPI_DISABLE();
    	local_radio_flags = radio_flags;   //save these if we need to do a reset
	    //set the rest
   		// Set the correlation threshold = 20
   		Write_ConfigureWord(CC2420_MDMCTRL1,0x05 , 0x00);

		#ifdef LRWPAN_COORDINATOR
   		// Set the FIFOP threshold to maximum
   			Write_ConfigureWord(CC2420_IOCFG0, 0x00,0x7F);
		#else
			#ifdef LRWPAN_USE_STATIC_PANID
   		    // Set the FIFOP threshold to maximum
  				Write_ConfigureWord(CC2420_IOCFG0,0x00,0x7F);
			#else
  			// Set the FIFOP threshold to maximum, received all BCN frames
 				Write_ConfigureWord(CC2420_IOCFG0,0x08,0x7F);
			#endif
		#endif
  		// Turn off "Security enable"
 		Write_ConfigureWord(CC2420_SECCTRL0,0x01,0xC4);
    	//have to set our Long address

      	halSetRadioIEEEAddress();
    	// Set the RF channel
    	halSetChannel(channel);
      	DEBUG_STRING(DBG_INFO, "Radio configured\n");
     	//enable the receive
		Write_Command(CC2420_SRXON);
		Write_Command(CC2420_SFLUSHRX);
      	// Initialize the FIFOP external interrupt
     	ENABLE_FIFOP_INT();
     	SPI_DISABLE();
     	ENABLE_GLOBAL_INTERRUPT();
		return(LRWPAN_STATUS_SUCCESS);
}
/*******************************************
清空无线模块接收队列RXFIFO
********************************************/
void halFlushRXFIFO(void){

   DISABLE_GLOBAL_INTERRUPT() ;
   Write_Command(CC2420_SFLUSHRX);
   Write_Command(CC2420_SFLUSHRX);
  ENABLE_GLOBAL_INTERRUPT();
}
/*******************************************
IEEE回退算法，避免物理信道冲突多路访问，CSMA
********************************************/
//regardless of what happens here, we will try TXONCCA after this returns.
void  doIEEE_backoff(void) {
    BYTE be, nb, tmp, rannum;
    UINT32  delay, start_tick;

	be = aMinBE;
   	nb = 0;
  	do {
      	if (be) {
        //do random delay
        tmp = be;
        //compute new delay
        delay = 1;
        while (tmp) {
          	delay = delay << 1;  //delay = 2**be;
           	tmp--;
         }
        rannum =  halGetRandomByte() & (delay-1); //rannum will be between 0 and delay-1
        delay = 0;
        while (rannum) {
            delay  += SYMBOLS_TO_MACTICKS(aUnitBackoffPeriod);
            rannum--;
         }//delay = aUnitBackoff * rannum
        //now do backoff
       	start_tick = halGetMACTimer();
        while (halMACTimerNowDelta(start_tick) < delay);
   	}
    //check CCA
    //if (PIN_CCA)  break;
    if(CCA_IS_1)  break;
    nb++;
    be++;
    if (be > aMaxBE) be =aMaxBE;
   	}while (nb <= macMaxCSMABackoffs);
  	return;
}
/*******************************************
物理层发送数据协议包PPDU
********************************************/
#define TX_TIMEOUT    100   //in ms, overkill for waiting for TX to be ready
LRWPAN_STATUS_ENUM halSendPacket(BYTE flen, BYTE *frm)
{ 
	LRWPAN_STATUS_ENUM return_status;
    BYTE errflag;
    BYTE spiStatusByte;
    UINT32 start_tick;
    BYTE len;
	static int cnt=0;
    //unsigned char dataTXbuf[50];
    //dbgPrintPacket(frm, flen);
   	// Turn off global interrupts to avoid interference on the SPI interface

   	DEBUG_STRING( DBG_INFO,"halSendPacket");
   	DEBUG_STRING(DBG_INFO, "TX PKT Size: ");
   	DEBUG_UINT8(DBG_INFO,flen + PACKET_FOOTER_SIZE);
   	DEBUG_STRING(DBG_INFO,"\n");
   	if ((flen + PACKET_FOOTER_SIZE)> 127) {
     	//packet size is too large!
     	return(LRWPAN_STATUS_PHY_TX_PKT_TOO_BIG);
   	}
    return_status = LRWPAN_STATUS_SUCCESS;
    errflag = 0;

    halSendPacket_1:
    start_tick = halGetMACTimer();
    do {
      	if ( !((FIFOP_IS_1 || SFD_IS_1))) break;
    }  while (halMACTimerNowDelta(start_tick) < MSECS_TO_MACTICKS(TX_TIMEOUT ) );
    // Wait until the transceiver is idle
    if  ((FIFOP_IS_1) || (SFD_IS_1)) {
     	if (!errflag) {
        //we are stuck. lets try flushing the RXFIFO, try this again
         halFlushRXFIFO();
         errflag = 1;
         goto halSendPacket_1;
         } else {
            	//give it up.
             	DEBUG_STRING(DBG_ERR,"txpacket: unable to tx, stuck in receive mode\n");
             	return(LRWPAN_STATUS_PHY_TX_START_FAILED);
           }
      }
    DISABLE_GLOBAL_INTERRUPT() ;
    // Flush the TX FIFO just in case...
    //FASTSPI_STROBE(CC2420_SFLUSHTX);
    Write_Command(CC2420_SFLUSHTX);
    ENABLE_GLOBAL_INTERRUPT();
    //load up the FIFO
    //total length, does not include length byte itself
    //last two bytes are the FCS bytes that are added automatically
    len = flen +  PACKET_FOOTER_SIZE;
    // Write the packet to the TX FIFO
    DISABLE_GLOBAL_INTERRUPT() ;
    Write_TXFIFO(1,(BYTE*)&len);
    Write_TXFIFO(flen,(BYTE*) frm);  
   	/* for(int i=0;i<flen;i++)
      {
        conPrintUINT8(frm[i]);
      }
      conPrintString("\n");*/
    ENABLE_GLOBAL_INTERRUPT();
    // Wait for the RSSI value to become valid
    start_tick = halGetMACTimer();
    do {
        DISABLE_GLOBAL_INTERRUPT() ;
        //FASTSPI_UPD_STATUS(spiStatusByte);
        spiStatusByte = Read_Status();
         ENABLE_GLOBAL_INTERRUPT();
        if (spiStatusByte & BM(CC2420_RSSI_VALID)) break;
    }  while (halMACTimerNowDelta(start_tick) < MSECS_TO_MACTICKS(TX_TIMEOUT ) );

    if (!(spiStatusByte & BM(CC2420_RSSI_VALID)) ) {
       DEBUG_STRING(DBG_ERR,"txpacket: rssi never went valid\n");
        //this is a serious error, lets reset the radio before returning
       halResetRadio();
        return_status = LRWPAN_STATUS_PHY_TX_START_FAILED;
       goto halSendPacket_exit;
      }
	//MsstatePAN协议使用信标不使能的PAN，采用非时隙型CSMA-CA算法
    doIEEE_backoff();

	// TX begins after the CCA check has passed
    start_tick = halGetMACTimer();
    do {
            DISABLE_GLOBAL_INTERRUPT() ;
			//FASTSPI_STROBE(CC2420_STXONCCA);
			Write_Command(CC2420_STXONCCA);
			//FASTSPI_UPD_STATUS(spiStatusByte);
			spiStatusByte = Read_Status();
           	ENABLE_GLOBAL_INTERRUPT();
			halWaitUs(50);
	     	halWaitUs(50);
           if (spiStatusByte & BM(CC2420_TX_ACTIVE)) break;

    } while (halMACTimerNowDelta(start_tick) < MSECS_TO_MACTICKS(TX_TIMEOUT ) );
     if (! (spiStatusByte & BM(CC2420_TX_ACTIVE))) {
     	DEBUG_STRING(DBG_ERR,"txpacket: TXONCCA failed to start\n");
        return_status = LRWPAN_STATUS_PHY_TX_START_FAILED;
        goto halSendPacket_exit;
      }

	// Wait for the transmission to begin before exiting (makes sure that this function cannot be called
	// a second time, and thereby cancelling the first transmission (observe the FIFOP + SFD test above).
    start_tick = halGetMACTimer();
    do {
	  if (SFD_IS_1) break;
     }  while (halMACTimerNowDelta(start_tick) < MSECS_TO_MACTICKS(TX_TIMEOUT ) );

    if  (!(SFD_IS_1)) {
         DEBUG_STRING(DBG_ERR,"txpacket: Tx failed to start\n");
         return_status = LRWPAN_STATUS_PHY_TX_START_FAILED;
         goto halSendPacket_exit;
    }

 	phyTxStartCallBack();
    //wait for finish
    do {
    	if ( !((FIFOP_IS_1 || SFD_IS_1))) break;
      }  while (halMACTimerNowDelta(start_tick) < MSECS_TO_MACTICKS(TX_TIMEOUT ) );
      	// Wait until the transceiver is idle
    if  ((FIFOP_IS_1) || (SFD_IS_1)) {
      	DEBUG_STRING(DBG_ERR,"txpacket: Tx failed to end\n");
        return_status = LRWPAN_STATUS_PHY_TX_FINISH_FAILED;
       	goto halSendPacket_exit;
    }
    //do callBacks signaling the end
    phyTxEndCallBack();
    macTxCallback();
    DEBUG_STRING(DBG_ERR,"txpacket: Tx success\n");
    if(len == 0x15||len == 0x1D)
    {
     	cnt++;
       	if(cnt ==3)
       	mac_pib.flags.bits.ackPending = 0;
     }
     halSendPacket_exit:
	// Turn interrupts back on

   	return(return_status);
}

/*******************************************
无线设备接收中断回调函数，处理接收数据包
********************************************/
void evbRadioIntCallback(void){
  	unsigned char flen=0;
  	BYTE *ptr, *rx_frame;
  	BYTE ack_bytes[5];
  	BYTE crc;
  	unsigned char i;
  	unsigned char databuf[127];
  	//define alternate names for readability in this function
#define  fcflsb ack_bytes[0]
#define  fcfmsb  ack_bytes[1]
#define  dstmode ack_bytes[2]
#define  srcmode ack_bytes[3]


    //DEBUG_CHAR( DBG_ITRACE,DBG_CHAR_RXRCV );
    //read the packet, do not have to check for overflow, because we flush the frame buffer
   	//at the end of the read anyway, which clears any overflow condition.

    //select FIFO
  	SPI_ENABLE();
    Read_RXFIFO(&flen,databuf);
	//打印接收包内容
    /*for(int i=0;i<flen;i++)
    {
      	conPrintUINT8(databuf[i]);
    }
    conPrintString("\n");*/
    ptr = NULL; //temporary pointer
	flen &= 0x7F; // Ignore MSB
    // Ignore the packet if the length is too short
    if (flen <  LRWPAN_ACKFRAME_LENGTH) {
       	goto do_rxflush;
     }
    // Otherwise, if the length is valid, then proceed with the rest of the packet
    if (flen == LRWPAN_ACKFRAME_LENGTH) {
    	//this should be an ACK.
        //read the packet, do not allocate space for it
      	//DEBUG_CHAR( DBG_ITRACE,DBG_CHAR_ACKPKT );
        ack_bytes[0]= flen;
        //read next four bytes into the ack buffer
        ack_bytes[1] = databuf[0];
        ack_bytes[2] = databuf[1];
        ack_bytes[3] =  databuf[2];
        ack_bytes[4] =  databuf[3];
        crc =  databuf[13];
	  	//check CRC
        if (crc & 0x80){
          // CRC ok, perform callback if this is an ACK
          macRxCallback(ack_bytes, ack_bytes[4]);
      	}
        //goto do_rxflush;
    }
    else {
      	fcflsb = databuf[0];
      	fcfmsb = databuf[1];
      	if (!local_radio_flags.bits.listen_mode) {
         	//only reject if not in listen mode
           	//get the src, dst addressing modes
           	srcmode = LRWPAN_GET_SRC_ADDR(fcfmsb);
          	dstmode = LRWPAN_GET_DST_ADDR(fcfmsb);
          	if ((srcmode == LRWPAN_ADDRMODE_NOADDR) && (dstmode == LRWPAN_ADDRMODE_NOADDR)) {
               	//reject this packet, no addressing info
              	goto do_rxflush;
          	}
        }
      	if (!macRxBuffFull()) {
        //MAC RX buffer has room
        //allocate new memory space
        //read the length
        rx_frame = ISRMemAlloc(flen+1);
        ptr = rx_frame;
      	} else {
        	//MAC RX buffer is full
        	DEBUG_CHAR( DBG_ITRACE,DBG_CHAR_MACFULL);
      		//  goto do_rxflush;
      	  }
      	// at this point, if ptr is null, then either
      	// the MAC RX buffer is full or there is  no
      	// free memory for the new frame, or the packet is
      	// going to be rejected because of addressing info.
      	// In these cases, we need to
      	// throw the RX packet away
      	if (ptr == NULL) {
        	//just flush the bytes
        	goto do_rxflush;
      	}else {

        //save packet, including the length
        *ptr = flen; ptr++;
        //save the fcflsb, fcfmsb bytes
        *ptr = fcflsb; ptr++; flen--;
        *ptr = fcfmsb; ptr++; flen--;
        i=2;
        while(flen)
        {
          	*ptr = databuf[i];
         	ptr++;
          	i++;
          	flen--;
        }
        //do RX callback
        //check the CRC
        if (*(ptr-1) & 0x80) {
          	//CRC good
          	//change the RSSI byte from 2's complement to unsigned number
          	*(ptr-2) = *(ptr-2) + 0x80;
          	phyRxCallback();
          	macRxCallback(rx_frame, *(ptr-2));
        }else {
          	// CRC bad. Free the packet
          	ISRMemFree(rx_frame);
         }
       }
    }

    //flush any remaining bytes
   	do_rxflush:
       	halWaitUs(1);
       	SPI_DISABLE();
      	Write_Command(CC2420_SFLUSHRX);
      	Write_Command(CC2420_SFLUSHRX);


}
