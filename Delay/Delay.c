/*===========================================================================
��˾ �����ݷ��ϵ����������޹�˾
���� ����ӱ��
���ܣ��ö�ʱ��TIM4ʵ����ʱ����
���ڣ�2016.12.21
===========================================================================*/
#include "Delay.h"

__IO uint32_t TimingDelay;


/***************************************************
������void Delay_ms(__IO uint32_t nTime)
���ܣ�
��Σ�
���Σ�
***************************************************/
void Delay_ms(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while (TimingDelay != 0);
}

/***************************************************
������void TimingDelay_Decrement(void)
���ܣ�
��Σ�
���Σ�
***************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

/*===========================================================================
* ���� ��Delay_Initial() => ��ʼ����ʱ��������ʱʱ��Ϊ1ms                     *
============================================================================*/
void Delay_Initial(void)
{
    /* Enable TIM4 CLK */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);

    /* TIM4 configuration -------------------------------------------*/
    TIM4_Config();          //1ms�ж�
}

/*===========================================================================
* ���� ��static void TIM4_Config(void) => ��ʼ����ʱ��4                  *
============================================================================*/
static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 1 MHz, the TIM4 Prescaler is equal to 8 so the TIM1 counter
   clock used is 1 MHz / 8 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_Prescaler_8, 124);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

////stm8l15x_it.c���޸�TIM4�жϺ���
///*
//  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
//  * @param  None
//  * @retval None
//  */
//INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler,25)
//{
//    /* In order to detect unexpected events during development,
//       it is recommended to set a breakpoint on the following instruction.
//    */
//  TimingDelay_Decrement();
//  /* Cleat Interrupt Pending bit */
//  TIM4_ClearITPendingBit(TIM4_IT_Update);  
//}