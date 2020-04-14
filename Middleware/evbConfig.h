/*
  V0.1 Initial Release   10/July/2006  RBR

*/
#ifndef EVBCONFIG_H
#define EVBCONFIG_H

#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <hw_ints.h>
#include  <hw_sysctl.h>
#include  <hw_gpio.h>
#include  <hw_timer.h>
#include  <interrupt.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <watchdog.h>

#define HEAPSEGMENT 0x100
#define NEIGHBORSEGMENT 0xB00
#define FOSC 16000000    //internal clock frequency
#define LED_OFF 0
#define LED_ON  1  
/***********************************************************
函数宏定义以缩写库函数名
***********************************************************/
#define  GPIOPinTypeIn              GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut             GPIOPinTypeGPIOOutput
#define  SysCtlPeriEnable           SysCtlPeripheralEnable
#define  SysCtlPeriDisable          SysCtlPeripheralDisable

//other macros
#define NOP()               
#define RESET()             SysCtlReset( )
#define SLEEP()             SysCtlSleep( ) 
#define DISABLE_WDT()       HWREG(WATCHDOG_BASE + WDT_O_CTL)&=~(WDT_CTL_RESEN)
#define ENABLE_WDT()        HWREG(WATCHDOG_BASE + WDT_O_CTL)|=WDT_CTL_RESEN
/***********************************************************
开关与LED灯宏定义操作
***********************************************************/
#define SW1_INPUT_VALUE()       GPIOPinRead(SW1_PORT,SW1_PIN)
#define SW2_INPUT_VALUE()       GPIOPinRead(SW2_PORT,SW2_PIN)
#define LED1_OFF()   			GPIOPinWrite(LED1_PORT,LED1_PIN,~LED1_PIN)
#define LED1_ON()    			GPIOPinWrite(LED1_PORT,LED1_PIN,LED1_PIN)
#define LED2_OFF()   			GPIOPinWrite(LED2_PORT,LED2_PIN,~LED2_PIN)
#define LED2_ON()    			GPIOPinWrite(LED2_PORT,LED2_PIN,LED2_PIN)
#define LED1_STATE() 			GPIOPinRead(LED1_PORT,LED1_PIN)
#define LED2_STATE() 			GPIOPinRead(LED2_PORT,LED2_PIN)
/***********************************************************
函数声明
***********************************************************/
void LED_Init(void);
void SW_Init(void);
#endif //EVBCONFIG_H



