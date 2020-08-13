//整理：杜颖成 2019.12.13

#include "bsp.h"


/*===================================================================== 通用 =====================================================================*/
/*===========================================================================
* 函数 ：SClK_Initial() => 初始化系统时钟，系统时钟 = 1MHZ                  *
============================================================================*/
void SClK_Initial(void)
{
  CLK_DeInit();
  CLK_HSICmd(ENABLE);//使能内部高频RC

  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_16 );
}

/*===========================================================================
* 函数 ：TIM2_Initial() => 初始化定时器2，定时时间为1ms                     *
============================================================================*/
void TIM2_Initial(void)
{
    TIM2_Cmd(DISABLE);
    TIM2_DeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
    // 配置Timer2相关参数，时钟为16/16 = 1MHZ，定时时间 = 1000/1000000 = 1ms
    TIM2_TimeBaseInit(TIM2_Prescaler_4, TIM2_CounterMode_Up, 1000);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);//打开中断
   // TIM2_ClearFlag(TIM2_FLAG_Update);
    TIM2_Cmd(ENABLE);
}
/*===========================================================================
* 函数 ：TIM3_Initial() => 初始化定时器3，定时时间为1ms                     *
============================================================================*/
void TIM3_Initial(void)
{
    TIM3_Cmd(DISABLE);
    TIM3_DeInit();

    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

    // 配置Timer3相关参数，时钟为16/16 = 1MHZ，定时时间 = 1000/1000000 = 1ms
    TIM3_TimeBaseInit(TIM3_Prescaler_4, TIM3_CounterMode_Up, 1000);
    //TIM3_ITConfig(TIM3_IT_Update, DISABLE);//关中断
    TIM3_ITConfig(TIM3_IT_Update, ENABLE);//开中断
    TIM3_Cmd(ENABLE);
}
/*===========================================================================
* 函数 ：RTC_Config(uint16_t time) => 初始化RTC                    			*
============================================================================*/
//CPU时钟配置为38KHz时，分频后唤醒时钟为2.375KHz，定时时间为T=time*0.42105ms
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


/*===================================================================== 延时 =====================================================================*/
/************************************************************************
函数：void WaitSleep(unsigned short Time,unsigned short RTC_count)
功能：进入休眠状态，以约0.2秒为单位，
入参：unsigned short Time,需要休眠的时间T=RTC_count*0.42105ms；unsigned short RTC_count，
	  RTC_Config设置计数时间
出参：	无
*************************************************************************/
void WaitSleep(unsigned short Time,unsigned short RTC_count)
{
    unsigned short i;
    Close_Pin();				//Close_Pin后无需调用GPIO_Initial（PT1000已测试）
    RTC_Config( RTC_count );	//T=RTC_count*0.42105ms  2300->968.4ms		460->193.7ms	400
    IWDG_Init();      			//内部看门狗初始化。内部看门狗配置1.724s复位
	Feed_Watchdog();			//看门狗喂狗
    RTC_WakeUpCmd(ENABLE);
    PWR_UltraLowPowerCmd(ENABLE);
    for(i=0;i<Time;i++)
    {
		halt();
		Feed_Watchdog();		//看门狗喂狗
    }
}

/************************************************************************
函数：void WaitSleep_StartUp(unsigned short Time,unsigned short RTC_count)	Z.E.
说明：自取电方式供电上电时需要一定的延时，使电压越过1.7V，以保证不复位。
入参：unsigned short Time，		需要休眠的时间T=RTC_count*0.42105ms；
	  unsigned short RTC_count，RTC_Config设置计数时间
出参：无
*************************************************************************/
void WaitSleep_StartUp(unsigned short Time,unsigned short RTC_count)
{
    unsigned short i;
	asm("rim");                 							//开全局中断

	/*区别在于不调用Close_Pin();*/
    RTC_Config( RTC_count );								//T=RTC_count*0.42105ms  2300->968.4ms		460->193.7ms	400
	IWDG_Init();      										//内部看门狗初始化。内部看门狗配置1.724s复位
	Feed_Watchdog();										//看门狗喂狗
    RTC_WakeUpCmd(ENABLE);
    PWR_UltraLowPowerCmd(ENABLE);
    for(i=0;i<Time;i++)
    {
		halt();
		Feed_Watchdog();									//看门狗喂狗
    }
}

/************************************************************************
函数：void Delay_100us(unsigned short time)
功能：延时函数
入参：time
出参：
*************************************************************************/
void Delay_100us(unsigned short time)
{
    unsigned short y=0;

    while(time--)
    {
    	for(y=0;y<6;y++);
    }
}
//延时10Ms时，time=1
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
//延时1Ms时，time=1
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
函数：void GPIO_Initial(void)		Z.E.
功能：GPIO口初始化，低功耗状态。仅需复位后调用，Close_Pin后无需调用。
入参：无
出参：无
*************************************************************************/
void GPIO_Initial(void)
{
// 配置闲置引脚
	GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//PA口无特殊引脚
	GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);			//PB2闲置，其他另作配置
	GPIO_Init(GPIOC,0XEF,GPIO_Mode_Out_PP_Low_Slow);					//PC4（RF电源控制）引脚要高电平
	
//无线模块电源控制管脚初始化
	GPIO_Init(PORT_RF, PIN_RF, GPIO_Mode_Out_PP_High_Slow);					//PC4（RF电源控制）引脚初始化为高电平，此时RF电源关闭	（GPIO_Mode_In_PU_No_IT不行）

//配置SX1278控制引脚
    GPIO_Init(Reset_Port, Reset_Pin, GPIO_Mode_In_PU_No_IT);			//PB3（SX1278复位）引脚初始化	
	
//配置传感器引脚
#if DS18B20																//使用PT1000时驱动中已有引脚管理(PB0/PB1/PD0)
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_Out_PP_Low_Slow);    			//DS18B20管脚初始化
    GPIO_Init(PORT_CTL, PIN_CTL, GPIO_Mode_Out_PP_Low_Slow);    		//DS18B20电源控制管脚初始化
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);			//PT1000所用的PB1管脚保护（此引脚闲置）
#endif
}

/************************************************************************
函数：void GPIO_Initial_StartUp(void)		Z.E.
功能：POR后GPIO口初始化，低功耗状态。
入参：无
出参：无
*************************************************************************/
void GPIO_Initial_StartUp(void)
{
  //无线模块电源控制管脚初始化
	GPIO_Init(PORT_RF, PIN_RF, GPIO_Mode_Out_PP_High_Slow);					//PC4（RF电源控制）引脚初始化为高电平，此时RF电源关闭	（GPIO_Mode_In_PU_No_IT不行）
	
  //整板仅喂狗时0.247mA左右（RST、CSN、RF、WDI引脚不改变，故Close_Pin后无需调用GPIO_Initial）
	GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//全部拉低
	GPIO_Init(GPIOB,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//全部拉低
	GPIO_Init(GPIOC,0XEF,GPIO_Mode_Out_PP_Low_Slow);					//RF(PC4)引脚要高电平，其他拉低
	//GPIO_Init(GPIOD,0XFF,GPIO_Mode_In_FL_No_IT);						//PD0初始化成输出会导致PT1000驱动异常，可以直接不管此引脚，功耗不变的。非要初始化可用 GPIO_Mode_In_FL_No_IT（In_PU功耗大）
}

/************************************************************************
函数：void Close_Pin(void)		Z.E.
功能：控制引脚以节能。Close_Pin若要使用SPI，需要SPI_Initial()；否则无需对IO进行特殊处理。
入参：无
出参：无
*************************************************************************/
void Close_Pin(void)
{
  //整板仅喂狗时0.247mA左右（RST、CSN、RF、WDI引脚不改变，故Close_Pin后无需调用GPIO_Initial）
	GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//WDI(PA2)引脚要低电平 OK
	GPIO_Init(GPIOB,0XE7,GPIO_Mode_Out_PP_Low_Slow);					//RST(PB3)，拉低会有1mA过电流；MOSI(PB6)不可用OD_LOW，但可以PP_LOW；CSN(PB4)在stdby要上拉	0XE7
	GPIO_Init(GPIOC,0XFF,GPIO_Mode_Out_PP_Low_Slow);					//RF(PC4)引脚要高电平->电池探头此引脚已去掉，拉低
//	GPIO_Init(GPIOD,0XFF,GPIO_Mode_In_FL_No_IT);						//PD0初始化成输出会导致PT1000驱动异常，可以直接不管此引脚，功耗不变的。非要初始化可用 GPIO_Mode_In_FL_No_IT（In_PU功耗大）

	//SX1278 sleep模式特殊引脚低功耗处理（使用时按注释初始化）
	GPIO_Init(Reset_Port, Reset_Pin, GPIO_Mode_In_PU_No_IT);			// RST  (PB3)	使用时在SX1276Reset()中有初始化成OD_LOW
	GPIO_Init(PORT_CC_CSN, PIN_CC_CSN, GPIO_Mode_Out_PP_Low_Slow);		// CSN	(PB4)	不同：以前是GPIO_Mode_Out_PP_High_Slow
															
	//其他
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
	CLK_PeripheralClockConfig( CLK_Peripheral_USART1, DISABLE);
}

/*===================================================================== SPI =====================================================================*/
/************************************************************************
函数：void SPI_Initial(void)
功能：初始化SPI  
入参：无
出参：无
*************************************************************************/
void SPI_Initial(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
    SPI_DeInit(SPI1);

    // 配置SPI相关参数,2分频（8MHZ）
    SPI_Init(SPI1,SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2,
    SPI_Mode_Master, SPI_CPOL_Low, SPI_CPHA_1Edge,
    SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft,0);
    SPI_Cmd(SPI1,ENABLE);

    // SPI相关IO口配置
    GPIO_Init(PORT_CC_CSN, PIN_CC_CSN, GPIO_Mode_Out_PP_Low_Slow);		// CSN	(PB4)
    GPIO_Init(PORT_SPI, PIN_MISO, GPIO_Mode_In_PU_No_IT);      	 		// MISO (PB7)
    GPIO_Init(PORT_SPI, PIN_SCLK, GPIO_Mode_Out_PP_High_Fast);  		// SCLK (PB5)
    GPIO_Init(PORT_SPI, PIN_MOSI, GPIO_Mode_Out_PP_High_Fast);  		// MOSI (PB6)
}

/************************************************************************
函数：INT8U SPI_ExchangeByte(SPI_TypeDef* SPIx,INT8U input)
功能：通过SPI进行数据交换  
入参：需要写入SPI的值
出参：通过SPI读出的值
*************************************************************************/
INT8U SPI_ExchangeByte(SPI_TypeDef* SPIx,INT8U input)
{
	INT16U i=60000;
	SPI_SendData(SPIx,input);
	while ((RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_TXE))&&(i--));	//等待数据发送完成

	Feed_Watchdog();													//看门狗喂狗
	i=60000;
	while ((RESET == SPI_GetFlagStatus(SPIx,SPI_FLAG_RXNE))&&(i--));	//等待数据接收完成

	Feed_Watchdog();													//看门狗喂狗
	return (SPI_ReceiveData(SPIx));
}


/*===================================================================== 看门狗 =====================================================================*/
/**********************************************************************
函数：void IWDG_Init()
功能：内部看门狗初始化。内部看门狗配置1.724s复位
入参：无
出参：无
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
函数：void Feed_Watchdog()
功能：看门狗喂狗，可选内部看门狗、外部看门狗。
入参：无
出参：无
***********************************************************************/
void Feed_Watchdog()
{
#if ZIQUDIAN 															//自己取电探头有外狗
	WDI();																//外部看门狗喂狗。约1.6S复位？
#endif
	IWDG_ReloadCounter();												//内部看门狗喂狗。内部看门狗配置1.724s复位
}

/**********************************************************************
函数：void WDI(void)
功能：外部看门狗喂狗
入参：无
出参：无
***********************************************************************/
void WDI(void)
{
  	GPIO_SetBits(PORT_WDI,PIN_WDI);
  	__no_operation();//1us
    GPIO_ResetBits(PORT_WDI,PIN_WDI);
}


/*end*/
