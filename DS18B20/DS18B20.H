#ifndef _DS18B20_H_
#define _DS18B20_H_
#include "STM8l15x_conf.h"
#include "mytypedef.h"
#include "stdarg.h"
#include "stm8l15x.h"

//DS18B20数据管脚
#define PORT_DQ         GPIOB
#define PIN_DQ          GPIO_Pin_0

//DS18B20电源管脚
#define PORT_CTL        GPIOD
#define PIN_CTL         GPIO_Pin_0

//  DS18B20 DQ控制
#define DQ_H()          PORT_DQ->DDR &=~PIN_DQ        //输入模式   变成带上拉的输入，也有对外驱动能力
#define DQ_L()          PORT_DQ->DDR |=PIN_DQ         //输出模式   推挽输出低
//  DS18B20 CTL控制
#define DS18B20_ON()    GPIO_SetBits(PORT_CTL, PIN_CTL)
#define DS18B20_OFF()   GPIO_ResetBits(PORT_CTL, PIN_CTL)//GPIO_Init(PORT_CTL, PIN_CTL, GPIO_Mode_Out_PP_Low_Slow)//


void Delay_10us(uint16_t x);
INT8U DS18B20_Initial(void);
uint8_t DS18B20_ReadByte(void);
void DS18B20_WriteByte(uint8_t dat);
INT8U DS18B20_getTempValue(INT16U *TempBuff,INT8U TBLen);

#endif