/*
  V0.1 Initial Release   10/July/2006  RBR

*/
#ifndef EVBRADIO_H
#define EVBRADIO_H

#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <hw_sysctl.h>
#include  <hw_gpio.h>
#include  <hw_ssi.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <ssi.h>
#include  "LM3S9b96_PinMap.H"
#include  <hw_ints.h>
#include  <interrupt.h>
#include  <hw_uart.h>
#include  <uart.h>
#define  BM(x)  (1<<x)

/***********************************************************
����ģ��CC2420����Ĵ����궨��
***********************************************************/
#define CC2420_SNOP             0x00
#define CC2420_SXOSCON          0x01
#define CC2420_STXCAL           0x02
#define CC2420_SRXON            0x03
#define CC2420_STXON            0x04
#define CC2420_STXONCCA         0x05
#define CC2420_SRFOFF           0x06
#define CC2420_SXOSCOFF         0x07
#define CC2420_SFLUSHRX         0x08
#define CC2420_SFLUSHTX         0x09
#define CC2420_SACK             0x0A
#define CC2420_SACKPEND         0x0B
#define CC2420_SRXDEC           0x0C
#define CC2420_STXENC           0x0D
#define CC2420_SAES             0x0E
/***********************************************************
����ģ��CC2420���üĴ����궨��
***********************************************************/
#define CC2420_MAIN             0x10
#define CC2420_MDMCTRL0         0x11
#define CC2420_MDMCTRL1         0x12
#define CC2420_RSSI             0x13
#define CC2420_SYNCWORD         0x14
#define CC2420_TXCTRL           0x15
#define CC2420_RXCTRL0          0x16
#define CC2420_RXCTRL1          0x17
#define CC2420_FSCTRL           0x18
#define CC2420_SECCTRL0         0x19
#define CC2420_SECCTRL1         0x1A
#define CC2420_BATTMON          0x1B
#define CC2420_IOCFG0           0x1C
#define CC2420_IOCFG1           0x1D
#define CC2420_MANFIDL          0x1E
#define CC2420_MANFIDH          0x1F
#define CC2420_FSMTC            0x20
#define CC2420_MANAND           0x21
#define CC2420_MANOR            0x22
#define CC2420_AGCCTRL          0x23
#define CC2420_AGCTST0          0x24
#define CC2420_AGCTST1          0x25
#define CC2420_AGCTST2          0x26
#define CC2420_FSTST0           0x27
#define CC2420_FSTST1           0x28
#define CC2420_FSTST2           0x29
#define CC2420_FSTST3           0x2A
#define CC2420_RXBPFTST         0x2B
#define CC2420_FSMSTATE         0x2C
#define CC2420_ADCTST           0x2D
#define CC2420_DACTST           0x2E
#define CC2420_TOPTST           0x2F
#define CC2420_RESERVED         0x30
#define CC2420_TXFIFO           0x3E
#define CC2420_RXFIFO           0x3F
/***********************************************************
����ģ��CC2420RAM�ͷ��ͽ��ն��г��Ⱥ궨��
***********************************************************/
#define CC2420_RAM_SIZE			368
#define CC2420_FIFO_SIZE		128

/***********************************************************
����ģ��CC2420RAM���õ�ַ
***********************************************************/
#define CC2420RAM_TXFIFO		0x000
#define CC2420RAM_RXFIFO		0x080
#define CC2420RAM_KEY0			0x100
#define CC2420RAM_RXNONCE		0x110
#define CC2420RAM_SABUF			0x120
#define CC2420RAM_KEY1			0x130
#define CC2420RAM_TXNONCE		0x140
#define CC2420RAM_CBCSTATE		0x150
#define CC2420RAM_IEEEADDR		0x160
#define CC2420RAM_PANID			0x168
#define CC2420RAM_SHORTADDR		0x16A

/***********************************************************
����ģ��CC2420����Ĵ���״̬�궨��
***********************************************************/
#define CC2420_XOSC16M_STABLE		6
#define CC2420_TX_UNDERFLOW			5
#define CC2420_ENC_BUSY				4
#define CC2420_TX_ACTIVE		    3
#define CC2420_LOCK				    2
#define CC2420_RSSI_VALID		    1

/***********************************************************
����ģ��CC2420��ȫ���ƼĴ���SECCTRL0�궨��
***********************************************************/
#define CC2420_SECCTRL0_NO_SECURITY         0x0000
#define CC2420_SECCTRL0_CBC_MAC             0x0001
#define CC2420_SECCTRL0_CTR                 0x0002
#define CC2420_SECCTRL0_CCM                 0x0003
#define CC2420_SECCTRL0_SEC_M_IDX           2
#define CC2420_SECCTRL0_RXKEYSEL0           0x0000
#define CC2420_SECCTRL0_RXKEYSEL1           0x0020
#define CC2420_SECCTRL0_TXKEYSEL0           0x0000
#define CC2420_SECCTRL0_TXKEYSEL1           0x0040
#define CC2420_SECCTRL0_SEC_CBC_HEAD        0x0100
#define CC2420_SECCTRL0_RXFIFO_PROTECTION   0x0200

/***********************************************************
����ģ������ź�ǿ��ָʾRSSI�йغ궨��
***********************************************************/
// RSSI to Energy Detection conversion
// RSSI_OFFSET defines the RSSI level where the PLME.ED generates a zero-value
#define RSSI_OFFSET -38
#define RSSI_2_ED(rssi)   	((rssi) < RSSI_OFFSET ? 0 : ((rssi) - (RSSI_OFFSET)))
#define ED_2_LQI(ed) 		(((ed) > 63 ? 255 : ((ed) << 2)))
/***********************************************************
����ģ���������ú궨��
***********************************************************/
#define  PIN_FIFO_CONFIG            GPIOPinTypeGPIOInput(FIFO_PORT,FIFO_PIN)
#define  PIN_CCA_CONFIG             GPIOPinTypeGPIOInput(CCA_PORT,CCA_PIN)
#define  PIN_SFD_CONFIG             GPIOPinTypeGPIOInput(SFD_PORT,SFD_PIN)
#define  PIN_FIFOP_CONFIG           GPIOPinTypeGPIOInput(FIFOP_PORT,FIFOP_PIN)
#define  PIN_CSn_CONFIG             GPIOPinTypeGPIOOutput(CSN_PORT,CSN_PIN)
#define  PIN_VREG_EN_CONFIG        	GPIOPinTypeGPIOOutput(VREGEN_PORT,VREGEN_PIN)
#define  PIN_RESETn_CONFIG          GPIOPinTypeGPIOOutput(RST_PORT,RST_PIN)

/***********************************************************
����ģ�������жϺ궨��
***********************************************************/
#define FIFO_IS_1         			GPIOPinRead(FIFO_PORT , FIFO_PIN)
#define CCA_IS_1          			GPIOPinRead(CCA_PORT , CCA_PIN )
#define RESET_IS_1      			GPIOPinRead(RST_PORT , RST_PIN )
#define VREG_IS_1        			GPIOPinRead(VREGEN_PORT , VREGEN_PIN)
#define FIFOP_IS_1       			GPIOPinRead(FIFOP_PORT , FIFOP_PIN )
#define SFD_IS_1           			GPIOPinRead(SFD_PORT , SFD_PIN)

/***********************************************************
����ģ������ʹ�ܺ궨��
***********************************************************/
#define	SPI_ENABLE()                GPIOPinWrite(CSN_PORT , CSN_PIN , 0x00)
#define SPI_DISABLE()               GPIOPinWrite(CSN_PORT , CSN_PIN ,  CSN_PIN)  
// The CC2420 reset pin
#define SET_RESET_ACTIVE()     		GPIOPinWrite(RST_PORT , RST_PIN , 0x00)
#define SET_RESET_INACTIVE()   		GPIOPinWrite(RST_PORT , RST_PIN , RST_PIN)
// CC2420 voltage regulator enable pin
#define SET_VREG_ACTIVE()     		GPIOPinWrite(VREGEN_PORT , VREGEN_PIN , VREGEN_PIN)
#define SET_VREG_INACTIVE()   		GPIOPinWrite(VREGEN_PORT , VREGEN_PIN, 0x00)
//enable FIFOP interrupt
#define ENABLE_FIFOP_INT()    		IntEnable(FIFOP_INT)
#define DISABLE_FIFOP_INT()   		IntDisable(FIFOP_INT)
#define FASTSPI_RX_ADDR(a)    		SSIDataPut(SSI_BASE , a);	//����RXAddr����λ�����ݽ��ռĴ���

/***********************************************************
�ж����ų�ʼ�����жϷ���������
***********************************************************/
void  GPIOJ_IntInit(void);
void  GPIO_Port_J_ISR(void);
/***********************************************************
SPI���ݽ��յȴ���ɺ�������
***********************************************************/
unsigned char wait(void);         //�ȴ�SPI�������

/***********************************************************
�Ĵ�����������
***********************************************************/
void Write_Command(unsigned char command);   //д����Ĵ���
void Write_ConfigureWord(unsigned char Addr,unsigned char DataH,unsigned char DataL);//д������
unsigned char Read_Status(void);    //��״̬�Ĵ���

/***********************************************************
�����շ�FIFO�Ķ�ȡ����
***********************************************************/
void Write_TXFIFO(unsigned char Data_Num,unsigned char *p_Data);   //дTXFIFO
void Read_RXFIFO(unsigned char* Data_Num,unsigned char *p_Data);    //��RXFIFO
unsigned char Read_Byte_RXFIFO(void);   //read a byte from RXFIFO
/***********************************************************
����ģ��CC2420 RAM ����(С�˷���ģʽ)
***********************************************************/
void Write_RAM(unsigned char AddrH,unsigned char AddrL,unsigned char Data_Num,unsigned char *p_Data);
#endif



