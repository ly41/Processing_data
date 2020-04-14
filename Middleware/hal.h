/*
  V0.1 Initial Release   10/July/2006  RBR

*/
#ifndef HAL_H
#define HAL_H

#include "compiler.h"
#include "lrwpan_config.h"         //user configurations
#include "lrwpan_common_types.h"   //types common acrosss most files
#include "evbConfig.h"             //has Crystal frequency, other evboard specifics
#include  <interrupt.h>

#define PACKET_FOOTER_SIZE  2
#define SYMBOLS_PER_MAC_TICK()        1  //为4是因为CORTEX中分频不能直接得到16US的周期值

#define SYMBOLS_TO_MACTICKS(x) ((UINT32)x/(UINT32)SYMBOLS_PER_MAC_TICK())  
#define MSECS_TO_MACTICKS(x)   ((UINT32)x*((UINT32)LRWPAN_SYMBOLS_PER_SECOND/(UINT32)1000)*(UINT32)SYMBOLS_PER_MAC_TICK())
#define MACTIMER_MAX_VALUE     0x00FFFFFF   //24 bit counter

#define halMACTimerDelta(x,y) (((UINT32)x-((UINT32)y))& (UINT32)MACTIMER_MAX_VALUE)
#define DISABLE_GLOBAL_INTERRUPT()             IntMasterDisable()
#define ENABLE_GLOBAL_INTERRUPT()              IntMasterEnable()
#define HAL_SUSPEND(x) 
/***********************************************************
软件实现延时操作，uS
***********************************************************/
#define	halWaitUs(x) { unsigned char _dcnt; \
		_dcnt = ((x* FOSC/1000000)/(12)); \
		while(--_dcnt != 0) \
		continue; }
/***********************************************************
函数声明
***********************************************************/
UINT32 halMACTimerNowDelta(UINT32 x);
void halIdle (void);
void evbRadioIntCallback(void);
void halDisableRadio(void);

#endif //HAL_H


