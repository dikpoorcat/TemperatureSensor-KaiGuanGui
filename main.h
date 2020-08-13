#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "MyTypeDef.h"
#include "bsp.h"
#include "stdio.h"
#include "stdarg.h"
#include "EEPROM.H"
#include "time.h"
#include "PT1000-RC.h"
#include "SX1278.h"
//#include "Delay.h"



//放到main.c不行
#define test            0					//测试时选1（仅main.c生效）
#define ZIQUDIAN		0					//自取电探头选1（发射功率为20dBm，已加至预处理）
#define BATTERY        	0					//电池探头选1（发射功率为2dBm，已加至预处理）
#define KAIGUAN         01					//开关柜探头选1（发射功率为8dBm，已加至预处理）

#define DS18B20         0
#define PT1000          01


#define Addr            123					//数字前面加个 0 表示 8 进制，因此不可用09等表示
#define ADDER1          Addr>>8
#define ADDER2          Addr&0xff



#define delt(a,b)		(a>b?a-b:b-a)



/*结构体声明*/
struct config_str{
	INT16U	T_Under;						//正常发射间隔，单位：分钟
	INT16U	T_Over;							//高温发射间隔，单位：分钟（高于报警温度）
	INT16U	T_Sample;						//温度采集间隔，单位：秒
	INT16U	Tem_Alarm;						//报警温度，单位：摄氏度
	INT16U	Tem_Diff;						//报警温差，单位：摄氏度
};

/*全局变量声明*/
extern struct config_str KGG_Config;				//存放已录入的配置信息，从EEPROM读取
extern INT8U	Device_Addr[2];						//探头ID
extern INT8U 	RFRxBuff[255];						//最多255就够，一次发送最多255字节
extern INT8U 	ReNum;								//接收到的字节数
extern INT8U 	RSSI;
extern INT8U 	Tx_Buff[255];						//存放发送的数据（4字节地址，1字节命令，2字节温度值，1字节错误参数）。一次发送最多255字节
extern INT16U  Save_Temp;							//存放采集的温度值（实际温度的10倍，数值无转码）
extern INT16U  Save_Temp_old;						//存放上次采集的温度（实际温度的10倍，数值无转码）



/*函数声明*/
void Temperature_Handle(void);
void Unit_Conversion( void );
INT16U 	Get_Delt_Temp(INT16U temp1, INT16U temp2);
INT8U Get_Temp(void);
unsigned char Send_Temp(void);
unsigned char Send_Test( unsigned char Num );
void Alarm_Reset(void);
void Alarm_Test(void);

#endif