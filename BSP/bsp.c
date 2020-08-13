//������ӱ�� 2019.12.13

#include "bsp.h"


/*===================================================================== ͨ�� =====================================================================*/
/*===========================================================================
* ���� ��SClK_Initial() => ��ʼ��ϵͳʱ�ӣ�ϵͳʱ�� = 1MHZ                  *
============================================================================*/
void SClK_Initial(void)
{
  CLK_DeInit();
  CLK_HSICmd(ENABLE);//ʹ���ڲ���ƵRC

  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_16 );
}

/*===========================================================================
* ���� ��TIM2_Initial() => ��ʼ����ʱ��2����ʱʱ��Ϊ1ms                     *
============================================================================*/
void TIM2_Initial(void)
{
    TIM2_Cmd(DISABLE);
    TIM2_DeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
    // ����Timer2��ز�����ʱ��Ϊ16/16 = 1MHZ����ʱʱ�� = 1000/1000000 = 1ms
    TIM2_TimeBaseInit(TIM2_Prescaler_4, TIM2_CounterMode_Up, 1000);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);//���ж�
   // TIM2_ClearFlag(TIM2_FLAG_Update);
    TIM2_Cmd(ENABLE);
}
/*===========================================================================
* ���� ��TIM3_Initial() => ��ʼ����ʱ��3����ʱʱ��Ϊ1ms                     *
============================================================================*/
void TIM3_Initial(void)
{
    TIM3_Cmd(DISABLE);
    TIM3_DeInit();

    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

    // ����Timer3��ز�����ʱ��Ϊ16/16 = 1MHZ����ʱʱ�� = 1000/1000000 = 1ms
    TIM3_TimeBaseInit(TIM3_Prescaler_4, TIM3_CounterMode_Up, 1000);
    //TIM3_ITConfig(TIM3_IT_Update, DISABLE);//���ж�
    TIM3_ITConfig(TIM3_IT_Update, ENABLE);//���ж�
    TIM3_Cmd(ENABLE);
}
/*===========================================================================
* ���� ��RTC_Config(uint16_t time) => ��ʼ��RTC                    			*
============================================================================*/
//CPUʱ������Ϊ38KHzʱ����Ƶ����ʱ��Ϊ2.375KHz����ʱʱ��ΪT=time*0.42105ms
void RTC_Config(uint16_t time)
{
     //RTC_WakeUpCmd(DISABLE);
     CLK_LSICmd(ENABLE);
     RTC_DeInit();
     CLK_PeripheralClockConfig( CLK_Peripheral_RTC, ENABLE);
     CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_2);
     RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div8);
     RTC_ITConfig( RTC_IT_WUT, ENABLE);
     RTC_SetWakeUpCounter(time);
    // FLASH->CR1&=0x00;
     RTC_WakeUpCmd(ENABLE);
}


/*===================================================================== ��ʱ =====================================================================*/
/************************************************************************
������void WaitSleep(unsigned short Time,unsigned short RTC_count)
���ܣ���������״̬����Լ0.2��Ϊ��λ��
��Σ�unsigned short Time,��Ҫ���ߵ�ʱ��T=RTC_count*0.42105ms��unsigned short RTC_count��
	  RTC_Config���ü���ʱ��
���Σ�	��
*************************************************************************/
void WaitSleep(unsigned short Time,unsigned short RTC_count)
{
    unsigned short i;
    Close_Pin();				//Close_Pin���������GPIO_Initial��PT1000�Ѳ��ԣ�
    RTC_Config( RTC_count );	//T=RTC_count*0.42105ms  2300->968.4ms		460->193.7ms	400
    IWDG_Init();      			//�ڲ����Ź���ʼ�����ڲ����Ź�����1.724s��λ
	Feed_Watchdog();			//���Ź�ι��
    RTC_WakeUpCmd(ENABLE);
    PWR_UltraLowPowerCmd(ENABLE);
    for(i=0;i<Time;i++)
    {
		halt();
		Feed_Watchdog();		//���Ź�ι��
    }
}

/************************************************************************
������void WaitSleep_StartUp(unsigned short Time,unsigned short RTC_count)	Z.E.
˵������ȡ�緽ʽ�����ϵ�ʱ��Ҫһ������ʱ��ʹ��ѹԽ��1.7V���Ա�֤����λ��
��Σ�unsigned short Time��		��Ҫ���ߵ�ʱ��T=RTC_count*0.42105ms��
	  unsigned short RTC_count��RTC_Config���ü���ʱ��
���Σ���
*************************************************************************/
void WaitSleep_StartUp(unsigned short Time,unsigned short RTC_count)
{
    unsigned short i;
	asm("rim");                 							//��ȫ���ж�

	/*�������ڲ�����Close_Pin();*/
    RTC_Config( RTC_count );								//T=RTC_count*0.42105ms  2300->968.4ms		460->193.7ms	400
	IWDG_Init();      										//�ڲ����Ź���ʼ�����ڲ����Ź�����1.724s��λ
	Feed_Watchdog();										//���Ź�ι��
    RTC_WakeUpCmd(ENABLE);
    PWR_UltraLowPowerCmd(ENABLE);
    for(i=0;i<Time;i++)
    {
		halt();
		Feed_Watchdog();									//���Ź�ι��
    }
}

/************************************************************************
������void Delay_100us(unsigned short time)
���ܣ���ʱ����
��Σ�time
���Σ�
*************************************************************************/
void Delay_100us(unsigned short time)
{
    unsigned short y=0;

    while(time--)
    {
    	for(y=0;y<6;y++);
    }
}
//��ʱ10Msʱ��time=1
void Delay_10Ms(unsigned short time)
{
    unsigned short y=0;
    unsigned short x=0;
    while(time--)
    {
		for(x=0;x<10;x++)
		{
			for(y=0;y<196;y++);
			__no_operation();
			__no_operation();
		}
    }
}
//��ʱ1Msʱ��time=1
void Delay_1Ms(unsigned short time)
{
    unsigned short y=0;
    unsigned short x=0;
   	while(time--)
    {
		for(x=0;x<10;x++)
		{
			for(y=0;y<17;y++);
		}
    }
}


/*===================================================================== GPIO =====================================================================*/
/************************************************************************
������void GPIO_Initial(void)		Z.E.
���ܣ�GPIO�ڳ�ʼ�����͹���״̬�����踴λ����ã�Close_Pin��������á�
��Σ���
���Σ���
*************************************************************************/
void GPIO_Initial(void)
{
// ������������
	GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//PA������������
	GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);			//PB2���ã�������������
	GPIO_Init(GPIOC,0XEF,GPIO_Mode_Out_PP_Low_Slow);					//PC4��RF��Դ���ƣ�����Ҫ�ߵ�ƽ
	
//����ģ���Դ���ƹܽų�ʼ��
	GPIO_Init(PORT_RF, PIN_RF, GPIO_Mode_Out_PP_High_Slow);					//PC4��RF��Դ���ƣ����ų�ʼ��Ϊ�ߵ�ƽ����ʱRF��Դ�ر�	��GPIO_Mode_In_PU_No_IT���У�

//����SX1278��������
    GPIO_Init(Reset_Port, Reset_Pin, GPIO_Mode_In_PU_No_IT);			//PB3��SX1278��λ�����ų�ʼ��	
	
//���ô���������
#if DS18B20																//ʹ��PT1000ʱ�������������Ź���(PB0/PB1/PD0)
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_Out_PP_Low_Slow);    			//DS18B20�ܽų�ʼ��
    GPIO_Init(PORT_CTL, PIN_CTL, GPIO_Mode_Out_PP_Low_Slow);    		//DS18B20��Դ���ƹܽų�ʼ��
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);			//PT1000���õ�PB1�ܽű��������������ã�
#endif
}

/************************************************************************
������void GPIO_Initial_StartUp(void)		Z.E.
���ܣ�POR��GPIO�ڳ�ʼ�����͹���״̬��
��Σ���
���Σ���
*************************************************************************/
void GPIO_Initial_StartUp(void)
{
  //����ģ���Դ���ƹܽų�ʼ��
	GPIO_Init(PORT_RF, PIN_RF, GPIO_Mode_Out_PP_High_Slow);					//PC4��RF��Դ���ƣ����ų�ʼ��Ϊ�ߵ�ƽ����ʱRF��Դ�ر�	��GPIO_Mode_In_PU_No_IT���У�
	
  //�����ι��ʱ0.247mA���ң�RST��CSN��RF��WDI���Ų��ı䣬��Close_Pin���������GPIO_Initial��
	GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//ȫ������
	GPIO_Init(GPIOB,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//ȫ������
	GPIO_Init(GPIOC,0XEF,GPIO_Mode_Out_PP_Low_Slow);					//RF(PC4)����Ҫ�ߵ�ƽ����������
	//GPIO_Init(GPIOD,0XFF,GPIO_Mode_In_FL_No_IT);						//PD0��ʼ��������ᵼ��PT1000�����쳣������ֱ�Ӳ��ܴ����ţ����Ĳ���ġ���Ҫ��ʼ������ GPIO_Mode_In_FL_No_IT��In_PU���Ĵ�
}

/************************************************************************
������void Close_Pin(void)		Z.E.
���ܣ����������Խ��ܡ�Close_Pin��Ҫʹ��SPI����ҪSPI_Initial()�����������IO�������⴦��
��Σ���
���Σ���
*************************************************************************/
void Close_Pin(void)
{
  //�����ι��ʱ0.247mA���ң�RST��CSN��RF��WDI���Ų��ı䣬��Close_Pin���������GPIO_Initial��
	GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//WDI(PA2)����Ҫ�͵�ƽ OK
	GPIO_Init(GPIOB,0XE7,GPIO_Mode_Out_PP_Low_Slow);					//RST(PB3)�����ͻ���1mA��������MOSI(PB6)������OD_LOW��������PP_LOW��CSN(PB4)��stdbyҪ����	0XE7
	GPIO_Init(GPIOC,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//RF(PC4)����Ҫ�ߵ�ƽ->���̽ͷ��������ȥ��������
//	GPIO_Init(GPIOD,0XFF,GPIO_Mode_In_FL_No_IT);						//PD0��ʼ��������ᵼ��PT1000�����쳣������ֱ�Ӳ��ܴ����ţ����Ĳ���ġ���Ҫ��ʼ������ GPIO_Mode_In_FL_No_IT��In_PU���Ĵ�

	//SX1278 sleepģʽ�������ŵ͹��Ĵ���ʹ��ʱ��ע�ͳ�ʼ����
	GPIO_Init(Reset_Port, Reset_Pin, GPIO_Mode_In_PU_No_IT);			// RST  (PB3)	ʹ��ʱ��SX1276Reset()���г�ʼ����OD_LOW
	GPIO_Init(PORT_CC_CSN, PIN_CC_CSN, GPIO_Mode_Out_PP_Low_Slow);		// CSN	(PB4)	��ͬ����ǰ��GPIO_Mode_Out_PP_High_Slow
															
	//����
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
	CLK_PeripheralClockConfig( CLK_Peripheral_USART1, DISABLE);
}

/*===================================================================== SPI =====================================================================*/
/************************************************************************
������void SPI_Initial(void)
���ܣ���ʼ��SPI  
��Σ���
���Σ���
*************************************************************************/
void SPI_Initial(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
    SPI_DeInit(SPI1);

    // ����SPI��ز���,2��Ƶ��8MHZ��
    SPI_Init(SPI1,SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2,
    SPI_Mode_Master, SPI_CPOL_Low, SPI_CPHA_1Edge,
    SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft,0);
    SPI_Cmd(SPI1,ENABLE);

    // SPI���IO������
    GPIO_Init(PORT_CC_CSN, PIN_CC_CSN, GPIO_Mode_Out_PP_Low_Slow);		// CSN	(PB4)
    GPIO_Init(PORT_SPI, PIN_MISO, GPIO_Mode_In_PU_No_IT);      	 		// MISO (PB7)
    GPIO_Init(PORT_SPI, PIN_SCLK, GPIO_Mode_Out_PP_High_Fast);  		// SCLK (PB5)
    GPIO_Init(PORT_SPI, PIN_MOSI, GPIO_Mode_Out_PP_High_Fast);  		// MOSI (PB6)
}

/************************************************************************
������INT8U SPI_ExchangeByte(SPI_TypeDef* SPIx,INT8U input)
���ܣ�ͨ��SPI�������ݽ���  
��Σ���Ҫд��SPI��ֵ
���Σ�ͨ��SPI������ֵ
*************************************************************************/
INT8U SPI_ExchangeByte(SPI_TypeDef* SPIx,INT8U input)
{
	INT16U i=60000;
	SPI_SendData(SPIx,input);
	while ((RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_TXE))&&(i--));	//�ȴ����ݷ������

	Feed_Watchdog();													//���Ź�ι��
	i=60000;
	while ((RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_RXNE))&&(i--));	//�ȴ����ݽ������

	Feed_Watchdog();													//���Ź�ι��
	return (SPI_ReceiveData(SPIx));
}


/*===================================================================== ���Ź� =====================================================================*/
/**********************************************************************
������void IWDG_Init()
���ܣ��ڲ����Ź���ʼ�����ڲ����Ź�����1.724s��λ
��Σ���
���Σ���
***********************************************************************/
void IWDG_Init()
{
    IWDG_Enable();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0xff);
    IWDG_ReloadCounter();
}

/**********************************************************************
������void Feed_Watchdog()
���ܣ����Ź�ι������ѡ�ڲ����Ź����ⲿ���Ź���
��Σ���
���Σ���
***********************************************************************/
void Feed_Watchdog()
{
#if ZIQUDIAN 															//�Լ�ȡ��̽ͷ���⹷
	WDI();																//�ⲿ���Ź�ι����Լ1.6S��λ��
#endif
	IWDG_ReloadCounter();												//�ڲ����Ź�ι�����ڲ����Ź�����1.724s��λ
}

/**********************************************************************
������void WDI(void)
���ܣ��ⲿ���Ź�ι��
��Σ���
���Σ���
***********************************************************************/
void WDI(void)
{
  	GPIO_SetBits(PORT_WDI,PIN_WDI);
  	__no_operation();//1us
    GPIO_ResetBits(PORT_WDI,PIN_WDI);
}


/*end*/
