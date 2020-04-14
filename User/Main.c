#include <includes.h>
#include "LM3S9b96_PinMap.h"




/*********************************************************************************************************
  EXTERNAL变量
*********************************************************************************************************/
extern unsigned char GucFlag;
extern unsigned char GucIfSuccess;

/*********************************************************************************************************
  VARIABLES 变量
*********************************************************************************************************/
static OS_STK  GstkStart[TASK_START_STK_SIZE];                      /*  启动任务的堆栈              */
                                                                        
static OS_STK  GstkForm[TASK_FORMNET_STK_SIZE];

static OS_STK  DisPlay[TASK_FORMNET_STK_SIZE]; 

/*********************************************************************************************************
   FUNCTION PROTOTYPES 函数声明
*********************************************************************************************************/
static void taskStart (void  *parg);                                    /*  The start task  启动任务    */
static void taskForm (void  *parg);
void display(void);

/*********************************************************************************************************
** Function name:           main	   
** Descriptions:            uC/OS移植模板	
** input parameters:        无
** output parameters:       无      
** Returned value:          无 
*********************************************************************************************************/
int main (void)
{
    intDisAll();                                                        /* Disable all the interrupts   */
                                                                        /* 关闭所有中断                 */

    OSInit();                                                           /*  Initialize the kernel of uC */
                                                                        /*  OS-II 初始化uC/OS-II的内核  */

    OSTaskCreate ( taskStart,                                          
		           (void *)0, 
				   &GstkStart[TASK_START_STK_SIZE-1], 
				   TASK_START_PRIO );                                   /*  Initialize the start task   */
                                                                        /*  初始化启动任务              */  
    OSStart(); 
 
							 /*  Start uC/OS-II 启动uC/OS-II */
    return(0) ;
}

/*********************************************************************************************************
** Function name:           Task_Start	   
** Descriptions:            Start task	
** input parameters:        *p_arg
** output parameters:       无      
** Returned value:          无 
*********************************************************************************************************/
static  void  taskStart (void  *parg)
{   
    (void)parg;
    targetInit();                                                       /*  Initialize the target's MCU */
                                                                        /*  初始化目标单片机            */
    #if OS_TASK_STAT_EN > 0
        OSStatInit();                                                   /*  Enable statistics           */
                                                                        /*  使能统计功能                */
    #endif

    /*
     *  Create the other tasks here. 在这里创建其他任务
     */
	OSTaskCreate (taskForm, (void *)0,   		
			      &GstkForm[TASK_FORMNET_STK_SIZE-1], 
				  TASK_FORMNET_PRIO);                                       /*  初始化taskLed任务           */  	
			        
    while (1) {                             
        OSTaskSuspend(OS_PRIO_SELF);                                    /*  The start task can be pended*/
                                                                        /*  here. 启动任务可在这里挂起  */
    }
}

/*********************************************************************************************************
  The other tasks 其他任务
*********************************************************************************************************/
/*
 *  Add the other tasks here . 在这里增加其他任务
 */

/*********************************************************************************************************
** Function name:           taskForm	   
** Descriptions:            建网任务	
** input parameters:        *parg
** output parameters:       无      
** Returned value:          无	 
*********************************************************************************************************/
static void taskForm (void  *parg)
{
    //判断握手信号是否成功
	EVB_LED1_OFF();
	/*while(1)
	{
		halPutch('?');
		halWaitMs(100);
		if(GucFlag==1) break;
	}  */
	GucIfSuccess = 0;
	aplFormNetwork();
	while(apsBusy()) {apsFSM();} //wait for finish
//	conPrintROMString("Network formed, waiting for RX\n");
	//向上位机软件发送组网成功标志
//	conPrintROMString("#00000&000$0000*0000000%\n");
	GucIfSuccess = 1;
	EVB_LED1_ON();
    aplSetMacMaxFrameRetries(0);
	while (1) {
		apsFSM();
		
	}
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

