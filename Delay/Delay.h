#ifndef _DELAY_H_
#define _DELAY_H_
/*===========================================================================
��˾ �����ݷ��ϵ����������޹�˾
���� ����ӱ��
���ܣ��ö�ʱ��TIM4ʵ����ʱ����
���ڣ�2016.12.21
===========================================================================*/
#include "STM8l15x_conf.h"
#include "mytypedef.h"
#include "stdarg.h"
#include "stm8l15x.h"

//��������
extern void TimingDelay_Decrement(void);

//��������
extern void Delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void Delay_Initial(void);
static void TIM4_Config(void);
#endif