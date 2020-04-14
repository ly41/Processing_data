/*
 * "Copyright (c) 2006 Robert B. Reese ("AUTHOR")"
 * All rights reserved.
 * (R. Reese, reese@ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHOR" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHOR"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHOR" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHOR" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 * Files in this software distribution may have different usage
 * permissions, see the header in each file. Some files have NO permission
 * headers since parts of the original sources in these files
 * came from vendor  sources with no usage restrictions.
 *
 */

/*
V0.2 added PC-based binding         21/July/2006  RBR
V0.1 Initial Release                10/July/2006  RBR

*/
#ifndef HALSTACK_H
#define HALSTACK_H

/***********************************************************
��Ƶģ��״̬ö�����Ͷ���
***********************************************************/
typedef enum _RADIO_STATE_ENUM {
  RADIO_STATE_OFF,
  RADIO_STATE_ON
}RADIO_STATE_ENUM;

//defined such that timer0 has a period of 1 symbol, or 16 us
//FOSC assumed to be some multiple of 4 MHZ
#define SYMBOL_FREQ  62500      //equals to symbols period of 16us
/***********************************************************
TIMER0  ��ƵҪ�õ�16US�ķ�������
***********************************************************/
#define T0_PRESCALE ((FOSC/(SYMBOL_FREQ/SYMBOLS_PER_MAC_TICK())))            //prescale is computed as 200 
#define MAX_WAIT_MS  (TO_PRESCALE*65536*1000)/FOSC  //maximum wait in milliseconds

/***********************************************************
MACЭ��ջ��ʱ��TIMER0B  �жϲ����궨��
***********************************************************/
#define  T0_B                   TIMER0_BASE , TIMER_B
#define  T0_B_CFG_16_PER        TIMER0_BASE , TIMER_CFG_16_BIT_PAIR | TIMER_CFG_B_PERIODIC
#define  T0_B_INT_TIMEOUT      	TIMER0_BASE , TIMER_TIMB_TIMEOUT
/***********************************************************
Ӧ�ò��û���ʱ��TIMER2B  �жϲ����궨��
***********************************************************/
#define  T2_B                  	TIMER2_BASE , TIMER_B
#define  T2_B_CFG_16_PER        TIMER2_BASE , TIMER_CFG_16_BIT_PAIR | TIMER_CFG_B_PERIODIC
#define  T2_B_INT_TIMEOUT      	TIMER2_BASE , TIMER_TIMB_TIMEOUT
/***********************************************************
��������
***********************************************************/
void halInit(void);  //processor, board specific initializations
void halInitUart(void);  // do everything except baud rate setting.
extern void  UART_Puts(const  char  *s);
char halGetch(void);  //get a character from serial port
BOOL halGetchRdy(void);  //is a character available from the serial port?
void halPutch(char c);  //write a character to serial port
void halRawPut(char c);  //write a byte to serial port, no character interpretation
void halInitMACTimer(void); //init timer used for Radio timeouts
UINT32 halGetMACTimer(void); //return timer value
UINT32 halMACTimerNowDelta(UINT32 x);

#ifdef LRWPAN_COMPILER_NO_RECURSION
	UINT32 halISRGetMACTimer(void); //return timer value
#else
	#define halISRGetMACTimer() halGetMACTimer()
#endif //LRWPAN_COMPILER_NO_RECURSION

LRWPAN_STATUS_ENUM halInitRadio(PHY_FREQ_ENUM frequency, BYTE channel, RADIO_FLAGS radio_flags);
void halGetProcessorIEEEAddress(BYTE *buf);
void halSetRadioIEEEAddress(void );
LRWPAN_STATUS_ENUM halSetRadioIEEEFrequency(PHY_FREQ_ENUM frequency, BYTE channel);
void halSetRadioPANID(UINT16 panid);
void halSetRadioShortAddr(SADDR saddr);
LRWPAN_STATUS_ENUM halSendPacket(BYTE flen, BYTE *frm);
LRWPAN_STATUS_ENUM halSetChannel(BYTE channel);
UINT32 halMacTicksToUs(UINT32 x);
UINT8 halGetRandomByte(void);
void halSleep(UINT32 msecs);    //put processor to sleep
void halSuspend(UINT32 msecs);  //suspends process, intended for Win32, dummy on others
void halUtilMemCopy(BYTE *dst, BYTE *src, BYTE len);
void halWaitMs(UINT32 msecs);
void halShutdown(void);
void halWarmstart(void);
/***********************************************************
����ص���������
***********************************************************/
//call backs to PHY, MAC from HAL
void phyRxCallback(void);
void phyTxStartCallBack(void);
void phyTxEndCallBack(void);
void macRxCallback(BYTE *ptr, BYTE rssi);
void macTxCallback(void);
void evbIntCallback(void);  //Evaluation board slow timer interrupt callback
void usrIntCallback(void);   //general interrupt callback , when this is called depends on the HAL layer.
/***********************************************************
��ʱ���봮�ڳ�ʼ�����жϷ���������
***********************************************************/
void  timer0Init(void);
void  timer2Init(void);
void  Timer0B_ISR(void);
void  Timer2B_ISR(void);
void  UART0_ISR(void);
/***********************************************************
�û��Ͷ�ʱ���жϵ��ú�������
***********************************************************/
#ifdef LRWPAN_ENABLE_SLOW_TIMER
	void usrSlowTimerInt(void); //user interrupt slow timer interrupt callback
#endif //LRWPAN_ENABLE_SLOW_TIMER

#endif //HALSTACK_H

