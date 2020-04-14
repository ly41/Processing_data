#include "evbRadio.h"

/*******************************************
���ϳ��ı�ʶ������Ϊ�϶̵���ʽ
********************************************/
#define  SysCtlPeriEnable           SysCtlPeripheralEnable
#define  SysCtlPeriDisable          SysCtlPeripheralDisable
#define  GPIOPinTypeIn              GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut             GPIOPinTypeGPIOOutput

//unsigned char wait(void);
/*******************************************
�ȴ��������
********************************************/
unsigned char wait(void)
{
  if((HWREG(SSI_BASE + SSI_O_SR) & 0x00000010) == 0x00000010)
    return 1;
  else
    return 0;
}

/*******************************************
д����Ĵ���
********************************************/
void Write_Command(unsigned char command)
{
  SPI_ENABLE();                     //ʹ��SSI�շ�
  SSIDataPut(SSI_BASE , command);   //  ͨ��SSI�������
  while(wait());
  SPI_DISABLE();              //��ֹSPI��
}

/*******************************************
��״̬�Ĵ���
********************************************/
unsigned char Read_Status(void)
{
  unsigned char statusvalue;
  unsigned long pulData;
  Write_Command(CC2420_SNOP);    //�ղ������Ϊ�˶�ȡ��ǰ״̬�Ĵ�����ֵ
  SPI_ENABLE();             //ʹ��SSI�շ�
  SSIDataGet(SSI_BASE, &pulData);
  while(wait());
  SPI_DISABLE();                 //��ֹSPI��
  statusvalue = (unsigned char)(pulData);
  return statusvalue;
}

/*******************************************
д������
********************************************/
void Write_ConfigureWord(unsigned char Addr,unsigned char DataH,unsigned char DataL)
{
  SSIDataPut(SSI_BASE , Addr);                  //  ͨ��SSI�������
  while(wait());
  SSIDataPut(SSI_BASE , DataH);                 //���������ֵĸ��ֽ�
  while(wait());
  SSIDataPut(SSI_BASE , DataL);                 //���������ֵĵ��ֽ�
  while(wait());
}
/******************************************
��RXFIFO�ж�һ��BYTE
*******************************************/
unsigned char Read_Byte_RXFIFO(void)
{
    unsigned long pulData;
    SSIDataPut(SSI_BASE , 0xFF);
    while(wait());
    SSIDataGet(SSI_BASE, &pulData);
    while(wait());
    return  pulData;
}

/*******************************************
��RXFIFO
********************************************/
void Read_RXFIFO(unsigned char* Data_Num,unsigned char *p_Data)
{
  unsigned char Addr,i;
  unsigned long pulData;
  Addr=CC2420_RXFIFO|0x40;     //����־λΪ1

  SPI_ENABLE();
  SSIDataPut(SSI_BASE , Addr);
 // while(wait());
  //SSIDataGet(SSI_BASE, &status);
  for(i=0;i<8;i++)
  {
    SSIDataPut(SSI_BASE , 0xFF);
   // while(wait());
    SSIDataGet(SSI_BASE, &pulData);
  }
    SSIDataPut(SSI_BASE , 0xFF);
   // while(wait());
    SSIDataGet(SSI_BASE, &pulData);
    *Data_Num = (unsigned char)pulData;
  for(i=0;i<*Data_Num;i++)
  {
    SSIDataPut(SSI_BASE , 0xFF);
   // while(wait());
    SSIDataGet(SSI_BASE, &pulData);
   // while(wait());
    *(p_Data+i)=(unsigned char)(pulData);
  }
  while(wait());
  SPI_DISABLE();
}

/*******************************************
дTXFIFO
********************************************/
void Write_TXFIFO(unsigned char Data_Num,unsigned char *p_Data)
{
  unsigned char i/*statuevalue*/;

  SPI_ENABLE();
  SSIDataPut(SSI_BASE , CC2420_TXFIFO);
  while(wait());

  for (i=0; i<Data_Num; i++)
  {
    SSIDataPut(SSI_BASE , *(p_Data++));
    while(wait());
	/*if ((statuevalue&(1<<CC2420_TX_UNDERFLOW))!=0x00)         //��������
	{
	  Write_Command(CC2420_SFLUSHTX);
          SPI_ENABLE();
	}*/
  }
  while(wait());
  SPI_DISABLE();
}
/*******************************************
дRAM
********************************************/
void Write_RAM(unsigned char AddrH,unsigned char AddrL,unsigned char Data_Num,unsigned char *p_Data)
{
  unsigned char i;
  AddrH|=0X80;      //����RAMλ��1
  AddrL<<=6;
  AddrL&=0XDF;      //��д����λ��0����ʾд����

  SPI_ENABLE();
  SSIDataPut(SSI_BASE , AddrH);
  while(wait());
  SSIDataPut(SSI_BASE , AddrL);
  while(wait());

  for (i=0;i<Data_Num;i++)
  {
    SSIDataPut(SSI_BASE , *(p_Data+i));
    while(wait());
  }
  while(wait());
  SPI_DISABLE();
}
