#ifndef _BSP_H_
#define _BSP_H_

#include "STM8l15x_conf.h"
#include "SX1278.H"
#include "DS18B20.H"
#include "mytypedef.h"

//WDI�ܽ�
#define PORT_WDI        GPIOA
#define PIN_WDI         GPIO_Pin_2

//RF�ܽ�
#define PORT_RF         GPIOC
#define PIN_RF          GPIO_Pin_4

//���߿���
#define  RF_ON()        GPIO_ResetBits(PORT_RF, PIN_RF)
#define  RF_OFF()       GPIO_SetBits(PORT_RF, PIN_RF)


// SPI���Ŷ��� SCLK(PB5), MOSI(PB6), MISO(PB7)
#define PORT_SPI        GPIOB
#define PIN_SCLK        GPIO_Pin_5
#define PIN_MOSI        GPIO_Pin_6
#define PIN_MISO        GPIO_Pin_7

#define PORT_CC_CSN     GPIOB
#define PIN_CC_CSN      GPIO_Pin_4
#define RF_CSN_HIGH()   GPIO_SetBits(PORT_CC_CSN,PIN_CC_CSN)
#define RF_CSN_LOW()	GPIO_ResetBits(PORT_CC_CSN, PIN_CC_CSN)
#define Reset_Port      GPIOB       //SX1278��λ
#define Reset_Pin       GPIO_Pin_3
#define Reset_H()       GPIO_SetBits(Reset_Port,Reset_Pin)
#define Reset_L()       GPIO_ResetBits(Reset_Port,Reset_Pin)


//���ڽ�������
#define PORT_TRXD       GPIOC
#define UART_PIN_RXD    GPIO_Pin_6
#define UART_PIN_TXD    GPIO_Pin_5

void SClK_Initial(void);                // ��ʼ��ϵͳʱ�ӣ�ϵͳʱ�� = 16MHZ
void GPIO_Initial(void);                // ��ʼ��ͨ��IO�˿�
void GPIO_Initial_StartUp(void);		
void SPI_Initial(void);                 // ��ʼ��SPI
void TIM2_Initial(void);                // ��ʼ����ʱ��2����ʱʱ��Ϊ1ms
void TIM3_Initial(void);                // ��ʼ����ʱ��3����ʱʱ��Ϊ1ms
void RTC_Config(uint16_t time);
void IWDG_Init();
void WDI(void);
void WaitSleep(unsigned short Time,unsigned short RTC_count);
void WaitSleep_StartUp(unsigned short Time,unsigned short RTC_count);
void Delay_100us(unsigned short time);
void Delay_10Ms(unsigned short time);
void Delay_1Ms(unsigned short time);
void Close_Pin();
INT8U SPI_ExchangeByte(SPI_TypeDef* SPIx,INT8U input);    // ͨ��SPI�������ݽ���
void Feed_Watchdog(void);
#endif //_BSP_H_
