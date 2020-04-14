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

unsigned char GucFlag = 0;//����Э������PC�����ֳɹ���ʶ����
unsigned char GucIfSuccess=0;//�����ɹ���־����
static RADIO_FLAGS local_radio_flags;
RADIO_STATE_ENUM radio_state;
UINT8 halMacTimerOflow;   //assumed incremented by interrupt service

/***********************************************************
LRWPAN�첽�ж��£����������������
***********************************************************/
#ifdef  LRWPAN_ASYNC_INTIO
static volatile BYTE serio_rxBuff[LRWPAN_ASYNC_RX_BUFSIZE];
static volatile BYTE serio_rxHead, serio_rxTail;
#endif
/***********************************************************
������ײ��ʼ��������UART���붨ʱ���ĳ�ʼ��
***********************************************************/
void halInit(void){
    radio_state = RADIO_STATE_OFF;
	local_radio_flags.val = 0;
	halInitUart();
	halInitMACTimer();
  }
/***********************************************************
��������PC��ͨ�Žӿ�UART�ĳ�ʼ��
***********************************************************/
void halInitUart(void) {
    SysCtlPeriEnable(SYSCTL_PERIPH_UART0);      /*  ʹ��UART0ģ��               */
    SysCtlPeriEnable(U0TX_PERIPH);              /*  ʹ��UART0���ڵ�GPIOG�˿�    */

    GPIOPinTypeUART(U0RX_PORT,                  /*  ����PG0��PG1ΪUART����      */
                    U0RX_PIN | U0TX_PIN);

    UARTConfigSet(UART0_BASE ,
                  9600 ,                                /*  �����ʣ�9600                */
                  UART_CONFIG_WLEN_8 |                  /*  ����λ��8                   */
                  UART_CONFIG_STOP_ONE |                /*  ֹͣλ��1                   */
                  UART_CONFIG_PAR_NONE);                /*  У��λ����                  */

    UARTEnable(UART0_BASE);                             /*  ʹ��UART0�˿� */  
 	HWREG(UART0_BASE+UART_O_LCRH) &=(~(UART_LCRH_FEN));    //����FIFO Ϊ��ͳ��1�ֽ����  
  	UARTEnable(UART0_BASE); 
	                                //ʹ�ܷ��ͺͽ���
#ifdef LRWPAN_ASYNC_INTIO
    //����UART�����ж�
    UARTFIFOLevelSet(UART0_BASE,                            //  ���÷��ͺͽ���FIFO���
                     UART_FIFO_TX4_8,                       //  ����FIFOΪ2/8��ȣ�4B��
                     UART_FIFO_RX6_8);                      //  ����FIFOΪ6/8��ȣ�12B��
  	UARTIntDisable(UART0_BASE ,                          /* ʧ�ܽ����ж�*/
                  UART_INT_RX );
  	serio_rxHead = 0;                 
  	serio_rxTail = 0;
  	UARTIntEnable(UART0_BASE ,UART_INT_RX | UART_INT_RT ); /*  ʹ�ܽ����ж�   */                
  	IntEnable(INT_UART0);                               /*  ʹ��UART0���ж�             */ 
  	IntMasterEnable();                                  /*  ʹ�ܴ������ж�              */
  	UARTEnable(UART0_BASE);                             /*  ʹ��UART0�˿�               */
#endif //LRWPAN_ASYNC_INTIO
}

/***********************************************************
�첽IOģʽ�£���������PC���������ݴ�����
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
���첽IOģʽ�£���������PC���շ����ݴ�����
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
��UART�ڻ�ȡһ�ֽ�����
***********************************************************/
char halGetch(void){
  char  tmp;
// while (UART_FR_RXFF);           //RXFF is UART����FIFO��
  while((HWREG(UART0_BASE+UART_O_FR))&UART_FR_RXFE)
 //tmp = (0xFF & UART_O_DR);
   tmp =UARTCharGet(UART0_BASE);
 return(tmp);                       // return the formal parameter   ,is the way right ?
}
/***********************************************************
UART�ڽ�������״̬
***********************************************************/
BOOL halGetchRdy(void){
//return(UART_FR_RXFF);
  return((HWREG(UART0_BASE+UART_O_FR))&UART_FR_RXFE);
 }
#endif

/***********************************************************
����һ���ַ�������UART
***********************************************************/
void halPutch(char c)
{
  UARTCharPut(UART0_BASE, c);
}
/***********************************************************
���ݴ���״̬������һ���ַ�������UART
***********************************************************/
void halRawPut(char c){
   /*while(!(HWREG(UART0_BASE+UART_O_FR))&UART_FR_TXFE);	     //�ж�TXFEλ��״̬
  HWREG(UART0_BASE+UART_O_DR)= c;	*/
  while((HWREG(UART0_BASE+UART_O_FR))&UART_FR_TXFE)
  UARTCharPut(UART0_BASE, c);
}
/***********************************************************
�첽IOģʽ�£�����UART�����жϷ�����
***********************************************************/
#ifdef  LRWPAN_ASYNC_INTIO
void  UART0_ISR(void)
{
    char c;
    unsigned long ulStatus;
 
    ulStatus = UARTIntStatus(UART0_BASE, true);             //  ��ȡ��ǰ�ж�״̬
    UARTIntClear(UART0_BASE, ulStatus);                     //  ����ж�״̬

    if ((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT))   //  ���ǽ����жϻ���
    {                                                           //      ���ճ�ʱ�ж�
        for (;;)
        {
            if (!UARTCharsAvail(UART0_BASE)) break;         //  ������FIFO�����ַ�������
            c = UARTCharGetNonBlocking(UART0_BASE);                  //  �ӽ���FIFO���ȡ�ַ�

            if (c == '\r')
            {
                UARTCharPut(UART0_BASE, '\r');              //  ���Իس�����<CR><LF>
                UARTCharPut(UART0_BASE, '\n');
                break;
            }

            if (isprint(c))                                 //  ���ǿɴ�ӡ�ַ�
            {
                UARTCharPut(UART0_BASE, c);             //  ����
				if(c=='!') GucFlag=1;	//���ֳɹ��ź�
     			serio_rxHead++;
     			if (serio_rxHead == LRWPAN_ASYNC_RX_BUFSIZE ) serio_rxHead = 0;
     			serio_rxBuff[serio_rxHead] =c;
            }
        }
    }
 /*unsigned long ulStatus;

   ulStatus = UARTIntStatus(UART0_BASE, true); //  ��ȡ��ǰ�ж�״̬
   UARTIntClear(UART0_BASE, ulStatus);  //  ����ж�״̬
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
��ʱ��Time0��ΪMAC��ʱ������ʼ������
***********************************************************/
//Timer0 used as MAC timer, Timer3  as the user periodic interrupt timer
void  timer0Init(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);             /*  ʹ�ܶ�ʱ��ģ��              */
    TimerConfigure(T0_B_CFG_16_PER);                    /*  ����TimerBΪ16λ���ڶ�ʱ��  */
    TimerPrescaleSet(T0_B , 95);                        /*  Ԥ�Ƚ���96��Ƶ             */
    TimerLoadSet(T0_B , 0xFFFF);                        /*  ���ö�ʱ����ֵ             */
    TimerIntEnable(T0_B_INT_TIMEOUT);                   /*  ʹ��TimerB��ʱ�ж�         */
    IntEnable(INT_TIMER0B);                             /*  ʹ��TimerB�ж�             */
}
/***********************************************************
��ʱ��Time2��Ϊ�û��ж϶�ʱ������ʼ������
***********************************************************/
void  timer2Init(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_TIMER2);             /*  ʹ�ܶ�ʱ��ģ��              */
    TimerConfigure(T2_B_CFG_16_PER);                    /*  ����TimerBΪ16λ���ڶ�ʱ��  */
    TimerPrescaleSet(T2_B , 47);                        /*  Ԥ�Ƚ���10��Ƶ             */
    TimerLoadSet(T2_B , 0xFFFF);                       /*  ���ö�ʱ����ֵ              */
    TimerIntEnable(T2_B_INT_TIMEOUT);                   /*  ʹ��TimerB��ʱ�ж�          */
    IntEnable(INT_TIMER2B);                             /*  ʹ��TimerB�ж�              */
    TimerEnable(T2_B);                                  /*  ʹ��TimerB                  */
}
/***********************************************************
������hal�㶨ʱ����ʼ����Time0��Time2
***********************************************************/
void halInitMACTimer(void) {
   halMacTimerOflow =0;
   //setup Timer0
   timer0Init(); 
#ifdef LRWPAN_ENABLE_SLOW_TIMER
    timer2Init();
#endif //LRWPAN_ENABLE_SLOW_TIMER
   TimerEnable(T0_B);                                /*  ʹ��TimerB                  */
}
/***********************************************************
������hal�����ر�
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
������hal�������
***********************************************************/
void halWarmstart(void){
//enable timer interrupts 
   TimerIntEnable(T0_B_INT_TIMEOUT);                   /*  ʹ��TimerB��ʱ�ж�          */ 
#ifdef LRWPAN_ENABLE_SLOW_TIMER
   TimerIntEnable(T2_B_INT_TIMEOUT);                   /*  ʹ��TimerB��ʱ�ж�          */
 #endif
}
/***********************************************************
��ϵͳ����ת��λuS
//only works as long as SYMBOLS_PER_MAC_TICK is not less than 1
***********************************************************/
UINT32 halMacTicksToUs(UINT32 ticks){
   UINT32 rval;
    
   rval =  (ticks/SYMBOLS_PER_MAC_TICK())* (1000000/LRWPAN_SYMBOLS_PER_SECOND);
   return(rval);
}
/***********************************************************
������hal���ȡMAC��ʱ��ֵ
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
������hal���жϷ�������Ի�ȡMAC��ʱ��ֵ
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
������ʱ����������ֵΪ��ʱ����ǰֵ��X֮��Ĳ�ֵ
***********************************************************/
UINT32 halMACTimerNowDelta(UINT32 x){
   UINT32 now;
   now = halGetMACTimer();
   now = (now - x);
   now = now & (UINT32) MACTIMER_MAX_VALUE;
   return(now);
}
/***********************************************************
������hal�㸴��len������Ŀ�ı���
***********************************************************/
void halUtilMemCopy(BYTE *dst, BYTE *src, BYTE len) {
	while (len) {
		*dst = *src;
		dst++;src++;
		len--;
	}
}
/***********************************************************
������hal���ȡ�豸��IEEE��չ��ַ
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
������hal�������ʱ����
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
������hal����к��������Խ����йصĿ��Ź�����
***********************************************************/
void halIdle(void) {
  //CLRWDT(); //clear the watchdog timer	
}
/***********************************************************
������hal��Ӳ�����ߺ���
***********************************************************/
void halSleep(UINT32 msecs) {
   ENABLE_WDT() ;
  // IDLEN = 0;                  //Device enters Sleep mode on SLEEP instruction
   SLEEP();
  // NOP();
   DISABLE_WDT();
}
/***********************************************************
Timer0B���жϷ����� 
***********************************************************/
void  Timer0B_ISR(void)
{
    TimerIntClear(T0_B_INT_TIMEOUT);    /*  ���TimerB��ʱ�жϣ���Ҫ�� */
    halMacTimerOflow++;
  /* 
   evbRadioIntCallback();
   usrIntCallback();*/
}
/***********************************************************
Timer2B���жϷ����� 
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
