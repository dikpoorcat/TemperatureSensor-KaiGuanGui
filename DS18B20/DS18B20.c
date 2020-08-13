/*===========================================================================
公司 ：杭州方诚电力技术有限公司
作者 ：吴彦伟
功能：DS18B20温度采集功能函数实现
日期：2015.7.16
修订：2017.12.07 增加错误代码	杜
===========================================================================*/
#include "bsp.h"



/*===========================================================================
函数 ：void Delay_10us(uint16_t x)
功能 ：10US延时
输入 ：uint16_t x
输出 ：
============================================================================*/
void Delay_10us(uint16_t x)
{
    while(x--);
}
/*===========================================================================
* 函数 ：DS18B20_Initial()                                                  *
* 功能 ：初始化DS18B20的状态
* 返回 ：失败时返回1，成功返回0（真奇葩）
============================================================================*/
INT8U DS18B20_Initial(void)
{
     INT16U CiShu;

    GPIO_Init(PORT_CTL, PIN_CTL, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_Out_PP_Low_Slow);

    Delay_10us(1);
    DQ_L() ;//    GPIO_ResetBits(PORT_DQ,PIN_DQ);//拉低DQ，复位
    Delay_10us(80);//等待总线上所有器件复位
    DQ_H();//释放  带内部上拉的输入，检测DQ高低电平
     __no_operation();
    for(CiShu=0;CiShu<2000;CiShu++)
    {
        if(!(PORT_DQ->IDR & PIN_DQ))break;
    }
    if(CiShu==2000)return 1;
    for(CiShu=0;CiShu<2000;CiShu++)
    {
        if((PORT_DQ->IDR & PIN_DQ))break;
    }
    if(CiShu==2000)return 1;
    return 0;
}
/*===========================================================================
* 函数 ：uint8_t Read_Byte(void)                                            *
* 功能 ：读取总线上一个字节的数据                                           *
============================================================================*/
uint8_t DS18B20_ReadByte(void)//读一个字节
{
    uint8_t i,data=0;

    DQ_H();
    for(i=8;i>0;i--)
    {
        DQ_L();
        __no_operation();//1us
        __no_operation();//1us
        DQ_H();
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us

        data>>=1;
        if(PORT_DQ->IDR & PIN_DQ)//判断是否为高电平
        {
            data|=0x80;
        }
        Delay_10us(2);
    }
    return data;
}
/*===========================================================================
* 函数 ：Write_Byte(uint8_t dat)                                            *
* 功能 ：读取总线上一个字节的数据                                           *
============================================================================*/
void DS18B20_WriteByte(uint8_t dat)
{
    uint8_t temp,i=0;
    DQ_H();//释放

#if 0
    while(1)
    {
        Delay_10us(1);
        DQ_L();
        Delay_10us(1);
        DQ_H();//释放
    }
#endif
    for(i=0;i<8;i++)
    {
        DQ_L();//GPIO_ResetBits(PORT_DQ,PIN_DQ);
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us
        __no_operation();//1us

        temp=dat&0x01;
        if(temp) DQ_H();
        else DQ_L();
        __no_operation();//1us
        __no_operation();//1us
        Delay_10us(2);
        DQ_H();//释放
        dat>>=1;
    }
    DQ_H();
}
/*===========================================================================
函数：uint8_t DS18B20_getTempValue(unsigned short *TempBuff,uint8_t TBLen)                                        *
功能：读取温度传感去18B20的数据
入参：   TempBuff:温度存放的 位置，
         TBLen    TempBuff的长度
         存放说明：每次读取的数据都存放在TempBuff[0]
                   前面的数据依次向后移动
出参： 返回1时，表示正确，超量程、初始化失败等都返回0。
============================================================================*/
INT8U DS18B20_getTempValue(INT16U *TempBuff,INT8U TBLen)
{
    INT16U TempH=0,TempL=0;
    INT8U Over;

    if(DS18B20_Initial())							//返回1时初始化失败，上报4016 0x0FB0
    {
        TempBuff[0]=4016;
        return 0;
    }

    DS18B20_WriteByte(0xcc);						//跳过度序列号的操作
    DS18B20_WriteByte(0x44);						//启动温度转换
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_In_FL_No_IT);//切换为浮空输入，防止外部上拉消耗！

    //延时750Ms等待转换完成 实际给1.1S
	Feed_Watchdog();								//看门狗喂狗
    Delay_10us(55000);
	Feed_Watchdog();								//看门狗喂狗
    Delay_10us(55000);
	Feed_Watchdog();								//看门狗喂狗

    DS18B20_Initial();
    DS18B20_WriteByte(0xcc);						//跳过读序列号
    DS18B20_WriteByte(0xbe);						//调用写函数，读温度寄存器
    if(PORT_DQ->IDR & PIN_DQ)						//判断是否为高电平
       Over=1;
    Delay_10us(20000);								//150ms(读温度寄存器前等待时间，原为100ms，但部分探头等待时间不够，会导致温度异常，现增加至200ms)
    TempL=DS18B20_ReadByte();						//读出温度的低位
    TempH=DS18B20_ReadByte();						//读出温度的高位

    DS18B20_OFF();                      			//关TCL电源
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_In_FL_No_IT);//切换为浮空输入，防止外部上拉消耗！

	//TempBuff[0]=(uint16_t)((TempH*256+TempL)*0.625);
	TempBuff[0]=((TempH<<8)+TempL);
	if((TempBuff[0]&0xf800)==0xf800)				//负数
	{
		TempBuff[0]=(~TempBuff[0]+1);
		TempBuff[0]=TempBuff[0]*5;
		TempBuff[0]=TempBuff[0]>>3;  				//读出的数据需要*0.625  = 5/8
		if(TempBuff[0]>550)
		{
			TempBuff[0]=550;
			Over=0;									//返回0，代表测温异常
		}
		TempBuff[0]|=0xf800;
    }
    else    //非负数
    {
		TempBuff[0]=TempBuff[0]*5;
		TempBuff[0]=TempBuff[0]>>3;
		if(TempBuff[0]>1250)
		{
			TempBuff[0]=4017;						//DS18B20超量程时报4017 0x0FB1
			Over=0;									//返回0，代表测温异常
		}
    }
    return Over;
}