//*****************************************************************************
//
// LM3S1138 Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifndef _LM3S9b96_PINMAP_H_
#define _LM3S9b96_PINMAP_H_
/***********************************************************
SPI引脚的宏定义
***********************************************************/
#define SSICLK_PERIPH           (SYSCTL_PERIPH_GPIOA)
#define SSICLK_PORT             (GPIO_PORTA_BASE)
#define SSICLK_PIN              (GPIO_PIN_2)

#define SSIFSS_PERIPH           (SYSCTL_PERIPH_GPIOA)
#define SSIFSS_PORT             (GPIO_PORTA_BASE)
#define SSIFSS_PIN              (GPIO_PIN_3)

#define SSIRX_PERIPH            (SYSCTL_PERIPH_GPIOA)
#define SSIRX_PORT              (GPIO_PORTA_BASE)
#define SSIRX_PIN               (GPIO_PIN_4)

#define SSITX_PERIPH            (SYSCTL_PERIPH_GPIOA)
#define SSITX_PORT              (GPIO_PORTA_BASE)
#define SSITX_PIN               (GPIO_PIN_5)
/***********************************************************
通用异步收发UART接口的宏定义
***********************************************************/
#define U0RX_PERIPH             (SYSCTL_PERIPH_GPIOA)
#define U0RX_PORT               (GPIO_PORTA_BASE)
#define U0RX_PIN                (GPIO_PIN_0)

#define U0TX_PERIPH             (SYSCTL_PERIPH_GPIOA)
#define U0TX_PORT               (GPIO_PORTA_BASE)
#define U0TX_PIN                (GPIO_PIN_1)
/***********************************************************
板级LED灯的宏定义
***********************************************************/
#define LED1_PERIPH             (SYSCTL_PERIPH_GPIOF) //对应9b96板上的LED3(1)
#define LED1_PORT               (GPIO_PORTF_BASE)
#define LED1_PIN                (GPIO_PIN_2)

#define LED2_PERIPH             (SYSCTL_PERIPH_GPIOF)//对应9b96板上的LED4(2)
#define LED2_PORT               (GPIO_PORTF_BASE)
#define LED2_PIN                (GPIO_PIN_3)
/***********************************************************
板级开关的宏定义
***********************************************************/
#define SW1_PERIPH             (SYSCTL_PERIPH_GPIOJ)//对应9b96板上的KEY1
#define SW1_PORT               (GPIO_PORTJ_BASE)
#define SW1_PIN                (GPIO_PIN_7)

#define SW2_PERIPH             (SYSCTL_PERIPH_GPIOF)
#define SW2_PORT               (GPIO_PORTJ_BASE)
#define SW2_PIN                (GPIO_PIN_6)
/***********************************************************
防JTAG锁开关定义
***********************************************************/
#define  KEY_PERIPH             SYSCTL_PERIPH_GPIOJ
#define  KEY_PORT               GPIO_PORTJ_BASE
#define  KEY_PIN                GPIO_PIN_7
/***********************************************************
引脚触发接收中断引脚宏定义
***********************************************************/
#define   FIFOP_INT 			INT_GPIOJ
/***********************************************************
CC2420与开发板接口的引脚配置
***********************************************************/
#define VREGEN_PERIPH           (SYSCTL_PERIPH_GPIOJ) //定义VREGEN管脚，用于使能稳压器
#define VREGEN_PORT             (GPIO_PORTJ_BASE)
#define VREGEN_PIN              (GPIO_PIN_1)

#define RST_PERIPH              (SYSCTL_PERIPH_GPIOJ) //定义RST管脚，用于复位
#define RST_PORT                (GPIO_PORTJ_BASE)
#define RST_PIN                 (GPIO_PIN_6)

#define FIFO_PERIPH             (SYSCTL_PERIPH_GPIOJ) //定义FIFO管脚，可用于判断是否溢出或接收到数据（本次程序未使用）
#define FIFO_PORT               (GPIO_PORTJ_BASE)
#define FIFO_PIN                (GPIO_PIN_2)

#define FIFOP_PERIPH            (SYSCTL_PERIPH_GPIOJ) //定义FIFOP管脚，用来判断是否接收到数据
#define FIFOP_PORT              (GPIO_PORTJ_BASE)
#define FIFOP_PIN               (GPIO_PIN_3)

#define SFD_PERIPH              (SYSCTL_PERIPH_GPIOJ)  //定义SFD管脚，用于判断是否接收到SFD
#define SFD_PORT                (GPIO_PORTJ_BASE)
#define SFD_PIN                 (GPIO_PIN_5)

#define CCA_PERIPH              (SYSCTL_PERIPH_GPIOJ)  //定义CCA管脚，信道估计管脚
#define CCA_PORT                (GPIO_PORTJ_BASE)
#define CCA_PIN                 (GPIO_PIN_4)

#define CSN_PERIPH              (SYSCTL_PERIPH_GPIOJ)  //定义CSN管脚，用于片选
#define CSN_PORT                (GPIO_PORTJ_BASE)
#define CSN_PIN                 (GPIO_PIN_0)
//End////////////////////////////////////////////////////////
#endif // _LM3S9b96_PINMAP_H_

