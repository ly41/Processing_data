#ifndef  __INCLUDES_H
#define  __INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif
  
/*********************************************************************************************************
  Date types(Compiler specific)  数据类型（和编译器相关）                
*********************************************************************************************************/
typedef unsigned char   uint8;                                          /* Unsigned  8 bit quantity     */
typedef signed   char   int8;                                           /* Signed    8 bit quantity     */
typedef unsigned short  uint16;                                         /* Unsigned  16 bit quantity    */
typedef signed   short  int16;                                          /* Signed    16 bit quantity    */
typedef unsigned int    uint32;                                         /* Unsigned  32 bit quantity    */
typedef signed   int    int32;                                          /* Signed    32 bit quantity    */
typedef float           fp32;                                           /* Single    precision          */
                                                                        /* floating  point              */
typedef double          fp64;                                           /* Double    precision          */
                                                                        /* floating  point              */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL  0
#endif


/*********************************************************************************************************
  Standard header files 标准头文件
*********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


/*********************************************************************************************************
  Driver's header files  驱动库头文件 
*********************************************************************************************************/
#include <hw_adc.h>
#include <hw_can.h>
#include <hw_comp.h>
#include <hw_ethernet.h>
#include <hw_flash.h>
#include <hw_gpio.h>
#include <hw_hibernate.h>
#include <hw_i2c.h>
#include <hw_ints.h>
#include <hw_memmap.h>
#include <hw_nvic.h>
#include <hw_pwm.h>
#include <hw_qei.h>
#include <hw_ssi.h>
#include <hw_sysctl.h>
#include <hw_timer.h>
#include <hw_types.h>
#include <hw_uart.h>
#include <hw_watchdog.h>


#include <adc.h>
#include <can.h>
#include <comp.h>
#include <cpu.h>
#include <debug.h>
#include <ethernet.h>
#include <flash.h>
#include <gpio.h>
#include <hibernate.h>
#include <i2c.h>
#include <interrupt.h>
#include <mpu.h>
#include <pin_map.h>
#include <pwm.h>
#include <qei.h>
#include <ssi.h>
#include <sysctl.h>
#include <systick.h>
#include <timer.h>
#include <uart.h>
#include <watchdog.h>


/*********************************************************************************************************
  Port's header files 移植头文件
*********************************************************************************************************/
#include <os_cpu.h>
#include <os_cfg.h>
#include <ucos_ii.h>
#include "..\Target\Target.h"   


/*********************************************************************************************************
  User's header files 用户头文件
*********************************************************************************************************/
#include <Main.h>
#include "msstate_lrwpan.h"
#include "../Library/inc/hw_memmap.h"
#include "../Library/inc/hw_types.h"
#include "../Library/inc/hw_gpio.h"
#include "../Library/driverlib/sysctl.h"
#include "../Library/driverlib/i2c.h"
#include "../Library/driverlib/gpio.h"
#include "../Library/driverlib/epi.h"
#include "../Library/driverlib/debug.h"
#include "../Library/drivers/set_pinout.h"
#include "../Library/drivers/camerafpga.h"
#include "../Library/inc/hw_ints.h"
#include "../Library/driverlib/interrupt.h"
#include "../Library/driverlib/sysctl.h"
#include "../Library/driverlib/timer.h"
#include "../Library/grlib/grlib.h"
#include "../Library/drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "../Library/driverlib/interrupt.h"
      
#ifdef __cplusplus
}
#endif

#endif
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
