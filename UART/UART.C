/*===========================================================================
公司 ：杭州方诚电力技术有限公司
作者 ：吴彦伟
功能：模拟单片机串口实现发送接收数据功能
日期：2015.7.5
===========================================================================*/
#include "STM8l15x_conf.h"
#include "MyTypeDef.h"
#include "bsp.h"
#include "stm8l15x_usart.h"

/**********************************
函数：UART_Config(void)
功能：对串口工作方式初始化设置
日期：2015.7.5
***********************************/
void UART_Config(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
    USART_DeInit(USART1);//复位寄存器
    USART_Init(USART1,9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx|USART_Mode_Rx);//初始化串口工作方式
    //USART_ITConfig(USART_IT_TXE,ENABLE);//串口发送中断
    //USART_ITConfig(USART_IT_TC, ENABLE);//关闭串口发送完中断
    USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);//打开串口接收中断
  //  USART_HalfDuplexCmd(USART1,ENABLE);
    USART_Cmd(USART1,ENABLE);//开启串口
}
/**********************************
函数：UART_SendByte(uint8_t data)
功能：串口发送一个字节的数据
日期：2015.7.5
***********************************/
void UART_SendByte(USART_TypeDef* USARTx,uint8_t data)
{
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
    USART_SendData8(USARTx,data);
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
}
/**************************************************
函数：UART_SendString(uint8_t *data, uint8_t len)
功能：串口发送字符串
日期：2015.7.5
****************************************************/
void UART_SendString(USART_TypeDef* USARTx,uint8_t *data, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
      UART_SendByte(USARTx,data[i]);
}
  int  putchar(int c)
 {
     USART_SendData8(USART1,c);
     while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
     return(c);
 }
void Uart_sendarray(u8 *p,u8 len)
{
	u8 *p1;
	u8 i = 0;
	p1 = p;
	for(i = 0;i < len;i++)
	{
		putchar(*p1);
		p1++;
	}
}