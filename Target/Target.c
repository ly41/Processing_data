#include <includes.h>
#include <LM3S9b96_PinMap.h>
#include "driverlib/sysctl.h"

  
#define		MaxSize		11		//set from within usrRxPacketCallback
BYTE rxFlag;              
BYTE payload[5];
UINT16 numTimeouts;
BOOL first_packet;
UINT16 ping_cnt1;
UINT32 my_timer;
UINT32  last_tx_start;
LADDR_UNION dstADDR;
char tempBuf[30];
extern void  UART_Puts(const  char  *s);
tContext sContext;
tRectangle sRect;
volatile unsigned long ulLoop;
int lasttemp;




char buffer[16];
char buffer1[16];
char buffer2[16];
char buffer3[16];


char buffer4[16];
char buffer5[16];
char buffer6[16];
char buffer7[16];
char buffer8[16];
char buffer9[16];
char buffer10[16];
char buffer11[16];
char buffer12[16];
char buffer13[16];
char buffer14[16];
char buffer15[16];
char buffer16[16];
char buffer17[16];

BOOL a;
BOOL m;
BOOL n;
BOOL o;
BOOL n1;

int d=0;
int e;
int b=30;
int c;
int C;
int x;
int y;
int Z;
	


typedef  struct {

	long node;
	int type;
	int addr;
	float data;
	char card[];
}RFDDATA;
RFDDATA DATA;
/*typedef struct{
	RFDDATA data[MaxSize];
	int front,rear;
}SqQueue;*/

void AnalysisPacket(void);
void datajudge(void);
void judjealarm(void);
	

BOOL BOOLToggle(BOOL a)
{BOOL TEMP;
		if(a==true)
			TEMP=false;
		else TEMP=true;
		return TEMP;
}


void LCD_Init(void)
{
	/* tContext sContext;
    tRectangle sRect;*/
			a=false;
sprintf(buffer4,"%d",C);
    
   

   
    PinoutSet();

    
    Kitronix320x240x16_SSD2119Init();

    
    GrContextInit(&sContext, &g_sKitronix320x240x16_SSD2119);

    
    sRect.sXMin = 0;
    sRect.sYMin = 0;
    sRect.sXMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.sYMax = 43;
    GrContextForegroundSet(&sContext, ClrDarkBlue);
    GrRectFill(&sContext, &sRect);

    //
    // Put a white box around the banner.
    //
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectDraw(&sContext, &sRect);

    //
    // Put the application name in the middle of the banner.
    //
    GrContextFontSet(&sContext, &g_sFontCm30);
    GrStringDrawCentered(&sContext, "Parking", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 20, 0);

    //
    // Say hello using the Computer Modern 40 point font.
    //
    GrContextFontSet(&sContext, &g_sFontCm40);
    GrStringDrawCentered(&sContext, "	Welcome!", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
                         0);
										


GrContextFontSet(&sContext, &g_sFontCm30);
GrStringDrawCentered(&sContext, "USED:", -1,
                         GrContextDpyWidthGet(&sContext) / 4,( GrContextDpyHeightGet(&sContext)-24)/8*7+24, 0);
 GrContextFontSet(&sContext,&g_sFontCm30);
GrStringDrawCentered(&sContext, buffer4, -1,
                         GrContextDpyWidthGet(&sContext) / 2,( GrContextDpyHeightGet(&sContext)-24)/8*7+24, 0);

    //
    // Flush any cached drawing operations.
    //
    GrFlush(&sContext);
		
}

	void lcd()
	{
		sprintf(buffer4,"%d",C);
GrContextFontSet(&sContext, &g_sFontCm30);
GrStringDrawCentered(&sContext, "USED:", -1,
                         GrContextDpyWidthGet(&sContext) / 4,( GrContextDpyHeightGet(&sContext)-24)/8*7+24, 0);
 GrContextFontSet(&sContext,&g_sFontCm30);
GrStringDrawCentered(&sContext, buffer4, -1,
                         GrContextDpyWidthGet(&sContext) / 2,( GrContextDpyHeightGet(&sContext)-24)/8*7+24, 0);
}


void SendToUP210(int type, int value)
{
	char conbuf[10] = { 0 };
//	char valuebuf[2] = { 0 };
	
	conbuf[0] = '#';

	if (type == 1) // smoke
	{
		conbuf[1] = 'S';
		
		if(value == 0)	// normal
		{
			conbuf[2] = conbuf[3] = '0';
		}
		else						// warning
		{
			conbuf[2] = '0';
			conbuf[3] = '1';
		}
	}
	else if (type == 2)	// available
	{
		conbuf[1] = 'A';
		
		sprintf(conbuf+2, "%d", value); 
	}
	else if (type == 3)	// used
	{
		conbuf[1] = 'U';
		
		sprintf(conbuf+2, "%d", value);
	}
	else if (type == 4)	// card
	{
		conbuf[1] = 'C';
		
		sprintf(conbuf+2, "%d", value);
	}
	else if (type == 5)	// index
	{
		conbuf[1] = 'I';
		
		sprintf(conbuf+2, "%d", value);
	}
	/*else if (type == 6)	// index
	{
		conbuf[1] = 'J';
		
		sprintf(conbuf+2, "%d", value);
	}*/
	
	conbuf[4] = '\n';
	conbuf[5] = 0;
	
	conPrintString(conbuf);
	
}

void string(int data)
 { 
    c=b-1;
    sprintf(buffer,"%d",c);
	b=c;		// available
	 
	SendToUP210(2, b);
	 
	sprintf(buffer,"%d",b); 
 }
 void string1(int data)
 {
	//sprintf(buffer,"%d",b);  
	  c=b+1;
 sprintf(buffer,"%d",c);
	 b=c;
	sprintf(buffer,"%d",b); 
 }
 void string2(int data)
 { 
	 e=d+1;
	 sprintf(buffer,"%d",e);
	 d=e; 		// used
	 
    SendToUP210(3, d);
	 
	 sprintf(buffer,"%d",d);
 }
 void string3(int data)
 {
	  e=d-1;
	 sprintf(buffer,"%d",e);
	 d=e; 
	 sprintf(buffer,"%d",d);
  // sprintf(buffer,"%d",d);	 
 }
 
 /*void string4(int data)
 {
	// int A;
	 C=6;
 sprintf(buffer,"%d",C);
 }
  void string5(int data)
 {
	// int B;
	 C=8;
 sprintf(buffer,"%d",C);
 }
 
 void string6(int data)
 {
 y=00;
			 sprintf(buffer,"%d",y);
 }
 void string7(int data)
 {
 	y=11;
			 sprintf(buffer,"%d",	y);
 }
 void string8(int data)
 {
 
 }
 void string9(int data)
 {
 
 }
 */
 
 
 
 
void lockt()
{
if(a==true)
{
	string1(DATA.data);
		string3(DATA.data);
//	lcdtest2();
	a=false;}                                                                                                                                                             
else {
	string(DATA.data);
		string2(DATA.data);
	//lcdtest1();
   a=true;
}
}



/***********************************************************
目标板初始化
***********************************************************/
void  targetInit (void)
{

    SysCtlLDOSet(SYSCTL_LDO_2_75V);                     /*  设置LDO输出电压             */

    SysCtlClockSet(SYSCTL_USE_OSC |                     /*  系统时钟设置，采用主振荡器  */
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ |
                   SYSCTL_SYSDIV_1);
	
			LCD_Init();	
    tickInit();                                          /*  Initialize the uC/OS-II tick*/                                                          /*  interrupt,using the Kernal's*/
    numTimeouts = 0;
	my_timer = 0;
	first_packet = TRUE;                                 /*  timer                       */     
    // unsigned char test;
	//this initialization set our SADDR to 0xFFFF,
	//PANID to the default PANID
    JTAG_Wait();   
	//HalInit, evbInit will have to be called by the user
	halInit();
	evbInit();
	aplInit();  //init the stack
	conPrintConfig();
	ENABLE_GLOBAL_INTERRUPT();  //enable interrupts

	EVB_LED2_OFF();
    ping_cnt1 = 0;
	rxFlag = 0;      
    DEBUG_SET_LEVEL(10);     
}

/***********************************************************
防止JTAG失效
***********************************************************/
void  JTAG_Wait(void)
{
    SysCtlPeriEnable(KEY_PERIPH);                               //  使能KEY所在的GPIO端口
    GPIOPinTypeIn(KEY_PORT , KEY_PIN);                          //  设置KEY所在管脚为输入

    if ( GPIOPinRead(KEY_PORT , KEY_PIN)  ==  0x00 )            //  如果复位时按下KEY，则进入
    {
        for (;;);                                               //  死循环，以等待JTAG连接
    }
    SysCtlPeriDisable(KEY_PERIPH);                              //  禁止KEY所在的GPIO端口
}
/***********************************************************
端点接收回调函数
//callback for anytime the Zero Endpoint RX handles a command
//user can use the APS functions to access the arguments
//and take additional action is desired.
//the callback occurs after the ZEP has already taken
//its action.
***********************************************************/
LRWPAN_STATUS_ENUM  usrZepRxCallback(void){
#ifdef LRWPAN_COORDINATOR
	if (aplGetRxCluster() == ZEP_END_DEVICE_ANNOUNCE) {
		//a new end device has announced itself, print out the
		//the neightbor table and address map
		dbgPrintNeighborTable();
	}
#endif//LRWPAN_COORDINATOR
 	return LRWPAN_STATUS_SUCCESS;
}
/***********************************************************
用户接收包回调函数，处理接收数据
//callback from APS when packet is received
//user must do something with data as it is freed
//within the stack upon return.
***********************************************************/
LRWPAN_STATUS_ENUM  usrRxPacketCallback(void) {
	BYTE len, *ptr;
	unsigned char i;
//int shit;
	ptr = aplGetRxMsgData();
  len = aplGetRxMsgLen();
	EVB_LED2_ON();
	
	memset(tempBuf, 0, 30);
	memcpy(tempBuf, ptr, len);
	
//	conPrintString(tempBuf);

	conPrintChar('\r');

		
   /* for(i=25;i<len;i++)
    {
      	tempBuf[i]=*ptr;
        ptr++;
        conPrintChar(tempBuf[i]);
    }*/
		
	
	
		AnalysisPacket();
		datajudge();
		//shit=DATA.node/0x3e8;
		//if(shit==0x63)
	//	datajudge();
		
	EVB_LED2_OFF();
	rxFlag = 1;//signal that we got a packet
	//use this source address as the next destination address
	dstADDR.saddr = aplGetRxSrcSADDR();   
	return LRWPAN_STATUS_SUCCESS;
}

void datajudge(void)
{
//	n1=true;
	switch(DATA.type)
{
	case 12:	// smoke
	if(DATA.data>28)
	{
		y=00;	// warning
		sprintf(buffer5,"%d",y);
			 
		SendToUP210(1, 0);
		
		//EVB_LED2_ON();
	//	EVB_LED1_ON();		
	}
	else
	{
		y=11;	// normal
		sprintf(buffer5,"%d",	y);
		
		SendToUP210(1, 1);
	
	//	EVB_LED2_OFF();
	//	EVB_LED1_OFF();		
	}
			break;
	case 101:		// RFID
	/*	if(strncmp(DATA.card,"089C0532",8)==0)	// card 1
		{
		if(n==true)	// status
		{ 
			//int AA;
			x=20;	// ist in
			sprintf(buffer6,"%d",	x);
		
			SendToUP210(4, 10);
		
			n=false;
		}
		else 
	    {
			//int AB;
			x=21;	// 2nd out
			sprintf(buffer6,"%d",x);
			
			SendToUP210(4, 11);
			
			n=true;
		}
	}*/
	/*else if(strncmp(DATA.card,"08C60432",8)==0)	  // card 2
	{
		if(n==true)
		{
			//int BA;
			x=30;  // in
			sprintf(buffer7,"%d",	x);
			
			SendToUP210(4, 20);
			
			n=false;
		}
		else 
	    {
			//	int BB;
			x=31;	// out
			sprintf(buffer7,"%d",	x);
			
			SendToUP210(4, 21);
			
			n=true;
		}
	}*/
	/*else if(strncmp(DATA.card,"08430632",8)==0)	  	// card 3
	{
		if(n==true)
		{
			//int CA;
			x=40;  // in
			sprintf(buffer8,"%d",	x);
			
			SendToUP210(4, 30);
			
			n=false;
		}
		else 
	    {
			//int CB;
			x=41; // out
			sprintf(buffer8,"%d",	x);
			
			SendToUP210(4, 31);
			
			n=true;
		}
	}	*/
	 if(strncmp(DATA.card,"08DD0332",8)==0)		  // card 4
	{
	if(n==true)
    {
			//int DA;
			x=50;	//in
			sprintf(buffer9,"%d",	x);
			
			SendToUP210(4, 40);
			
			n=false;
    }
		else
	    {
			//	int DB;
			x=51;		// out
			sprintf(buffer9,"%d",	x);
			
			SendToUP210(4, 41);
			
			n=true;
		}
	}
	else if(strncmp(DATA.card,"08F00232",8)==0)		// card 5
	{
		if(n1==true)
		{
			//int EA;
			x=60;		// in
			sprintf(buffer10,"%d",	x);
			
			SendToUP210(4, 50);
			
			n1=false;
		}
		else
	    {
			//int EB;
			x=61;		// out
			sprintf(buffer10,"%d",	x);
			
			SendToUP210(4, 51);
			
			n1=true;
		}
	}
	
/*	else if(strncmp(DATA.card,"08740332",8)==0)		// card 0 useless
	{
	
			x=80;		// in
			sprintf(buffer11,"%d",	x);
			
			SendToUP210(4, 00);
		}*/

	case 61:		// hardcode 
		if(DATA.data >33)
				C=60;	// index=6 in
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 60);
	/*	{	
					if(n1==true)
					{
			C=60;	// index=6 in
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 60);
					n1=false;}
					if(n1==false)
					{
			C=61;	// index=6 out
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 61);
					n1=true;}
		}*/
	  	break;
	case 29:	
		if(DATA.data >28)
				C=80;	// index=8 in
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 80);
		/*{	
					if(n==true)
					{
			C=80;	// index=8 in
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 80);
					n=false;}
					if(n==false)
					{
			C=81;	// index=8 out
		sprintf(buffer4,"%d",C);
	
		SendToUP210(5, 81);
					n=true;}
		}*/
		lcd();
		break;
	
		case 59:	
		if(DATA.data >28)
				C=90;	// index=9 in
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 90);
		/*{	
					if(n1==true)
					{
			C=90;	// index=9 in
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 90);
					n1=false;}
					if(n1==false)
					{
			C=91;	// index=9 out
		sprintf(buffer4,"%d",C);
		
		SendToUP210(5, 91);
					n1=true;}
		}*/
		//lcd();
		break;
	/*	case 31:
					 if(DATA.data >58000)
					 { Z=01;
				 sprintf(buffer12,"%d",Z);
         SendToUP210(6, 01);
					 }
					 else if(DATA.data >48000)
					 { Z=11;
				 sprintf(buffer12,"%d",Z);	
					 SendToUP210(6, 11);
					 }
					 else if(DATA.data >38000)
					 { Z=21;
				 sprintf(buffer12,"%d",Z);
         SendToUP210(6, 21);
					 }
					 else if(DATA.data >28000)
					 { Z=31;
				 sprintf(buffer12,"%d",Z);
          SendToUP210(6, 31);
					 }
				 else if(DATA.data >18000)
					 { Z=41;
				 sprintf(buffer12,"%d",Z);
         SendToUP210(6, 41);
					 }
					 else 
					 { Z=51;
				 sprintf(buffer12,"%d",Z);	
					 SendToUP210(6, 51);
					 }
					 break;*/
	
	}
}


#ifdef LRWPAN_FFD
/***********************************************************
全功能设备，设备请求组网回调函数
//Callback to user level to see if OK for this node
//to join - implement Access Control Lists here based
//upon IEEE address if desired
***********************************************************/
BOOL usrJoinVerifyCallback(LADDR *ptr, BYTE capinfo){
//如果宏定义为ONLY_ROUTER，只测试通过路由器传数据，即树型网络测试，否则处理各类型联网请求设备
#if ONLY_ROUTER    
	//only accept routers.
	//only let routers join us if we are coord
	#ifdef LRWPAN_COORDINATOR
		if (LRWPAN_GET_CAPINFO_DEVTYPE(capinfo)) {
			//this is a router, let it join
//			conPrintROMString("Accepting router\n");
			return TRUE;
		}else {
//			conPrintROMString("Rejecting non-router\n");
			return FALSE;
		 }
	#else
		return TRUE;
	#endif //LRWPAN_COORDINATOR

#else
	return TRUE;
#endif //1
}
/***********************************************************
全功能设备，设备组网成功，消息调试输出回调函数
***********************************************************/
BOOL usrJoinNotifyCallback(LADDR *ptr){
	//allow anybody to join
//	conPrintROMString("Node joined: ");
//	conPrintLADDR(ptr);
	conPCRLF();
	DEBUG_PRINTNEIGHBORS(DBG_INFO);
	return TRUE;
}
#endif //LRWPAN_FFD

/***********************************************************
如果定义LRWPAN低定时器宏，定义低定时器中断处理函数
//called when the slow timer interrupt occurs
***********************************************************/
#ifdef LRWPAN_ENABLE_SLOW_TIMER
	void usrSlowTimerInt(void ) {}
#endif //LRWPAN_ENABLE_SLOW_TIMER

/***********************************************************
用户中断回调函数
//general interrupt callback , when this is called depends on the HAL layer.
***********************************************************/
void usrIntCallback(void){}

/*********************************************************************************************************
** Function name:       ledInit
** Descriptions:        Initialize the target board's leds,support up to 4 leds 
**                      初始化目标板的LED，最多支持4个
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    void  ledInit (void)
    {
        #if TARGET_LED1_EN > 0
            SysCtlPeripheralEnable(LED1_SYSCTL);
	    GPIODirModeSet(LED1_GPIO_PORT, LED1_PIN, GPIO_DIR_MODE_OUT);
            GPIOPadConfigSet(LED1_GPIO_PORT, LED1_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif

        #if TARGET_LED2_EN > 0
            SysCtlPeripheralEnable(LED2_SYSCTL);
	    GPIODirModeSet(LED2_GPIO_PORT, LED2_PIN, GPIO_DIR_MODE_OUT);
            GPIOPadConfigSet(LED2_GPIO_PORT, LED2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif

        #if TARGET_LED3_EN > 0
 	    SysCtlPeripheralEnable(LED3_SYSCTL);
	    GPIODirModeSet(LED3_GPIO_PORT, LED3_PIN, GPIO_DIR_MODE_OUT);
            GPIOPadConfigSet(LED3_GPIO_PORT, LED3_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif

        #if TARGET_LED4_EN > 0
 	    SysCtlPeripheralEnable(LED4_SYSCTL);
	    GPIODirModeSet(LED4_GPIO_PORT, LED4_PIN, GPIO_DIR_MODE_OUT);
            GPIOPadConfigSet(LED4_GPIO_PORT, LED4_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif
    }
#endif


/*********************************************************************************************************
** Function name:       ledOn
** Descriptions:        Switch on one or all of the LEDs 点亮其中一个或全部的LED
** Input parameters:    led: The num.of led to be switched on, 1-4 for LED1-LED4, 
**                      0xFF for all leds, others no action
**                      led: 要点亮的LED的号码，1-4代表LED1-LED4，0xFF代表全部LED，其他值无意义。
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    void  ledOn (INT8U  ucLed)
    {
        switch (ucLed) {
        case 1:                                                         /*  Switch on Led1  打开Led1    */
	    #if TARGET_LED1_EN > 0
                GPIOPinWrite(LED1_GPIO_PORT, LED1_PIN, ~LED1_PIN);
	    #endif
            break;
      
        case 2:                                                         /*  Switch on Led2  打开Led2    */
	    #if TARGET_LED2_EN > 0
      	        GPIOPinWrite(LED2_GPIO_PORT, LED2_PIN, ~LED2_PIN);
	    #endif
            break;
    
        case 3:                                                         /*  Switch on Led3  打开Led3    */
            #if TARGET_LED3_EN > 0
      	        GPIOPinWrite(LED3_GPIO_PORT, LED3_PIN, ~LED3_PIN);
            #endif
            break;
    
        case 4:                                                         /*  Switch on Led4  打开Led4    */
            #if TARGET_LED4_EN > 0
      	        GPIOPinWrite(LED4_GPIO_PORT, LED4_PIN, ~LED4_PIN);
	    #endif
            break;

        case 0xFF:                                                      /*  Switch on all  打开全部led  */
	    #if TARGET_LED1_EN > 0
      	        GPIOPinWrite(LED1_GPIO_PORT, LED1_PIN, ~LED1_PIN);
            #endif

	    #if TARGET_LED2_EN > 0
      	        GPIOPinWrite(LED2_GPIO_PORT, LED2_PIN, ~LED2_PIN);
	    #endif

            #if TARGET_LED3_EN > 0
      	        GPIOPinWrite(LED3_GPIO_PORT, LED3_PIN, ~LED3_PIN);
	    #endif

	    #if TARGET_LED4_EN > 0
      	        GPIOPinWrite(LED4_GPIO_PORT, LED4_PIN, ~LED4_PIN);
	    #endif
	    break;
	   
        default:                                                        /*  Default  默认               */
            break;
        }
    }
#endif


/*********************************************************************************************************
** Function name:       ledOff
** Descriptions:        Switch off one or all of the LEDs 关闭其中一个或全部的LED
** Input parameters:    led: The num.of led to be switched off, 1-4 for LED1-LED4, 
**                      0xFF for all leds, others no action
**                      led: 要关闭的LED的号码，1-4代表LED1-LED4，0xFF代表全部LED，其他值无意义。
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    void  ledOff (INT8U  ucLed)
    {
        switch (ucLed) {
        case 1:                                                         /*  Switch off Led1  关闭Led1   */
            #if TARGET_LED1_EN > 0
      	        GPIOPinWrite(LED1_GPIO_PORT, LED1_PIN, LED1_PIN);
            #endif
            break;
      
        case 2:                                                         /*  Switch off Led2  关闭Led2   */
	    #if TARGET_LED2_EN > 0
      	        GPIOPinWrite(LED2_GPIO_PORT, LED2_PIN, LED2_PIN);
	    #endif
            break;
    
        case 3:                                                         /*  Switch off Led3  关闭Led3   */
	    #if TARGET_LED3_EN > 0
                GPIOPinWrite(LED3_GPIO_PORT, LED3_PIN, LED3_PIN);
	    #endif
            break;

        case 4:                                                         /*  Switch off Led4  关闭Led4   */
	    #if TARGET_LED4_EN > 0
      	        GPIOPinWrite(LED4_GPIO_PORT, LED4_PIN, LED4_PIN);
	    #endif
            break;

        case 0xFF:                                                      /*  Switch off all  关闭全部led */
	    #if TARGET_LED1_EN > 0
      	        GPIOPinWrite(LED1_GPIO_PORT, LED1_PIN, LED1_PIN);
	    #endif

	    #if TARGET_LED2_EN > 0
      	        GPIOPinWrite(LED2_GPIO_PORT, LED2_PIN, LED2_PIN);
	    #endif

            #if TARGET_LED3_EN > 0
      	        GPIOPinWrite(LED3_GPIO_PORT, LED3_PIN, LED3_PIN);
	    #endif

	    #if TARGET_LED4_EN > 0
      	        GPIOPinWrite(LED4_GPIO_PORT, LED4_PIN, LED4_PIN);
	    #endif
	    break;
	   
        default:                                                        /*  Default  默认               */
            break;
        }
    }
#endif


/*********************************************************************************************************
** Function name:       ledToggle
** Descriptions:        Toggle one or all of the LEDs 取反其中一个或全部的LED
** Input parameters:    led: The num.of led to be toggled, 1-4 for LED1-LED4, 
**                      0xFF for all leds, others no action
**                      led: 要取反的LED的号码，1-4代表LED1-LED4，0xFF代表全部LED，其他值无意义。
** Output parameters:   None 无
** Returned value:      None 无         
*********************************************************************************************************/
#if (TARGET_LED1_EN > 0) || (TARGET_LED2_EN > 0) || (TARGET_LED3_EN > 0) || (TARGET_LED4_EN > 0)
    void  ledToggle (INT8U  ucLed)
    {
        switch (ucLed) {
        case 1:                                                         /*  Toggle Led1  取反Led1       */
	    #if TARGET_LED1_EN > 0
      	        GPIOPinWrite(LED1_GPIO_PORT, LED1_PIN, ~GPIOPinRead(LED1_GPIO_PORT, LED1_PIN));
	    #endif
            break;
      
        case 2:                                                         /*  Toggle Led2  取反Led2       */
            #if TARGET_LED2_EN > 0
    	        GPIOPinWrite(LED2_GPIO_PORT, LED2_PIN, ~GPIOPinRead(LED2_GPIO_PORT, LED2_PIN));
	    #endif
            break;
    
        case 3:                                                         /*  Toggle Led3  取反Led3       */
            #if TARGET_LED3_EN > 0
    	        GPIOPinWrite(LED3_GPIO_PORT, LED3_PIN, ~GPIOPinRead(LED3_GPIO_PORT, LED3_PIN));
	    #endif
            break;

        case 4:                                                         /*  Toggle Led4  取反Led4       */
            #if TARGET_LED4_EN > 0
    	        GPIOPinWrite(LED4_GPIO_PORT, LED4_PIN, ~GPIOPinRead(LED4_GPIO_PORT, LED4_PIN));
	    #endif
            break;

        case 0xFF:                                                      /*  Toggle all  取反全部led     */
	    #if TARGET_LED1_EN > 0
	        GPIOPinWrite(LED1_GPIO_PORT, LED1_PIN, ~GPIOPinRead(LED1_GPIO_PORT, LED1_PIN));
	    #endif

            #if TARGET_LED2_EN > 0
	        GPIOPinWrite(LED2_GPIO_PORT, LED2_PIN, ~GPIOPinRead(LED2_GPIO_PORT, LED2_PIN));
	    #endif

            #if TARGET_LED3_EN > 0
    	        GPIOPinWrite(LED3_GPIO_PORT, LED3_PIN, ~GPIOPinRead(LED3_GPIO_PORT, LED3_PIN));
	    #endif

	    #if TARGET_LED4_EN > 0
    	        GPIOPinWrite(LED4_GPIO_PORT, LED4_PIN, ~GPIOPinRead(LED4_GPIO_PORT, LED4_PIN));
	    #endif
            break;
	  	  
        default:                                                        /*  Default  默认               */
            break;
        }
    }
#endif


/*********************************************************************************************************
** Function name:       buzInit 
** Descriptions:	Initialize the target board's buzzer 初始化目标板的蜂鸣器
** Input parameters:	None 无
** Output parameters:	None 无
** Returned value:	None 无         
*********************************************************************************************************/
#if TARGET_BUZ_EN > 0
    void  buzInit (void)
    {
        SysCtlPeripheralEnable(BUZ_SYSCTL);
        GPIODirModeSet(BUZ_GPIO_PORT, BUZ_PIN, GPIO_DIR_MODE_OUT);
        GPIOPadConfigSet(BUZ_GPIO_PORT, BUZ_PIN,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        buzOff();
    }
#endif


/*********************************************************************************************************
** Function name:       buzOn
** Descriptions:        Switch on the buzzer 打开蜂鸣器
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
#if TARGET_BUZ_EN > 0
    void  buzOn (void)
    {
        GPIOPinWrite(BUZ_GPIO_PORT, BUZ_PIN, ~BUZ_PIN);
    }
#endif


/*********************************************************************************************************
** Function name:       buzOff
** Descriptions:	Switch off the buzzer 关闭蜂鸣器
** Input parameters:	None 无
** Output parameters:	None 无
** Returned value:	None 无
*********************************************************************************************************/
#if TARGET_BUZ_EN > 0
    void  buzOff (void)
    {
        GPIOPinWrite(BUZ_GPIO_PORT, BUZ_PIN, BUZ_PIN);
    }
#endif


/*********************************************************************************************************
** Function name:       buzToggle
** Descriptions:        Toggle the buzzer 取反蜂鸣器
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
#if TARGET_BUZ_EN > 0
    void buzToggle (void)    
    {
        GPIOPinWrite(BUZ_GPIO_PORT, BUZ_PIN, ~GPIOPinRead(BUZ_GPIO_PORT, BUZ_PIN));
    }
#endif


/*********************************************************************************************************
** Function name:       keyInit
** Descriptions:	Initialize the target board's keys,support up to 4 keys 
**                      初始化目标板的按键，最多支持4个
** Input parameters:    None 无
** Output parameters:	None 无
** Returned value:	None 无
*********************************************************************************************************/
#if (TARGET_KEY1_EN > 0) ||  (TARGET_KEY2_EN > 0) || (TARGET_KEY3_EN > 0) || (TARGET_KEY4_EN > 0)
    void keyInit (void)    
    {
        #if TARGET_KEY1_EN > 0
            SysCtlPeripheralEnable(KEY1_SYSCTL);
	    GPIODirModeSet(KEY1_GPIO_PORT, KEY1_PIN, GPIO_DIR_MODE_IN);
            GPIOPadConfigSet(KEY1_GPIO_PORT, KEY1_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif	
 
        #if TARGET_KEY2_EN > 0
 	    SysCtlPeripheralEnable(KEY2_SYSCTL);
	    GPIODirModeSet(KEY2_GPIO_PORT, KEY2_PIN, GPIO_DIR_MODE_IN);
            GPIOPadConfigSet(KEY2_GPIO_PORT, KEY2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif	    

        #if TARGET_KEY3_EN > 0
 	    SysCtlPeripheralEnable(KEY3_SYSCTL);
	    GPIODirModeSet(KEY3_GPIO_PORT, KEY3_PIN, GPIO_DIR_MODE_IN);
            GPIOPadConfigSet(KEY3_GPIO_PORT, KEY3_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif	  

        #if TARGET_KEY4_EN > 0
 	    SysCtlPeripheralEnable(KEY4_SYSCTL);
	    GPIODirModeSet(KEY4_GPIO_PORT, KEY4_PIN, GPIO_DIR_MODE_IN);
            GPIOPadConfigSet(KEY4_GPIO_PORT, KEY4_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        #endif	  
    }
#endif


/*********************************************************************************************************
** Function name:       keyRead
** Descriptions:	Read the status of the keys. 读取按键的状态
** Input parameters:	None 无
** Output parameters:	None 无
** Returned value:	8-bit unsigned char data. Bit0-bit3 stand for the status of Key1-Key4,
**                                                Bit4-Bit7 no meaning
**                      8位无符号数，位0－位3表示Key1-Key4的状态，位4－位7无意义         
*********************************************************************************************************/
#if (TARGET_KEY1_EN > 0) ||  (TARGET_KEY2_EN > 0) || (TARGET_KEY3_EN > 0) || (TARGET_KEY4_EN > 0)
    INT8U keyRead (void)
    {
        INT8U ucTemp;

        ucTemp = 0xFF;

        #if TARGET_KEY1_EN > 0
	    if (!GPIOPinRead(KEY1_GPIO_PORT, KEY1_PIN)) {
                ucTemp &= 0xFE;  
            }
        #endif

        #if TARGET_KEY2_EN > 0
	    if (!GPIOPinRead(KEY2_GPIO_PORT, KEY2_PIN)) { 
                ucTemp &= 0xFD; 
            }
        #endif

        #if TARGET_KEY3_EN > 0
            if (!GPIOPinRead(KEY3_GPIO_PORT, KEY3_PIN)) {
                ucTemp &= 0xFB; 
            }
        #endif

        #if TARGET_KEY4_EN > 0
            if (!GPIOPinRead(KEY4_GPIO_PORT, KEY4_PIN)) {
                ucTemp &= 0xF7; 
            }
        #endif     

        return(ucTemp);
    }
#endif


/*********************************************************************************************************
** Function name:       timer0AInit
** Descriptions:	Initialize Timer0A to 32bit timeout 初始化定时器0A为32位超时
** Input parameters:	Tick: Number of timeout tick 超时脉冲数
                        Prio: Interrupt priority 中断优先级
** Output parameters:	None 无
** Returned value:	None 无
*********************************************************************************************************/
#if TARGET_TMR0A_EN > 0
    void timer0AInit (INT32U  ulTick, INT8U  ucPrio)
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
  	TimerLoadSet(TIMER0_BASE, TIMER_A, ulTick);
  	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  	IntEnable(INT_TIMER0A);
  	IntPrioritySet(INT_TIMER0A, ucPrio);
	TimerEnable(TIMER0_BASE, TIMER_A);  	
            
    }
#endif


/*********************************************************************************************************
** Function name:       timer0AISR
** Descriptions:	Timeout interrupt handler of Timer0A 定时器0A超时中断
** Input parameters:	None 无
** Output parameters:	None 无
** Returned value:	None 无        
*********************************************************************************************************/
#if TARGET_TMR0A_EN > 0
    void timer0AISR (void)
    {
        /*
         *  Optional Code. If you don't call any uC/OS-II's functions & variables, 
         *  this code can be cancelled
         *  选择代码,如果你没有调用任何的uC/OS-II的函数和变量，可选择不编译这段代码.
         */ 
        #if 0
            #if OS_CRITICAL_METHOD == 3
                OS_CPU_SR cpu_sr;
            #endif 

            OS_ENTER_CRITICAL();                       
            OSIntNesting++;
            OS_EXIT_CRITICAL();
        #endif 
  
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);	                /*  Clear the interrupt flag.   */
                                                                        /*  清空中断标志                */
        /*
         *  Add you initialization code here.
         *  在这里加入你的初始化代码。
         */
        buzToggle();
        
        /*
         *  Optional Code. If you don't call any uC/OS-II's functions & variables,  
         *  this code can be cancelled.
         *  选择代码,如果你没有调用任何的uC/OS-II的函数和变量，可选择不编译这段代码.
         */ 
        
        #if 1 
            OSIntExit();
        #endif      					        
    }
#endif


/*********************************************************************************************************
** Function name:       tickInit
** Descriptions:	Initialize uC/OS-II's tick source(system timer)，
                        初始化uC/OS-II的时钟源（系统定时器）
** Input parameters:	None 无
** Output parameters:	None 无
** Returned value:	None 无        
*********************************************************************************************************/
static  void  tickInit (void)
{
    SysTickPeriodSet((INT32U)(SysCtlClockGet() / OS_TICKS_PER_SEC) -1 );
    SysTickEnable();
    SysTickIntEnable();
}


/*********************************************************************************************************
** Function name:       tickISRHandler
** Descriptions:	Timeout interrupt handler of system timer 系统定时器超时中断
** Input parameters:	None 无
** Output parameters:	None 无
** Returned value:	None 无        
*********************************************************************************************************/
void  tickISRHandler (void)
{
    #if OS_CRITICAL_METHOD == 3
        OS_CPU_SR cpu_sr;
    #endif 

    OS_ENTER_CRITICAL();                         
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                                       /*  Call uC/OS-II's OSTimeTick()*/

    OSIntExit();                                 
}

/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/





void AnalysisPacket()
{
   int Dataflag=0;
   char *ptrData=tempBuf;

	 char SADDR[10];
	 char SenType[10];
	 char SenData[10];
	 char EndNum[10];
   char *ptrSAddr=SADDR;
   char *ptrSenType=SenType;
   char *ptrSenData=SenData;
   char *ptrEndNum=EndNum;
      while(*ptrData!='\0')
   {
	   switch(*ptrData)
	   {
	   case '#':  Dataflag=1;  ptrData++;break;
	   case '&':  Dataflag=2;  ptrData++;break;
	   case '$':  Dataflag=3;  ptrData++;break;
	   case '*':  Dataflag=4;  ptrData++;break;
	   case '%':  Dataflag=5; break;
     }
	 switch(Dataflag)
	 {
	 case 1:   *ptrEndNum=*ptrData;
               ptrEndNum++;    break;
	 case 2:   *ptrSenType=*ptrData;
               ptrSenType++;   break;
	 case 3:   *ptrSAddr=*ptrData;
               ptrSAddr++;     break;
	 case 4:   *ptrSenData=*ptrData;
               ptrSenData++;   break;
	 }
     ptrData++;
   }
	 DATA.addr=atof(SADDR);
	 
	 DATA.node=atof(EndNum);
	 DATA.type=atof(SenType);
	 if(DATA.type==61)
	 DATA.data=atof(SenData);
	 if(DATA.type==29)
	 DATA.data=atof(SenData);
	  if(DATA.type==59)
	 DATA.data=atof(SenData);
	 if(DATA.type==31)
	 DATA.data=atof(SenData);
		// DATA.type=atof(SenType);
	// if(DATA.type==111)
		// DATA.type=atof(SenType);
	 	 if(DATA.type==12)
			 DATA.data=atof(SenData);
	  if(DATA.type==101)
	
	 {
	 strcpy(DATA.card,SenData);
	 
	 }
}

