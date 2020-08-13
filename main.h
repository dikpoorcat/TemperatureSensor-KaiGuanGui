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



//�ŵ�main.c����
#define test            0					//����ʱѡ1����main.c��Ч��
#define ZIQUDIAN		0					//��ȡ��̽ͷѡ1�����书��Ϊ20dBm���Ѽ���Ԥ����
#define BATTERY        	0					//���̽ͷѡ1�����书��Ϊ2dBm���Ѽ���Ԥ����
#define KAIGUAN         01					//���ع�̽ͷѡ1�����书��Ϊ8dBm���Ѽ���Ԥ����

#define DS18B20         0
#define PT1000          01


#define Addr            123					//����ǰ��Ӹ� 0 ��ʾ 8 ���ƣ���˲�����09�ȱ�ʾ
#define ADDER1          Addr>>8
#define ADDER2          Addr&0xff



#define delt(a,b)		(a>b?a-b:b-a)



/*�ṹ������*/
struct config_str{
	INT16U	T_Under;						//��������������λ������
	INT16U	T_Over;							//���·���������λ�����ӣ����ڱ����¶ȣ�
	INT16U	T_Sample;						//�¶Ȳɼ��������λ����
	INT16U	Tem_Alarm;						//�����¶ȣ���λ�����϶�
	INT16U	Tem_Diff;						//�����²��λ�����϶�
};

/*ȫ�ֱ�������*/
extern struct config_str KGG_Config;				//�����¼���������Ϣ����EEPROM��ȡ
extern INT8U	Device_Addr[2];						//̽ͷID
extern INT8U 	RFRxBuff[255];						//���255�͹���һ�η������255�ֽ�
extern INT8U 	ReNum;								//���յ����ֽ���
extern INT8U 	RSSI;
extern INT8U 	Tx_Buff[255];						//��ŷ��͵����ݣ�4�ֽڵ�ַ��1�ֽ����2�ֽ��¶�ֵ��1�ֽڴ����������һ�η������255�ֽ�
extern INT16U  Save_Temp;							//��Ųɼ����¶�ֵ��ʵ���¶ȵ�10������ֵ��ת�룩
extern INT16U  Save_Temp_old;						//����ϴβɼ����¶ȣ�ʵ���¶ȵ�10������ֵ��ת�룩



/*��������*/
void Temperature_Handle(void);
void Unit_Conversion( void );
INT16U 	Get_Delt_Temp(INT16U temp1, INT16U temp2);
INT8U Get_Temp(void);
unsigned char Send_Temp(void);
unsigned char Send_Test( unsigned char Num );
void Alarm_Reset(void);
void Alarm_Test(void);

#endif