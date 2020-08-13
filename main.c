/*===========================================================================
公司：杭州方诚电力技术有限公司
作者：杜颖成，杜晓瞻
功能：SX1278探头程序LoRa收发功能及温度采集等
日期：2016.11.02
修订：2017.02.22	修改为脱机下载器使用(EEPROM)
修订：2017.03.08	增加发送完成中断检测
修订：2017.06.29	PT1000作传感器时上下限温度处理；同时支持DS18B20
修订：2017.09.26	9种错误代码明确：为4001~4005、高温3150、低温-500等
修订：2017.10.23	部分DS18B20读温度寄存器前等待时间不够会导致温度异常，增加至150ms
修订：2018.01.09	SX1278驱动重做，各种模式下低功耗实现；外部看门狗WDI引脚从PC1更改到PA2，目前自取电板整机低功耗（休眠+喂狗+Sleep_mode）113uA；头文件整理；main.c整理，部分函数重做并移到bsp.c
修订：2018.04.19	清理无用程序Tempadjust部分等
修订：2018.04.25	适用于电池探头，对应的硬件上删除了外部看门狗等器件，调整相关引脚以降低功耗。DS18B20采样次数调整为1次
修订：2018.06.27	电池探头增加高温报警功能，当温度高于75摄氏度时1min/次上报；去掉了电池探头温度采集后的30秒延时，且复位后立刻发送用于生产测试
修订：2018.11.07	电池探头低温测试时发现会变成1min/次，原因是负温前面0XF800被判断为>750导致，现已Save_Temp&0x07ff；DS18B20错误代码时也禁止1min发送。全局修改：超过125度时上报4017。
修订：2018.11.23	PT1000_getTempValue仅获取到正确温度时返回1，错误代码、超量程、采样失败等都返回0。
修订：2018.12.06	DS18B20采集到125、85度时重新采样；DS18B20驱动延时适当加长。
修订：2019.10.30	加入开关柜宏定义，适用于开关柜初版样机。
修订：2019.11.08	开关柜自取电程序基本完成。（射频参数未调整）
修订：2019.11.14	PT1000驱动量程调整，测温范围确定为-60~290℃，已校正。（杜晓瞻）
修订：2019.11.18	PT1000驱动优化，使用Medium优化等级可成功烧录。开关柜lora参数修改。
修订：2019.11.27	开关柜lora参数修改；温度采集与射频发送之间隔开，防止电压下拉过大。DS18B20采集到85度和125度时强制修改为错误代码（此后接收板可以不用改了）。
修订：2019.12.04	修改了RF_Wait_PacketSent\RF_Wait_PacketReceive\RF_Wait_Receive_single，每隔1秒喂狗。
修订：2019.12.06	将PayloadCrcError判断放到接收驱动底层；整理了SX1278驱动（与接收板相同）。
修订：2019.12.23	开关柜四代（无电压检测开关电路）实现5A情况慢启动，低功耗处理完成，脱机烧录测试通过。受限于FLASH，测温范围修改为-60~290℃（FLASH受限）。config_str结构体修改为INT16U。
修订：2019.12.23	增加电压，固定3.3
===========================================================================*/
#include "main.h"

/*全局变量定义*/
struct config_str KGG_Config={				//开关柜默认配置。存放已录入的配置信息，从EEPROM读取
	.T_Under = 5,							//正常发射间隔，单位：分钟
	.T_Over = 1,							//高温发射间隔，单位：分钟（高于报警温度）
	.T_Sample = 60,							//温度采集间隔，单位：秒
	.Tem_Alarm = 75,						//报警温度，单位：摄氏度（请勿设置成60以下，否则-60度时会误报）
	.Tem_Diff = 2							//报警温差，单位：摄氏度
};
INT8U	Device_Addr[2]={ADDER1,ADDER2};		//探头ID
INT8U 	RFRxBuff[255]={0};					//最多255就够，一次发送最多255字节
INT8U 	ReNum=0;							//接收到的字节数
INT8U 	RSSI=0;
INT8U 	Tx_Buff[255];						//存放发送的数据（4字节地址，1字节命令，2字节温度值，1字节错误参数）。一次发送最多255字节
INT16U  Save_Temp=0;						//存放采集的温度值（实际温度的10倍，数值无转码）
INT16U  Save_Temp_old=0;					//存放上次采集的温度（实际温度的10倍，数值无转码）
INT16U	T_sleep=0;

INT8U Vbat=33;

/************************************
main函数
************************************/
int main(void)
{
	SClK_Initial();             					//初始化系统时钟，1M
	GPIO_Initial_StartUp();
	WaitSleep_StartUp(60,2300);						//越过1.7V的电压后，才能执行后面（EEP操作可在前）

	GPIO_Initial();		
	SPI_Initial();		
	RF_Initial();
	IWDG_Init();                					//内部看门狗初始化。内部看门狗配置1.724s复位
#if !test
	E2P_Read(0,Device_Addr,2);    					//读EEPROM（注意：在线烧录时EEPROM无写入，地址为0）
	E2P_Read(2,(INT8U*)&KGG_Config.T_Under,10);    	//读EEPROM（注意：在线烧录时EEPROM无写入，配置参数为默认值）
	Unit_Conversion();								//计算T_sleep；将KGG_Config中，以分钟为单位的值转换为秒，温度单位扩大10倍。
#endif
	Feed_Watchdog();								//看门狗喂狗

#if test
	while(1)
	{
		Get_Temp();
		WaitSleep( 30, 2300 );
		Send_Temp();
		WaitSleep( 30, 2300 );
	}
#else
	while(1)
	{
		Temperature_Handle();						//采集温度并处理
		Send_Temp();
		WaitSleep(T_sleep,2300);					//防止发射后立刻采集；也作补足延时用（Temperature_Handle()里面少了T_Sample/2）
	}
#endif
}













/**********************************************************************
函数：void Temperature_Handle(void)
功能：采集温度并根据不同温度进行延时，延时结束、温度采集异常、或温差报警
	  时跳出。
入参：无
出参：无
***********************************************************************/
void Temperature_Handle(void)
{
	INT8U   collect=1;									//温度采集次数计数，发射后重置
	INT8U   Sta=0;
	INT16U	Delt_Temp=0;
	
	while(1)
	{
		Sta=Get_Temp();									//采集温度（开关柜已去掉延时） 先测温后延时（先消耗再充电）
		WaitSleep(T_sleep,2300);						//温度采集间隔为T_Sample	采集后若break，在发射后补回T_Sample/2延时
		Delt_Temp = Get_Delt_Temp(Save_Temp,Save_Temp_old);		//两次采集的温度差
		Save_Temp_old=Save_Temp;						//更新Save_Temp_old

		if(Sta && (Delt_Temp<KGG_Config.Tem_Diff))		//温度采集正常，且两次采集温差小于2摄氏度
		{
			if((Save_Temp&0x07ff)<KGG_Config.Tem_Alarm)	//温度正常时，发射频率低
			{
				if(collect*KGG_Config.T_Sample>=KGG_Config.T_Under) break;		//判断发射间隔，超时跳出
			}
			else										//超过警戒温度，增加发射频率
			{
				if(collect*KGG_Config.T_Sample>=KGG_Config.T_Over) break;		//判断发射间隔，超时跳出
			}
			collect++;									//计时，并继续循环
		}
		else break;	  								//温度采集异常，或温差报警
		
		WaitSleep(T_sleep,2300);						//将延时分为两段的原因：防止采集、发射动作紧接着，导致电压下拉过大
	}
}

/**********************************************************************
函数：void Temperature_Handle(void)
功能：计算T_sleep；将KGG_Config中，以分钟为单位的值转换为秒，温度单位扩大10倍。
入参：无
出参：无
***********************************************************************/
void Unit_Conversion( void )
{
	T_sleep = KGG_Config.T_Sample/2;					//每次休眠时间为温度采集时间的一半
	KGG_Config.T_Under *= 60;							//正常发射间隔，单位：分钟->秒
	KGG_Config.T_Over *= 60;							//高温发射间隔，单位：分钟->秒（高于报警温度）
	/*KGG_Config.T_Sample单位为秒不用变*/
	KGG_Config.Tem_Alarm *= 10;							//报警温度，单位：摄氏度->10倍（请勿设置成60以下，否则-60度时会误报）
	KGG_Config.Tem_Diff *= 10;							//报警温差，单位：摄氏度->10倍
}

/**********************************************************************
函数：INT16U 	Get_Delt_Temp(INT16U temp1, INT16U temp2)
功能：计算温度差，已考虑DS18B20的负数表示。
入参：INT16U temp1，温度1；INT16U temp2，温度2。
出参：温度差
***********************************************************************/
INT16U 	Get_Delt_Temp(INT16U temp1, INT16U temp2)
{
	if((temp1^temp2)&0xE000)							//若高3位不同，则一个正数一个负数
	{
		return (temp1 + temp2)&0x07ff;					//异号的温度差
	}
	return delt(Save_Temp,Save_Temp_old);				//同号的温度差
}

/**********************************************************************
函数：void Get_Temp(void)
功能：温度采集，可选PT1000和DS18B20
入参：无
出参：返回值Statuse，若正确则返回1，否则返回0.
***********************************************************************/
INT8U Get_Temp(void)
{
#if PT1000  //PT1000作传感器
    INT8U Statuse=0;

    Statuse=PT1000_getTempValue(&Save_Temp);

//    //温度采集状态,需要？S左右，加喂狗
//	Feed_Watchdog();									//看门狗喂狗

    #if ( (!BATTERY) && (!test) && (!KAIGUAN))			//电池探头（BATTERY=1）或测试（test=1）或开关柜（KAIGUAN=1）时不会进入此句
    WaitSleep( 30, 2300 );								//延时30秒
    #endif

    return Statuse;
#endif
#if DS18B20 //DS18B20作传感器
    INT8U Statuse=0;

	#if BATTERY //电池探头 只采样一次
	Statuse=DS18B20_getTempValue( &Save_Temp,1);		//温度采集状态,需要1S左右，加喂狗
	Feed_Watchdog();
	if( (Save_Temp==850)||(Save_Temp==1250) )			//若采集到85度/125度，极可能是异常值，再次采集
	{
		Statuse=DS18B20_getTempValue( &Save_Temp,1);	//温度采集状态,需要1S左右，加喂狗
		Feed_Watchdog();
	}

	#else
    INT8U i,m,n;
    INT16U TEMP[3];
    INT16U temp;

	for(i=0;i<3;i++)
	{
		Statuse=DS18B20_getTempValue( &Save_Temp,1);															//采集了三次，有成功有失败时  Statuse怎么处理
		//温度采集状态,需要1S左右，加喂狗
		Feed_Watchdog();								//看门狗喂狗
		TEMP[i]=Save_Temp;
		#if ( (!BATTERY) && (!test) && (!KAIGUAN))		//电池探头（BATTERY=1）或测试（test=1）或开关柜（KAIGUAN=1）时不会进入此句
		WaitSleep( 10, 2300 );               			//4*i*i+3  4S,8S,20S
		#endif
	}
	//从大到小排序
	for(m=0;m<2;m++)
	{
		for(n=m+1;n<3;n++)
		{
			if(TEMP[n]>=TEMP[m])
			{
				temp=TEMP[m];
				TEMP[m]=TEMP[n];
				TEMP[n]=temp;
			}
		}
	}
    Save_Temp=  TEMP[1];								//取中位数
	if( Save_Temp==850 ) Save_Temp = 4017;				//若采集到85度，改写为错误代码4017，即0x0FB1
	if( Save_Temp==1250 ) Save_Temp = 4018;				//若采集到125度，改写为错误代码4018，即0x0FB2
	#endif

    GPIO_ResetBits(PORT_DQ,PIN_DQ);						//复位
    DS18B20_OFF();
    return Statuse;
#endif
}

/**********************************************************************
函数：unsigned char Send_Temp(void)
功能：发送组帧，并检查信道是否忙，不忙就发送数据，最多重试3次。
入参：无
出参：发送完成返回1；发送超时返回0xff；信道忙返回0。
***********************************************************************/
unsigned char Send_Temp(void)
{
	INT8U 	Count=5;							//CAD检测冲突时重发次数
	INT8U	Err=0;

	Tx_Buff[0]=Device_Addr[0];
	Tx_Buff[1]=Device_Addr[1];
	Tx_Buff[2]=Save_Temp>>8;
	Tx_Buff[3]=Save_Temp&0xff;
/*****************************************************修改**********************************************************************/
	Tx_Buff[4]=Vbat;
/*****************************************************修改**********************************************************************/
	while(Count--)
	{
		SPI_Initial();      					//初始化SPI
		Feed_Watchdog();						//看门狗喂狗
/*****************************************************修改**********************************************************************/
		Err=SX1278_TX( Tx_Buff , 1000 , 5 );	//LoRa发送数据，最多等待1000ms
/*****************************************************修改**********************************************************************/
		if( Err )								//Err非0，表示已进行发送
		{
			return Err;
		}
	}
	SX1276LoRaSetOpMode( Sleep_mode );
	return 0;									//三次检测信道都忙
}

/**********************************************************************
函数：unsigned char Send_Test( unsigned char Num )
功能：发送组帧，并检查信道是否忙，不忙就发送数据，最多重试3次。
入参：无
出参：发送完成返回1；发送超时返回0xff；信道忙返回0。
***********************************************************************/
unsigned char Send_Test( unsigned char Num )
{
	INT8U 	Count=3;							//CAD检测冲突时重发次数
	INT8U	Err=0;

	Tx_Buff[0]=Device_Addr[0];
	Tx_Buff[1]=Device_Addr[1];
	Tx_Buff[2]=Save_Temp>>8;
	Tx_Buff[3]=Save_Temp&0xff;
	for(Count=4;Count<Num;Count++)
	{
		Tx_Buff[Count]=Count;
	}

	while(Count--)
	{
		SPI_Initial();      					//初始化SPI
		Feed_Watchdog();						//看门狗喂狗

		if(Num<4) Num=4;
		Err=SX1278_TX( Tx_Buff , 3000 ,Num );	//LoRa发送数据，最多等待3000ms
		if( Err )								//Err非0，表示已进行发送
		{
			return Err;
		}
	}
	SX1276LoRaSetOpMode( Sleep_mode );
	return 0;									//三次检测信道都忙
}

/**********************************************************************
函数：void Alarm_Reset(void)
功能：当MCU复位时可从电流变化中看出来
入参：无
出参：无
***********************************************************************/
void Alarm_Reset(void)
{
#if 0
  	int i;
	for(i=0;i<2;i++)
	{
	  //大电流
		GPIO_Init(GPIOA,0XFF,GPIO_Mode_In_FL_IT);//
		GPIO_Init(GPIOB,0XFF,GPIO_Mode_In_FL_IT);//
		GPIO_Init(GPIOC,0XFF,GPIO_Mode_In_FL_IT);//RF/WDI引脚要高电平
//		GPIO_Init(GPIOD,0XFF,GPIO_Mode_In_FL_IT);//COM引脚尽量不去动它
		Delay_10Ms(70);
		Feed_Watchdog();						//看门狗喂狗
	  //小电流
		GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_OD_Low_Slow);//
		GPIO_Init(GPIOB,0XFF,GPIO_Mode_Out_OD_Low_Slow);//
		GPIO_Init(GPIOC,0XFF,GPIO_Mode_Out_OD_Low_Slow);//RF/WDI引脚要高电平
//		GPIO_Init(GPIOD,0XFF,GPIO_Mode_Out_OD_Low_Slow);//COM引脚尽量不去动它
		Delay_10Ms(70);
		Feed_Watchdog();						//看门狗喂狗
	}
#else
	INT8U	t = 0xFF;
	
	GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(3);
	GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);		//	_|_
	while(t--) Delay_100us(3);
	GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_OD_Low_Slow);

	while(t--) Delay_100us(3);
	GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);		//	_|_
	while(t--) Delay_100us(3);
	GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_OD_Low_Slow);
#endif
}

/**********************************************************************
函数：void Alarm_Test(void)
功能：可从电压变化中看出来
入参：无
出参：无
***********************************************************************/
void Alarm_Test(void)
{
	INT8U	t = 0xFF;
	
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);		//	_|_ 	高阻，电压更低
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(20);
	
//	Delay_10Ms(50);
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);		//	_|_		高阻，电压更低
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(20);
}
