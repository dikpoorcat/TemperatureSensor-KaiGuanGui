#ifndef _PT1000_RC_H_
#define _PT1000_RC_H_
/*===========================================================================
��˾ �����ݷ��ϵ����������޹�˾
���� ����ӱ��
���ܣ�PT1000-RC�¶Ȳɼ����ܺ���ʵ��
���ڣ�2016.12.21
===========================================================================*/
#include "STM8l15x_conf.h"
#include "mytypedef.h"
#include "stdarg.h"
#include "stm8l15x.h"
#include "Delay.h"
#include "main.h"

//����
#define PORT_Rref       GPIOB
#define PIN_Rref        GPIO_Pin_1
#define PORT_Rx         GPIOB
#define PIN_Rx          GPIO_Pin_0

#define R_REF			1
#define R_PT1000		0

////��������
//uint16_t captureValue;
//const float PT1000FD[][10];

//��������
INT8U PT1000_getTempValue(INT16U *TempBuff);
void Sampling_Res(INT8U res);
uint16_t Value_Res(INT8U res);
void PT1000_Initial(void);
static void CLK_Config(void);
static void COMP_Config(void);
static void TIM2_Config(void);

#endif