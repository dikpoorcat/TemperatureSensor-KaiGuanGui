#ifndef _DELAY_H_
#define _DELAY_H_
/*===========================================================================
公司 ：杭州方诚电力技术有限公司
作者 ：杜颖成
功能：用定时器TIM4实现延时功能
日期：2016.12.21
===========================================================================*/
#include "STM8l15x_conf.h"
#include "mytypedef.h"
#include "stdarg.h"
#include "stm8l15x.h"

//变量声明
extern void TimingDelay_Decrement(void);

//函数声明
extern void Delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void Delay_Initial(void);
static void TIM4_Config(void);
#endif