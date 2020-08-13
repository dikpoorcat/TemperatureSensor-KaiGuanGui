/*===========================================================================
��˾�����ݷ��ϵ����������޹�˾
���ߣ���ӱ�ɣ�����հ
���ܣ�SX1278̽ͷ����LoRa�շ����ܼ��¶Ȳɼ���
���ڣ�2016.11.02
�޶���2017.02.22	�޸�Ϊ�ѻ�������ʹ��(EEPROM)
�޶���2017.03.08	���ӷ�������жϼ��
�޶���2017.06.29	PT1000��������ʱ�������¶ȴ���ͬʱ֧��DS18B20
�޶���2017.09.26	9�ִ��������ȷ��Ϊ4001~4005������3150������-500��
�޶���2017.10.23	����DS18B20���¶ȼĴ���ǰ�ȴ�ʱ�䲻���ᵼ���¶��쳣��������150ms
�޶���2018.01.09	SX1278��������������ģʽ�µ͹���ʵ�֣��ⲿ���Ź�WDI���Ŵ�PC1���ĵ�PA2��Ŀǰ��ȡ��������͹��ģ�����+ι��+Sleep_mode��113uA��ͷ�ļ�����main.c�������ֺ����������Ƶ�bsp.c
�޶���2018.04.19	�������ó���Tempadjust���ֵ�
�޶���2018.04.25	�����ڵ��̽ͷ����Ӧ��Ӳ����ɾ�����ⲿ���Ź���������������������Խ��͹��ġ�DS18B20������������Ϊ1��
�޶���2018.06.27	���̽ͷ���Ӹ��±������ܣ����¶ȸ���75���϶�ʱ1min/���ϱ���ȥ���˵��̽ͷ�¶Ȳɼ����30����ʱ���Ҹ�λ�����̷���������������
�޶���2018.11.07	���̽ͷ���²���ʱ���ֻ���1min/�Σ�ԭ���Ǹ���ǰ��0XF800���ж�Ϊ>750���£�����Save_Temp&0x07ff��DS18B20�������ʱҲ��ֹ1min���͡�ȫ���޸ģ�����125��ʱ�ϱ�4017��
�޶���2018.11.23	PT1000_getTempValue����ȡ����ȷ�¶�ʱ����1��������롢�����̡�����ʧ�ܵȶ�����0��
�޶���2018.12.06	DS18B20�ɼ���125��85��ʱ���²�����DS18B20������ʱ�ʵ��ӳ���
�޶���2019.10.30	���뿪�ع�궨�壬�����ڿ��ع����������
�޶���2019.11.08	���ع���ȡ����������ɡ�����Ƶ����δ������
�޶���2019.11.14	PT1000�������̵��������·�Χȷ��Ϊ-60~290�棬��У����������հ��
�޶���2019.11.18	PT1000�����Ż���ʹ��Medium�Ż��ȼ��ɳɹ���¼�����ع�lora�����޸ġ�
�޶���2019.11.27	���ع�lora�����޸ģ��¶Ȳɼ�����Ƶ����֮���������ֹ��ѹ��������DS18B20�ɼ���85�Ⱥ�125��ʱǿ���޸�Ϊ������루�˺���հ���Բ��ø��ˣ���
�޶���2019.12.04	�޸���RF_Wait_PacketSent\RF_Wait_PacketReceive\RF_Wait_Receive_single��ÿ��1��ι����
�޶���2019.12.06	��PayloadCrcError�жϷŵ����������ײ㣻������SX1278����������հ���ͬ����
�޶���2019.12.23	���ع��Ĵ����޵�ѹ��⿪�ص�·��ʵ��5A������������͹��Ĵ�����ɣ��ѻ���¼����ͨ����������FLASH�����·�Χ�޸�Ϊ-60~290�棨FLASH���ޣ���config_str�ṹ���޸�ΪINT16U��
�޶���2019.12.23	���ӵ�ѹ���̶�3.3
===========================================================================*/
#include "main.h"

/*ȫ�ֱ�������*/
struct config_str KGG_Config={				//���ع�Ĭ�����á������¼���������Ϣ����EEPROM��ȡ
	.T_Under = 5,							//��������������λ������
	.T_Over = 1,							//���·���������λ�����ӣ����ڱ����¶ȣ�
	.T_Sample = 60,							//�¶Ȳɼ��������λ����
	.Tem_Alarm = 75,						//�����¶ȣ���λ�����϶ȣ��������ó�60���£�����-60��ʱ���󱨣�
	.Tem_Diff = 2							//�����²��λ�����϶�
};
INT8U	Device_Addr[2]={ADDER1,ADDER2};		//̽ͷID
INT8U 	RFRxBuff[255]={0};					//���255�͹���һ�η������255�ֽ�
INT8U 	ReNum=0;							//���յ����ֽ���
INT8U 	RSSI=0;
INT8U 	Tx_Buff[255];						//��ŷ��͵����ݣ�4�ֽڵ�ַ��1�ֽ����2�ֽ��¶�ֵ��1�ֽڴ����������һ�η������255�ֽ�
INT16U  Save_Temp=0;						//��Ųɼ����¶�ֵ��ʵ���¶ȵ�10������ֵ��ת�룩
INT16U  Save_Temp_old=0;					//����ϴβɼ����¶ȣ�ʵ���¶ȵ�10������ֵ��ת�룩
INT16U	T_sleep=0;

INT8U Vbat=33;

/************************************
main����
************************************/
int main(void)
{
	SClK_Initial();             					//��ʼ��ϵͳʱ�ӣ�1M
	GPIO_Initial_StartUp();
	WaitSleep_StartUp(60,2300);						//Խ��1.7V�ĵ�ѹ�󣬲���ִ�к��棨EEP��������ǰ��

	GPIO_Initial();		
	SPI_Initial();		
	RF_Initial();
	IWDG_Init();                					//�ڲ����Ź���ʼ�����ڲ����Ź�����1.724s��λ
#if !test
	E2P_Read(0,Device_Addr,2);    					//��EEPROM��ע�⣺������¼ʱEEPROM��д�룬��ַΪ0��
	E2P_Read(2,(INT8U*)&KGG_Config.T_Under,10);    	//��EEPROM��ע�⣺������¼ʱEEPROM��д�룬���ò���ΪĬ��ֵ��
	Unit_Conversion();								//����T_sleep����KGG_Config�У��Է���Ϊ��λ��ֵת��Ϊ�룬�¶ȵ�λ����10����
#endif
	Feed_Watchdog();								//���Ź�ι��

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
		Temperature_Handle();						//�ɼ��¶Ȳ�����
		Send_Temp();
		WaitSleep(T_sleep,2300);					//��ֹ��������̲ɼ���Ҳ��������ʱ�ã�Temperature_Handle()��������T_Sample/2��
	}
#endif
}













/**********************************************************************
������void Temperature_Handle(void)
���ܣ��ɼ��¶Ȳ����ݲ�ͬ�¶Ƚ�����ʱ����ʱ�������¶Ȳɼ��쳣�����²��
	  ʱ������
��Σ���
���Σ���
***********************************************************************/
void Temperature_Handle(void)
{
	INT8U   collect=1;									//�¶Ȳɼ��������������������
	INT8U   Sta=0;
	INT16U	Delt_Temp=0;
	
	while(1)
	{
		Sta=Get_Temp();									//�ɼ��¶ȣ����ع���ȥ����ʱ�� �Ȳ��º���ʱ���������ٳ�磩
		WaitSleep(T_sleep,2300);						//�¶Ȳɼ����ΪT_Sample	�ɼ�����break���ڷ���󲹻�T_Sample/2��ʱ
		Delt_Temp = Get_Delt_Temp(Save_Temp,Save_Temp_old);		//���βɼ����¶Ȳ�
		Save_Temp_old=Save_Temp;						//����Save_Temp_old

		if(Sta && (Delt_Temp<KGG_Config.Tem_Diff))		//�¶Ȳɼ������������βɼ��²�С��2���϶�
		{
			if((Save_Temp&0x07ff)<KGG_Config.Tem_Alarm)	//�¶�����ʱ������Ƶ�ʵ�
			{
				if(collect*KGG_Config.T_Sample>=KGG_Config.T_Under) break;		//�жϷ���������ʱ����
			}
			else										//���������¶ȣ����ӷ���Ƶ��
			{
				if(collect*KGG_Config.T_Sample>=KGG_Config.T_Over) break;		//�жϷ���������ʱ����
			}
			collect++;									//��ʱ��������ѭ��
		}
		else break;	  								//�¶Ȳɼ��쳣�����²��
		
		WaitSleep(T_sleep,2300);						//����ʱ��Ϊ���ε�ԭ�򣺷�ֹ�ɼ������䶯�������ţ����µ�ѹ��������
	}
}

/**********************************************************************
������void Temperature_Handle(void)
���ܣ�����T_sleep����KGG_Config�У��Է���Ϊ��λ��ֵת��Ϊ�룬�¶ȵ�λ����10����
��Σ���
���Σ���
***********************************************************************/
void Unit_Conversion( void )
{
	T_sleep = KGG_Config.T_Sample/2;					//ÿ������ʱ��Ϊ�¶Ȳɼ�ʱ���һ��
	KGG_Config.T_Under *= 60;							//��������������λ������->��
	KGG_Config.T_Over *= 60;							//���·���������λ������->�루���ڱ����¶ȣ�
	/*KGG_Config.T_Sample��λΪ�벻�ñ�*/
	KGG_Config.Tem_Alarm *= 10;							//�����¶ȣ���λ�����϶�->10�����������ó�60���£�����-60��ʱ���󱨣�
	KGG_Config.Tem_Diff *= 10;							//�����²��λ�����϶�->10��
}

/**********************************************************************
������INT16U 	Get_Delt_Temp(INT16U temp1, INT16U temp2)
���ܣ������¶Ȳ�ѿ���DS18B20�ĸ�����ʾ��
��Σ�INT16U temp1���¶�1��INT16U temp2���¶�2��
���Σ��¶Ȳ�
***********************************************************************/
INT16U 	Get_Delt_Temp(INT16U temp1, INT16U temp2)
{
	if((temp1^temp2)&0xE000)							//����3λ��ͬ����һ������һ������
	{
		return (temp1 + temp2)&0x07ff;					//��ŵ��¶Ȳ�
	}
	return delt(Save_Temp,Save_Temp_old);				//ͬ�ŵ��¶Ȳ�
}

/**********************************************************************
������void Get_Temp(void)
���ܣ��¶Ȳɼ�����ѡPT1000��DS18B20
��Σ���
���Σ�����ֵStatuse������ȷ�򷵻�1�����򷵻�0.
***********************************************************************/
INT8U Get_Temp(void)
{
#if PT1000  //PT1000��������
    INT8U Statuse=0;

    Statuse=PT1000_getTempValue(&Save_Temp);

//    //�¶Ȳɼ�״̬,��Ҫ��S���ң���ι��
//	Feed_Watchdog();									//���Ź�ι��

    #if ( (!BATTERY) && (!test) && (!KAIGUAN))			//���̽ͷ��BATTERY=1������ԣ�test=1���򿪹ع�KAIGUAN=1��ʱ�������˾�
    WaitSleep( 30, 2300 );								//��ʱ30��
    #endif

    return Statuse;
#endif
#if DS18B20 //DS18B20��������
    INT8U Statuse=0;

	#if BATTERY //���̽ͷ ֻ����һ��
	Statuse=DS18B20_getTempValue( &Save_Temp,1);		//�¶Ȳɼ�״̬,��Ҫ1S���ң���ι��
	Feed_Watchdog();
	if( (Save_Temp==850)||(Save_Temp==1250) )			//���ɼ���85��/125�ȣ����������쳣ֵ���ٴβɼ�
	{
		Statuse=DS18B20_getTempValue( &Save_Temp,1);	//�¶Ȳɼ�״̬,��Ҫ1S���ң���ι��
		Feed_Watchdog();
	}

	#else
    INT8U i,m,n;
    INT16U TEMP[3];
    INT16U temp;

	for(i=0;i<3;i++)
	{
		Statuse=DS18B20_getTempValue( &Save_Temp,1);															//�ɼ������Σ��гɹ���ʧ��ʱ  Statuse��ô����
		//�¶Ȳɼ�״̬,��Ҫ1S���ң���ι��
		Feed_Watchdog();								//���Ź�ι��
		TEMP[i]=Save_Temp;
		#if ( (!BATTERY) && (!test) && (!KAIGUAN))		//���̽ͷ��BATTERY=1������ԣ�test=1���򿪹ع�KAIGUAN=1��ʱ�������˾�
		WaitSleep( 10, 2300 );               			//4*i*i+3  4S,8S,20S
		#endif
	}
	//�Ӵ�С����
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
    Save_Temp=  TEMP[1];								//ȡ��λ��
	if( Save_Temp==850 ) Save_Temp = 4017;				//���ɼ���85�ȣ���дΪ�������4017����0x0FB1
	if( Save_Temp==1250 ) Save_Temp = 4018;				//���ɼ���125�ȣ���дΪ�������4018����0x0FB2
	#endif

    GPIO_ResetBits(PORT_DQ,PIN_DQ);						//��λ
    DS18B20_OFF();
    return Statuse;
#endif
}

/**********************************************************************
������unsigned char Send_Temp(void)
���ܣ�������֡��������ŵ��Ƿ�æ����æ�ͷ������ݣ��������3�Ρ�
��Σ���
���Σ�������ɷ���1�����ͳ�ʱ����0xff���ŵ�æ����0��
***********************************************************************/
unsigned char Send_Temp(void)
{
	INT8U 	Count=5;							//CAD����ͻʱ�ط�����
	INT8U	Err=0;

	Tx_Buff[0]=Device_Addr[0];
	Tx_Buff[1]=Device_Addr[1];
	Tx_Buff[2]=Save_Temp>>8;
	Tx_Buff[3]=Save_Temp&0xff;
/*****************************************************�޸�**********************************************************************/
	Tx_Buff[4]=Vbat;
/*****************************************************�޸�**********************************************************************/
	while(Count--)
	{
		SPI_Initial();      					//��ʼ��SPI
		Feed_Watchdog();						//���Ź�ι��
/*****************************************************�޸�**********************************************************************/
		Err=SX1278_TX( Tx_Buff , 1000 , 5 );	//LoRa�������ݣ����ȴ�1000ms
/*****************************************************�޸�**********************************************************************/
		if( Err )								//Err��0����ʾ�ѽ��з���
		{
			return Err;
		}
	}
	SX1276LoRaSetOpMode( Sleep_mode );
	return 0;									//���μ���ŵ���æ
}

/**********************************************************************
������unsigned char Send_Test( unsigned char Num )
���ܣ�������֡��������ŵ��Ƿ�æ����æ�ͷ������ݣ��������3�Ρ�
��Σ���
���Σ�������ɷ���1�����ͳ�ʱ����0xff���ŵ�æ����0��
***********************************************************************/
unsigned char Send_Test( unsigned char Num )
{
	INT8U 	Count=3;							//CAD����ͻʱ�ط�����
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
		SPI_Initial();      					//��ʼ��SPI
		Feed_Watchdog();						//���Ź�ι��

		if(Num<4) Num=4;
		Err=SX1278_TX( Tx_Buff , 3000 ,Num );	//LoRa�������ݣ����ȴ�3000ms
		if( Err )								//Err��0����ʾ�ѽ��з���
		{
			return Err;
		}
	}
	SX1276LoRaSetOpMode( Sleep_mode );
	return 0;									//���μ���ŵ���æ
}

/**********************************************************************
������void Alarm_Reset(void)
���ܣ���MCU��λʱ�ɴӵ����仯�п�����
��Σ���
���Σ���
***********************************************************************/
void Alarm_Reset(void)
{
#if 0
  	int i;
	for(i=0;i<2;i++)
	{
	  //�����
		GPIO_Init(GPIOA,0XFF,GPIO_Mode_In_FL_IT);//
		GPIO_Init(GPIOB,0XFF,GPIO_Mode_In_FL_IT);//
		GPIO_Init(GPIOC,0XFF,GPIO_Mode_In_FL_IT);//RF/WDI����Ҫ�ߵ�ƽ
//		GPIO_Init(GPIOD,0XFF,GPIO_Mode_In_FL_IT);//COM���ž�����ȥ����
		Delay_10Ms(70);
		Feed_Watchdog();						//���Ź�ι��
	  //С����
		GPIO_Init(GPIOA,0XFF,GPIO_Mode_Out_OD_Low_Slow);//
		GPIO_Init(GPIOB,0XFF,GPIO_Mode_Out_OD_Low_Slow);//
		GPIO_Init(GPIOC,0XFF,GPIO_Mode_Out_OD_Low_Slow);//RF/WDI����Ҫ�ߵ�ƽ
//		GPIO_Init(GPIOD,0XFF,GPIO_Mode_Out_OD_Low_Slow);//COM���ž�����ȥ����
		Delay_10Ms(70);
		Feed_Watchdog();						//���Ź�ι��
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
������void Alarm_Test(void)
���ܣ��ɴӵ�ѹ�仯�п�����
��Σ���
���Σ���
***********************************************************************/
void Alarm_Test(void)
{
	INT8U	t = 0xFF;
	
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);		//	_|_ 	���裬��ѹ����
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(20);
	
//	Delay_10Ms(50);
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);		//	_|_		���裬��ѹ����
	while(t--) Delay_100us(20);
	GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);
	while(t--) Delay_100us(20);
}
