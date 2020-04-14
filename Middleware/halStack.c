/*
  V0.1 Initial Release   10/July/2006  RBR

*/
#include "lrwpan_config.h"
#include "hal.h"
#include "halstack.h"
#include "console.h"
#include "debug.h"
#include "ieee_lrwpan_defs.h"
#include "memalloc.h"
#include  "LM3S9b96_PinMap.h"
#include  <stdio.h>
#include  <string.h>
#include  <hw_types.h>
#include  <hw_uart.h>
#include  <hw_timer.h>
#include  <interrupt.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <timer.h>
#include  <hw_ints.h>
#include  <uart.h>
#include  <hw_watchdog.h>
#include  <watchdog.h>

unsigned char GucFlag = 0;//定义协调器与PC机握手成功标识变量
unsigned char GucIfSuccess=0;//组网成功标志变量
static RADIO_FLAGS local_radio_flags;
RADIO_STATE_ENUM radio_state;
UINT8 halMacTimerOflow;   //assumed incremented by interrupt service

/***********************************************************
LRWPAN异步中断下，接收数组变量定义
***********************************************************/
#ifdef  LRWPAN_ASYNC_INTIO
static volatile BYTE serio_rxBuff[LRWPAN_ASYNC_RX_BUFSIZE];
static volatile BYTE serio_rxHead, serio_rxTail;
#endif
/***********************************************************
开发板底层初始化，包括UART口与定时器的初始化
***********************************************************/
void halInit(void){
    radio_state = RADIO_STATE_OFF;
	local_radio_flags.val = 0;
	halInitUart();
	halInitMACTimer();
  }
/***********************************************************
开发板与PC机通信接口UART的初始化
***********************************************************/
void halInitUart(void) {
    SysCtlPeriEnable(SYSCTL_PERIPH_UART0);      /*  使能UART0模块               */
    SysCtlPeriEnable(U0TX_PERIPH);              /*  使能UART0所在的GPIOG端口    */

    GPIOPinTypeUART(U0RX_PORT,                  /*  配置PG0和PG1为UART功能      */
                    U0RX_PIN | U0TX_PIN);

    UARTConfigSet(UART0_BASE ,
                  9600 ,                                /*  波特率：9600                */
                  UART_CONFIG_WLEN_8 |                  /*  数据位：8                   */
                  UART_CONFIG_STOP_ONE |                /*  停止位：1                   */
                  UART_CONFIG_PAR_NONE);                /*  校验位：无                  */

    UARTEnable(UART0_BASE);                             /*  使能UART0端口 */  
 	HWREG(UART0_BASE+UART_O_LCRH) &=(~(UART_LCRH_FEN));    //设置FIFO 为传统的1字节深度  
  	UARTEnable(UART0_BASE); 
	                                //使能发送和接收
#ifdef LRWPAN_ASYNC_INTIO
    //定义UART接收中断
    UARTFIFOLevelSet(UART0_BASE,                            //  设置发送和接收FIFO深度
                     UART_FIFO_TX4_8,                       //  发送FIFO为2/8深度（4B）
                     UART_FIFO_RX6_8);                      //  接收FIFO为6/8深度（12B）
  	UARTIntDisable(UART0_BASE ,                          /* 失能接收中断*/
                  UART_INT_RX );
  	serio_rxHead = 0;                 
  	serio_rxTail = 0;
  	UARTIntEnable(UART0_BASE ,UART_INT_RX | UART_INT_RT ); /*  使能接收中断   */                
  	IntEnable(INT_UART0);                               /*  使能UART0总中断             */ 
  	IntMasterEnable();                                  /*  使能处理器中断              */
  	UARTEnable(UART0_BASE);                             /*  使能UART0端口               */
#endif //LRWPAN_ASYNC_INTIO
}

/***********************************************************
异步IO模式下，开发板与PC机接收数据处理函数
***********************************************************/
#ifdef  LRWPAN_ASYNC_INTIO
	//get a character from serial port, uses interrupt driven IO
	char halGetch(void){
    	while(serio_rxHead ==serio_rxTail );
    	serio_rxTail++;
   		if (serio_rxTail == LRWPAN_ASYNC_RX_BUFSIZE) serio_rxTail = 0;
   		return (serio_rxBuff[serio_rxTail]);
	}
	BOOL halGetchRdy(void){
 		return(serio_rxTail != serio_rxHead);
 	}
#else //LRWPAN_ASYNC_INTIO
/***********************************************************
非异步IO模式下，开发板与PC机收发数据处理函数
***********************************************************/
void  UART_Puts(const  char  *s)
{
    char  c;
    for (;;)
    {
        c  =  *(s++);

        if (c  ==  '\0')
        {
            break;
        }
       UARTCharPut(UART0_BASE , c);
    }
}
/***********************************************************
从UART口获取一字节数据
***********************************************************/
char halGetch(void){
  char  tmp;
// while (UART_FR_RXFF);           //RXFF is UART接收FIFO满
  while((HWREG(UART0_BASE+UART_O_FR))&UART_FR_RXFE)
 //tmp = (0xFF & UART_O_DR);
   tmp =UARTCharGet(UART0_BASE);
 return(tmp);                       // return the formal parameter   ,is the way right ?
}
/***********************************************************
UART口接收数据状态
***********************************************************/
BOOL halGetchRdy(void){
//return(UART_FR_RXFF);
  return((HWREG(UART0_BASE+UART_O_FR))&UART_FR_RXFE);
 }
#endif

/***********************************************************
输入一个字符至串口UART
***********************************************************/
void halPutch(char c)
{
  UARTCharPut(UART0_BASE, c);
}
/***********************************************************
根据串口状态，输入一个字符至串口UART
***********************************************************/
void halRawPut(char c){
   /*while(!(HWREG(UART0_BASE+UART_O_FR))&UART_FR_TXFE);	     //判断TXFE位的状态
  HWREG(UART0_BASE+UART_O_DR)= c;	*/
  while((HWREG(UART0_BASE+UART_O_FR))&UART_FR_TXFE)
  UARTCharPut(UART0_BASE, c);
}
/***********************************************************
异步IO模式下，串口UART接收中断服务函数
***********************************************************/
#ifdef  LRWPAN_ASYNC_INTIO
void  UART0_ISR(void)
{
    char c;
    unsigned long ulStatus;
 
    ulStatus = UARTIntStatus(UART0_BASE, true);             //  读取当前中断状态
    UARTIntClear(UART0_BASE, ulStatus);                     //  清除中断状态

    if ((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT))   //  若是接收中断或者
    {                                                           //      接收超时中断
        for (;;)
        {
            if (!UARTCharsAvail(UART0_BASE)) break;         //  若接收FIFO里无字符则跳出
            c = UARTCharGetNonBlocking(UART0_BASE);                  //  从接收FIFO里读取字符

            if (c == '\r')
            {
                UARTCharPut(UART0_BASE, '\r');              //  回显回车换行<CR><LF>
                UARTCharPut(UART0_BASE, '\n');
                break;
            }

            if (isprint(c))                                 //  若是可打印字符
            {
                UARTCharPut(UART0_BASE, c);             //  回显
				if(c=='!') GucFlag=1;	//握手成功信号
     			serio_rxHead++;
     			if (serio_rxHead == LRWPAN_ASYNC_RX_BUFSIZE ) serio_rxHead = 0;
     			serio_rxBuff[serio_rxHead] =c;
            }
        }
    }
 /*unsigned long ulStatus;

   ulStatus = UARTIntStatus(UART0_BASE, true); //  读取当前中断状态
   UARTIntClear(UART0_BASE, ulStatus);  //  清除中断状态
   if ((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT)) 
	   UARTCharPut(UART0_BASE,'?');	

   if (HWREG(UART0_BASE+UART_O_FR)&UART_FR_RXFE ) {
     serio_rxHead++;
     if (serio_rxHead == LRWPAN_ASYNC_RX_BUFSIZE ) serio_rxHead = 0;
     serio_rxBuff[serio_rxHead] = UARTCharGet(UART0_BASE);
	 //UARTCharPut(UART0_BASE,serio_rxBuff[serio_rxHead]);
  }*/
}
#endif //LRWPAN_ASYNC_INTIO
/***********************************************************
定时器Time0作为MAC定时器，初始化函数
***********************************************************/
//Timer0 used as MAC timer, Timer3  as the user periodic interrupt timer
void  timer0Init(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);             /*  使能定时器模块              */
    TimerConfigure(T0_B_CFG_16_PER);                    /*  配置TimerB为16位周期定时器  */
    TimerPrescaleSet(T0_B , 95);                        /*  预先进行96分频             */
    TimerLoadSet(T0_B , 0xFFFF);                        /*  设置定时器初值             */
    TimerIntEnable(T0_B_INT_TIMEOUT);                   /*  使能TimerB超时中断         */
    IntEnable(INT_TIMER0B);                             /*  使能TimerB中断             */
}
/***********************************************************
定时器Time2作为用户中断定时器，初始化函数
***********************************************************/
void  timer2Init(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_TIMER2);             /*  使能定时器模块              */
    TimerConfigure(T2_B_CFG_16_PER);                    /*  配置TimerB为16位周期定时器  */
    TimerPrescaleSet(T2_B , 47);                        /*  预先进行10分频             */
    TimerLoadSet(T2_B , 0xFFFF);                       /*  设置定时器初值              */
    TimerIntEnable(T2_B_INT_TIMEOUT);                   /*  使能TimerB超时中断          */
    IntEnable(INT_TIMER2B);                             /*  使能TimerB中断              */
    TimerEnable(T2_B);                                  /*  使能TimerB                  */
}
/***********************************************************
开发板hal层定时器初始化，Time0与Time2
***********************************************************/
void halInitMACTimer(void) {
   halMacTimerOflow =0;
   //setup Timer0
   timer0Init(); 
#ifdef LRWPAN_ENABLE_SLOW_TIMER
    timer2Init();
#endif //LRWPAN_ENABLE_SLOW_TIMER
   TimerEnable(T0_B);                                /*  使能TimerB                  */
}
/***********************************************************
开发板hal层服务关闭
***********************************************************/
void halShutdown(void){
  //disable timer interrupts so that they don't wake us up
    TimerIntDisable( T0_B_INT_TIMEOUT) ;                         
#ifdef LRWPAN_ENABLE_SLOW_TIMER
    TimerIntDisable( T2_B_INT_TIMEOUT) ;
#endif	//LRWPAN_ENABLE_SLOW_TIMER
    halDisableRadio();
}
/***********************************************************
开发板hal层服务唤醒
***********************************************************/
void halWarmstart(void){
//enable timer interrupts 
   TimerIntEnable(T0_B_INT_TIMEOUT);                   /*  使能TimerB超时中断          */ 
#ifdef LRWPAN_ENABLE_SLOW_TIMER
   TimerIntEnable(T2_B_INT_TIMEOUT);                   /*  使能TimerB超时中断          */
 #endif
}
/***********************************************************
将系统节拍转换位uS
//only works as long as SYMBOLS_PER_MAC_TICK is not less than 1
***********************************************************/
UINT32 halMacTicksToUs(UINT32 ticks){
   UINT32 rval;
    
   rval =  (ticks/SYMBOLS_PER_MAC_TICK())* (1000000/LRWPAN_SYMBOLS_PER_SECOND);
   return(rval);
}
/***********************************************************
开发板hal层获取MAC定时器值
//read the macTimer, assumes interrupts, Timer0 are enabled! 
***********************************************************/
UINT32 halGetMACTimer(void)
{
     UINT32  rval;

  	do {
     	rval = (0xFFFF-TimerValueGet(T0_B));   //get current timer0 count ;
    	DISABLE_GLOBAL_INTERRUPT(); 
    	if (rval > 2) { //ok, trust that the Oflow byte is correctly incremented by ISR   
      		break;
    	}
   		ENABLE_GLOBAL_INTERRUPT();  //renable global interrupt, allow to tick some more
  	}while(1);

   	rval  += ((UINT32) halMacTimerOflow) << 16;
   	ENABLE_GLOBAL_INTERRUPT();
   	return (rval);
}

/***********************************************************
开发板hal层中断服务调用以获取MAC定时器值
***********************************************************/
#ifdef LRWPAN_COMPILER_NO_RECURSION
UINT32 halISRGetMACTimer(void)
{
     UINT8   iStatus;
     UINT32  rval;
    
	do {
     	rval = TMR0L;      //freeze timer 0
     	rval  +=  ((UINT16) TMR0H  << 8);
   		DISABLE_GLOBAL_INTERRUPT(); 
    	if (rval > 2) { //ok, trust that the Oflow byte is correctly incremented by ISR
      	break;
    	}
   		ENABLE_GLOBAL_INTERRUPT();  //renable global interrupt, allow to tick some more
  	}while(1);
   //at this point, global interrupts are disabled, read overflow value
   	rval  += ((UINT32) halMacTimerOflow) << 16;
   	ENABLE_GLOBAL_INTERRUPT();
   	return (rval);
}
#endif //LRWPAN_COMPILER_NO_RECURSION
/***********************************************************
用于延时操作，返回值为定时器当前值与X之间的差值
***********************************************************/
UINT32 halMACTimerNowDelta(UINT32 x){
   UINT32 now;
   now = halGetMACTimer();
   now = (now - x);
   now = now & (UINT32) MACTIMER_MAX_VALUE;
   return(now);
}
/***********************************************************
开发板hal层复制len长度至目的变量
***********************************************************/
void halUtilMemCopy(BYTE *dst, BYTE *src, BYTE len) {
	while (len) {
		*dst = *src;
		dst++;src++;
		len--;
	}
}
/***********************************************************
开发板hal层获取设备的IEEE扩展地址
***********************************************************/
void halGetProcessorIEEEAddress(BYTE *buf) {

	buf[0] = aExtendedAddress_B0;
	buf[1] = aExtendedAddress_B1;
	buf[2] = aExtendedAddress_B2;
	buf[3] = aExtendedAddress_B3;
	buf[4] = aExtendedAddress_B4;
	buf[5] = aExtendedAddress_B5;
	buf[6] = aExtendedAddress_B6;
	buf[7] = aExtendedAddress_B7;

}
/***********************************************************
开发板hal层软件延时函数
//this is a software delay loop, not meant for precision
***********************************************************/
void halWaitMs (UINT32 msecs)
{
	UINT32	i, cnt;
 
    cnt = msecs;
	do {
		i = 20;
		do {
			halWaitUs(50);
		} while(--i);
	} while(--cnt);
}
/***********************************************************
开发板hal层空闲函数，可以进行有关的看门狗操作
***********************************************************/
void halIdle(void) {
  //CLRWDT(); //clear the watchdog timer	
}
/***********************************************************
开发板hal层硬件休眠函数
***********************************************************/
void halSleep(UINT32 msecs) {
   ENABLE_WDT() ;
  // IDLEN = 0;                  //Device enters Sleep mode on SLEEP instruction
   SLEEP();
  // NOP();
   DISABLE_WDT();
}
/***********************************************************
Timer0B的中断服务函数 
***********************************************************/
void  Timer0B_ISR(void)
{
    TimerIntClear(T0_B_INT_TIMEOUT);    /*  清除TimerB超时中断，重要！ */
    halMacTimerOflow++;
  /* 
   evbRadioIntCallback();
   usrIntCallback();*/
}
/***********************************************************
Timer2B的中断服务函数 
***********************************************************/
void  Timer2B_ISR(void)
{
#ifdef LRWPAN_ENABLE_SLOW_TIMER
   	TimerIntClear(T2_B_INT_TIMEOUT); 
   	evbIntCallback(); // call evb 
   	usrSlowTimerInt();
  	/* 
   	evbRadioIntCallback();
   	usrIntCallback();*/
#endif //LRWPAN_ENABLE_SLOW_TIMER
 }
