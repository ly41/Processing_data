#include <includes.h>
#include "LM3S9b96_PinMap.h"




/*********************************************************************************************************
  EXTERNAL����
*********************************************************************************************************/
extern unsigned char GucFlag;
extern unsigned char GucIfSuccess;

/*********************************************************************************************************
  VARIABLES ����
*********************************************************************************************************/
static OS_STK  GstkStart[TASK_START_STK_SIZE];                      /*  ��������Ķ�ջ              */
                                                                        
static OS_STK  GstkForm[TASK_FORMNET_STK_SIZE];

static OS_STK  DisPlay[TASK_FORMNET_STK_SIZE]; 

/*********************************************************************************************************
   FUNCTION PROTOTYPES ��������
*********************************************************************************************************/
static void taskStart (void  *parg);                                    /*  The start task  ��������    */
static void taskForm (void  *parg);
void display(void);

/*********************************************************************************************************
** Function name:           main	   
** Descriptions:            uC/OS��ֲģ��	
** input parameters:        ��
** output parameters:       ��      
** Returned value:          �� 
*********************************************************************************************************/
int main (void)
{
    intDisAll();                                                        /* Disable all the interrupts   */
                                                                        /* �ر������ж�                 */

    OSInit();                                                           /*  Initialize the kernel of uC */
                                                                        /*  OS-II ��ʼ��uC/OS-II���ں�  */

    OSTaskCreate ( taskStart,                                          
		           (void *)0, 
				   &GstkStart[TASK_START_STK_SIZE-1], 
				   TASK_START_PRIO );                                   /*  Initialize the start task   */
                                                                        /*  ��ʼ����������              */  
    OSStart(); 
 
							 /*  Start uC/OS-II ����uC/OS-II */
    return(0) ;
}

/*********************************************************************************************************
** Function name:           Task_Start	   
** Descriptions:            Start task	
** input parameters:        *p_arg
** output parameters:       ��      
** Returned value:          �� 
*********************************************************************************************************/
static  void  taskStart (void  *parg)
{   
    (void)parg;
    targetInit();                                                       /*  Initialize the target's MCU */
                                                                        /*  ��ʼ��Ŀ�굥Ƭ��            */
    #if OS_TASK_STAT_EN > 0
        OSStatInit();                                                   /*  Enable statistics           */
                                                                        /*  ʹ��ͳ�ƹ���                */
    #endif

    /*
     *  Create the other tasks here. �����ﴴ����������
     */
	OSTaskCreate (taskForm, (void *)0,   		
			      &GstkForm[TASK_FORMNET_STK_SIZE-1], 
				  TASK_FORMNET_PRIO);                                       /*  ��ʼ��taskLed����           */  	
			        
    while (1) {                             
        OSTaskSuspend(OS_PRIO_SELF);                                    /*  The start task can be pended*/
                                                                        /*  here. ������������������  */
    }
}

/*********************************************************************************************************
  The other tasks ��������
*********************************************************************************************************/
/*
 *  Add the other tasks here . ������������������
 */

/*********************************************************************************************************
** Function name:           taskForm	   
** Descriptions:            ��������	
** input parameters:        *parg
** output parameters:       ��      
** Returned value:          ��	 
*********************************************************************************************************/
static void taskForm (void  *parg)
{
    //�ж������ź��Ƿ�ɹ�
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
	//����λ��������������ɹ���־
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

