#ifndef __TARGET_H 
#define __TARGET_H

#ifdef __cplusplus
	extern "C" {
#endif //__cplusplus

#include "includes.h"
/*********************************************************************************************************
  System Clock(CCLK) Setting   系统时钟(CCLK) 设定
  CCLK must be less than or equal to  20MHz/50MHz(depend on the max. cclk of the CPU)
  CCLK必须小于或等于20MHz/50MHz(根据单片机的最大CCLK而定)
  If PLL_EN=0, CCLK=EXT_CLK/CCLK_DIV, CCLK must <=20MHZ/50MHz 
  If PLL_EN>0, CCLK=200MHz/CCLK_DIV, CCLK must <=20MHZ/50MHz
*********************************************************************************************************/
#define  EXT_CLK            SYSCTL_XTAL_6MHZ	                        /*  external clock 外部时钟     */     
#define  PLL_EN      	    0                                           /*  1：Enable PLL  使能PLL      */
#define  CCLK_DIV           SYSCTL_SYSDIV_1                             /*  CCLK divider CCLK分频系数   */


/*********************************************************************************************************
  Setting of the target board's API function 
  目标板API函数设定
*********************************************************************************************************/
#define  TARGET_LED1_EN     0        			                /*  1:Enable LED1  使能LED1     */     
#define  TARGET_LED2_EN     0        			                /*  1:Enable LED2  使能LED2     */
#define  TARGET_LED3_EN     0        			                /*  1:Enable LED3  使能LED3     */
#define  TARGET_LED4_EN     0        			                /*  1:Enable LED4  使能LED4     */

#define  TARGET_BUZ_EN      0        		                        /*  1:Enable the buzzer         */

#define  TARGET_KEY1_EN     0  				                /*  1:Enable Key1  使能Key1     */
#define  TARGET_KEY2_EN     0  					        /*  1:Enable Key2  使能Key2     */
#define  TARGET_KEY3_EN     0  					        /*  1:Enable Key3  使能Key3     */
#define  TARGET_KEY4_EN     0  					        /*  1:Enable Key4  使能Key4     */

#define  TARGET_TMR0A_EN    0        			                /*  1：Eanble the Timer0A API 
                                                                            functions                   */
#if TARGET_LED1_EN > 0
#define LED1_SYSCTL  	SYSCTL_PERIPH_GPIOA 	                        /*  System control of LED1      */
#define LED1_GPIO_PORT  GPIO_PORTA_BASE     	                        /*  IO port of LED1             */  
#define LED1_PIN        GPIO_PIN_0				        /*  Pin number of LED1          */
#endif

#if TARGET_LED2_EN > 0
#define LED2_SYSCTL    	SYSCTL_PERIPH_GPIOA 	                        /*  System control of LED2      */
#define LED2_GPIO_PORT 	GPIO_PORTA_BASE     	                        /*  IO port of LED2             */
#define LED2_PIN       	GPIO_PIN_0          	                        /*  Pin number of LED2          */
#endif

#if TARGET_LED3_EN > 0
#define LED3_SYSCTL   	SYSCTL_PERIPH_GPIOA 	                        /*  System control of LED3      */
#define LED3_GPIO_PORT  GPIO_PORTA_BASE     	                        /*  IO port of LED3             */
#define LED3_PIN        GPIO_PIN_0     			                /*  Pin number of LED3          */
#endif

#if TARGET_LED4_EN > 0
#define LED4_SYSCTL  	SYSCTL_PERIPH_GPIOA		                /*  System control of LED4      */
#define LED4_GPIO_PORT  GPIO_PORTA_BASE    		                /*  IO port of LED4             */
#define LED4_PIN        GPIO_PIN_0         		                /*  Pin number of LED4          */
#endif

#if TARGET_BUZ_EN > 0
#define BUZ_SYSCTL      SYSCTL_PERIPH_GPIOA		                /*  System control of the buzzer*/
#define BUZ_GPIO_PORT   GPIO_PORTA_BASE     	                        /*  IO port of the buzzer       */  
#define BUZ_PIN         GPIO_PIN_0          	                        /*  Pin number of the buzzer    */
#endif

#if TARGET_KEY1_EN > 0 
#define KEY1_SYSCTL    	SYSCTL_PERIPH_GPIOA		                /*  System control of Key1      */ 
#define KEY1_GPIO_PORT  GPIO_PORTA_BASE     	                        /*  IO port of Key1             */
#define KEY1_PIN        GPIO_PIN_0          	                        /*  Pin number of Key1          */
#endif

#if TARGET_KEY2_EN > 0  
#define KEY2_SYSCTL  	SYSCTL_PERIPH_GPIOA 	                        /*  System control of Key2      */
#define KEY2_GPIO_PORT  GPIO_PORTA_BASE     	                        /*  IO port of Key2             */
#define KEY2_PIN        GPIO_PIN_0          	                        /*  Pin number of Key2          */
#endif

#if TARGET_KEY3_EN > 0
#define KEY3_SYSCTL  	SYSCTL_PERIPH_GPIOA 	                        /*  System control of Key3      */
#define KEY3_GPIO_PORT  GPIO_PORTA_BASE     	                        /*  IO port of Key3             */
#define KEY3_PIN       	GPIO_PIN_0          	                        /*  Pin number of Key3          */
#endif

#if TARGET_KEY4_EN > 0
#define KEY4_SYSCTL 	SYSCTL_PERIPH_GPIOA		                /*  System control of Key4      */
#define KEY4_GPIO_PORT  GPIO_PORTA_BASE     	                        /*  IO port of Key4             */
#define KEY4_PIN        GPIO_PIN_0          	                        /*  Pin number of Key4          */
#endif

/**********************************************
 将较长的标识符定义成较短的形式
***********************************************/

#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut         GPIOPinTypeGPIOOutput


#ifndef LRWPAN_COORDINATOR
	#define PING_DELAY   2  //wait before bouncing back
#else
	#define PING_DELAY   0 //coordinator does not wait
#endif //LRWPAN_COORDINATOR

#define RX_PING_TIMEOUT     5    //seconds
/*********************************************************************************************************
  Function Prototypes 函数原型                                       
*********************************************************************************************************/
#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN >0 ) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    extern void ledInit(void);
#endif //(TARGET_LED1_EN > 0) || (TARGET_LED2_EN >0 ) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)

#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    extern void ledOn(INT8U  ucLed);
#endif //(TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)

#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    extern void ledOff(INT8U  ucLed);
#endif //(TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)

#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    extern void ledToggle(INT8U  ucLed);
#endif //(TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)

#if TARGET_BUZ_EN > 0
    extern void buzInit(void);
#endif //TARGET_BUZ_EN

#if TARGET_BUZ_EN > 0
    extern void buzOn(void);
#endif //TARGET_BUZ_EN

#if TARGET_BUZ_EN > 0
    extern void buzOff(void);
#endif //TARGET_BUZ_EN

#if TARGET_BUZ_EN > 0
    extern void buzToggle(void);    
#endif //TARGET_BUZ_EN

#if (TARGET_KEY1_EN > 0) ||  (TARGET_KEY2_EN > 0) || (TARGET_KEY3_EN > 0) || (TARGET_KEY4_EN > 0)
    extern void keyInit(void);    
#endif //(TARGET_KEY1_EN > 0) ||  (TARGET_KEY2_EN > 0) || (TARGET_KEY3_EN > 0) || (TARGET_KEY4_EN > 0)

#if (TARGET_KEY1_EN > 0) ||  (TARGET_KEY2_EN > 0) || (TARGET_KEY3_EN > 0) || (TARGET_KEY4_EN > 0)
    extern INT8U keyRead(void);    
#endif //(TARGET_KEY1_EN > 0) ||  (TARGET_KEY2_EN > 0) || (TARGET_KEY3_EN > 0) || (TARGET_KEY4_EN > 0)

#if TARGET_TMR0A_EN > 0
    extern void timer0AInit(INT32U  ulTick, INT8U  ucPrio);
#endif //TARGET_TMR0A_EN

#if TARGET_TMR0A_EN > 0
    extern void timer0AISR(void);
#endif //TARGET_TMR0A_EN

static  void  tickInit (void);
extern void tickISRHandler(void);
extern void targetInit(void);
extern void intDisAll(void);
extern void JTAG_Wait(void); 

#ifdef __cplusplus
    }
#endif

#endif

/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
