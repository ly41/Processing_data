;*********************************************************************************************************
;*  Byte number of Main Stack  ����ջ���ֽ���            
;*********************************************************************************************************
Stack   EQU     256


;*********************************************************************************************************
;*  Byte number of the Heap  �ѵ��ֽ���                 
;*********************************************************************************************************
Heap    EQU     100


;*********************************************************************************************************
;*  Allocate space for Main Stack  ����ջ����Ŀռ�           
;*********************************************************************************************************
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack


;*********************************************************************************************************
;*  Allocate space for the heap    ջ����Ŀռ�                                      
;*********************************************************************************************************
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
HeapMem
        SPACE   Heap


;*********************************************************************************************************
;*  Declarations for the interrupt handlers that are used by the application.                            
;*  ��Ӧ�����õ����жϺ���������                                          
;*********************************************************************************************************
        EXTERN	OSPendSV  
		EXTERN  tickISRHandler
		EXTERN  Timer0B_ISR
 		EXTERN  Timer2B_ISR
 		EXTERN  UART0_ISR
 		EXTERN  GPIO_Port_J_ISR 	   
	
;*********************************************************************************************************
;*  Indicate that the code in this file preserves 8-byte alignment of the stack.              
;*  ���ļ��Ĵ���Զ�ջ8�ֽڶ��뱣��                                          
;*********************************************************************************************************
        PRESERVE8


;*********************************************************************************************************
;*  Reset code section.                                           
;*  ��λ����                                                                 
;*********************************************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;*********************************************************************************************************
;*  The vector table.
;*  ������
;*********************************************************************************************************
        EXPORT __Vectors
__Vectors
        DCD     StackMem + Stack                                        ; Top of Stack
        DCD     Reset_Handler                                           ; Reset Handler
        DCD     NmiSR                                                   ; NMI Handler
        DCD     IntDefaultHandler                                       ; Hard Fault Handler
        DCD     IntDefaultHandler                                       ; MPU Fault Handler
        DCD     IntDefaultHandler                                       ; Bus Fault Handler
        DCD     IntDefaultHandler                                       ; Usage Fault Handler
        DCD     0                                                       ; Reserved
        DCD     0                                                       ; Reserved
        DCD     0                                                       ; Reserved
        DCD     0                                                       ; Reserved
        DCD     IntDefaultHandler                                       ; SVCall Handler
        DCD     IntDefaultHandler                                       ; Debug Monitor Handler
        DCD     0                                                       ; Reserved
	    DCD     OSPendSV                                                ; PendSV Handler
	    DCD     tickISRHandler                                          ; SysTick Handler
        DCD     IntDefaultHandler                                       ; GPIO Port A
        DCD     IntDefaultHandler                                       ; GPIO Port B
        DCD     IntDefaultHandler                                       ; GPIO Port C
        DCD     IntDefaultHandler                                       ; GPIO Port D
        DCD     IntDefaultHandler                                       ; GPIO Port E
        DCD     UART0_ISR                                       ; UART0
        DCD     IntDefaultHandler                                       ; UART1
        DCD     IntDefaultHandler                                       ; SSI
        DCD     IntDefaultHandler                                       ; I2C
        DCD     IntDefaultHandler                                       ; PWM Fault
        DCD     IntDefaultHandler                                       ; PWM Generator 0
        DCD     IntDefaultHandler                                       ; PWM Generator 1
        DCD     IntDefaultHandler                                       ; PWM Generator 2
        DCD     IntDefaultHandler                                       ; Quadrature Encoder
        DCD     IntDefaultHandler                                       ; ADC Sequence 0
        DCD     IntDefaultHandler                                       ; ADC Sequence 1
        DCD     IntDefaultHandler                                       ; ADC Sequence 2
        DCD     IntDefaultHandler                                       ; ADC Sequence 3
        DCD     IntDefaultHandler                                       ; Watchdog
        DCD     IntDefaultHandler                                       ; Timer 0A
        DCD     Timer0B_ISR                                       ; Timer 0B
        DCD     IntDefaultHandler                                       ; Timer 1A
        DCD     IntDefaultHandler                                       ; Timer 1B
        DCD     IntDefaultHandler                                       ; Timer 2A
        DCD     Timer2B_ISR                                       ; Timer 2B
        DCD     IntDefaultHandler                                       ; Comp 0
        DCD     IntDefaultHandler                                       ; Comp 1
        DCD     IntDefaultHandler                                       ; Comp 2
        DCD     IntDefaultHandler                                       ; System Control
        DCD     IntDefaultHandler                                       ; Flash Control
        DCD     IntDefaultHandler                                       ; GPIO Port F
        DCD     IntDefaultHandler                                       ; GPIO Port G
        DCD     IntDefaultHandler                                       ; GPIO Port H
        DCD     IntDefaultHandler                                       ; UART2 Rx and Tx
        DCD     IntDefaultHandler                                       ; SSI1 Rx and Tx
        DCD     IntDefaultHandler                                       ; Timer 3 subtimer A
        DCD     IntDefaultHandler                                       ; Timer 3 subtimer B
        DCD     IntDefaultHandler                                       ; I2C1 Master and Slave
        DCD     IntDefaultHandler                                       ; Quadrature Encoder 1
        DCD     IntDefaultHandler                                       ; CAN0
        DCD     IntDefaultHandler                                       ; CAN1
        DCD     IntDefaultHandler                                       ; CAN2
        DCD     IntDefaultHandler                                       ; Ethernet
        DCD     IntDefaultHandler                                       ; Hibernate
		DCD     IntDefaultHandler                                       ; USB0
        DCD     IntDefaultHandler                                       ; PWM Generator 3
        DCD     IntDefaultHandler                                       ; uDMA Software Transfer
        DCD     IntDefaultHandler                                       ; uDMA Error
        DCD     IntDefaultHandler                                       ; ADC1 Sequence 0
        DCD     IntDefaultHandler                                       ; ADC1 Sequence 1
        DCD     IntDefaultHandler                                       ; ADC1 Sequence 2
        DCD     IntDefaultHandler                                       ; ADC1 Sequence 3
        DCD     IntDefaultHandler                                       ; I2S0
        DCD     IntDefaultHandler                                       ; External Bus Interface 0
        DCD     GPIO_Port_J_ISR                                         ; GPIO Port J


;*********************************************************************************************************
;*  Reset entry
;*  ��λ��ڵ�
;*********************************************************************************************************
        EXPORT  Reset_Handler
Reset_Handler
        IMPORT  __main
        LDR     R0, =__main
        BX      R0


;*********************************************************************************************************
;*  NMI exception handler. 
;*  It simply enters an infinite loop.
;*  ���������쳣������򡣼򵥵ؽ�����ѭ��
;*********************************************************************************************************
NmiSR
        B       NmiSR


;*********************************************************************************************************
;*  Fault interrupt handler. 
;*  It simply enters an infinite loop.
;*  �����жϴ�����򡣼򵥵ؽ�����ѭ��
;*********************************************************************************************************
FaultISR
        B       FaultISR


;*********************************************************************************************************
;*  Unexpected interrupt handler. 
;*  It simply enters an infinite loop.
;*  ���ڴ����жϴ�����򡣼򵥵ؽ�����ѭ��
;*********************************************************************************************************
IntDefaultHandler
        B       IntDefaultHandler


;*********************************************************************************************************
;*  Make sure the end of this section is aligned.
;*  ȷ�����ε�ĩβ����
;*********************************************************************************************************
        ALIGN



;*********************************************************************************************************
;*  Code section for initializing the heap and stack                                                      
;*  �Ѻ�ջ�ĳ�ʼ������                                                     
;*********************************************************************************************************
        AREA    |.text|, CODE, READONLY

;*********************************************************************************************************
;*  The function expected of the C library startup 
;*  code for defining the stack and heap memory locations.
;*  C������������ñ������ʼ���Ѻ�ջ 
;*********************************************************************************************************
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap
__user_initial_stackheap
        LDR     R0, =HeapMem
        LDR     R1, =(StackMem + Stack)
        LDR     R2, =(HeapMem + Heap)
        LDR     R3, =StackMem
        BX      LR


;*********************************************************************************************************
;*  Make sure the end of this section is aligned.
;*  ȷ�����ε�ĩβ����
;*********************************************************************************************************
        ALIGN


;*********************************************************************************************************
;*  End Of File                                                     
;*********************************************************************************************************
        END
