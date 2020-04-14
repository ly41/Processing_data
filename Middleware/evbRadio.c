#include "evbRadio.h"

/*******************************************
将较长的标识符定义为较短的形式
********************************************/
#define  SysCtlPeriEnable           SysCtlPeripheralEnable
#define  SysCtlPeriDisable          SysCtlPeripheralDisable
#define  GPIOPinTypeIn              GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut             GPIOPinTypeGPIOOutput

//unsigned char wait(void);
/*******************************************
等待传输完毕
********************************************/
unsigned char wait(void)
{
  if((HWREG(SSI_BASE + SSI_O_SR) & 0x00000010) == 0x00000010)
    return 1;
  else
    return 0;
}

/*******************************************
写命令寄存器
********************************************/
void Write_Command(unsigned char command)
{
  SPI_ENABLE();                     //使能SSI收发
  SSIDataPut(SSI_BASE , command);   //  通过SSI输出数据
  while(wait());
  SPI_DISABLE();              //禁止SPI口
}

/*******************************************
读状态寄存器
********************************************/
unsigned char Read_Status(void)
{
  unsigned char statusvalue;
  unsigned long pulData;
  Write_Command(CC2420_SNOP);    //空操作命令，为了读取当前状态寄存器的值
  SPI_ENABLE();             //使能SSI收发
  SSIDataGet(SSI_BASE, &pulData);
  while(wait());
  SPI_DISABLE();                 //禁止SPI口
  statusvalue = (unsigned char)(pulData);
  return statusvalue;
}

/*******************************************
写配置字
********************************************/
void Write_ConfigureWord(unsigned char Addr,unsigned char DataH,unsigned char DataL)
{
  SSIDataPut(SSI_BASE , Addr);                  //  通过SSI输出数据
  while(wait());
  SSIDataPut(SSI_BASE , DataH);                 //发送配置字的高字节
  while(wait());
  SSIDataPut(SSI_BASE , DataL);                 //发送配置字的低字节
  while(wait());
}
/******************************************
从RXFIFO中读一个BYTE
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
读RXFIFO
********************************************/
void Read_RXFIFO(unsigned char* Data_Num,unsigned char *p_Data)
{
  unsigned char Addr,i;
  unsigned long pulData;
  Addr=CC2420_RXFIFO|0x40;     //读标志位为1

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
写TXFIFO
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
	/*if ((statuevalue&(1<<CC2420_TX_UNDERFLOW))!=0x00)         //发生下溢
	{
	  Write_Command(CC2420_SFLUSHTX);
          SPI_ENABLE();
	}*/
  }
  while(wait());
  SPI_DISABLE();
}
/*******************************************
写RAM
********************************************/
void Write_RAM(unsigned char AddrH,unsigned char AddrL,unsigned char Data_Num,unsigned char *p_Data)
{
  unsigned char i;
  AddrH|=0X80;      //访问RAM位置1
  AddrL<<=6;
  AddrL&=0XDF;      //读写操作位置0，表示写操作

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
