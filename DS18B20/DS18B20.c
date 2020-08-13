/*===========================================================================
��˾ �����ݷ��ϵ����������޹�˾
���� ������ΰ
���ܣ�DS18B20�¶Ȳɼ����ܺ���ʵ��
���ڣ�2015.7.16
�޶���2017.12.07 ���Ӵ������	��
===========================================================================*/
#include "bsp.h"



/*===========================================================================
���� ��void Delay_10us(uint16_t x)
���� ��10US��ʱ
���� ��uint16_t x
��� ��
============================================================================*/
void Delay_10us(uint16_t x)
{
    while(x--);
}
/*===========================================================================
* ���� ��DS18B20_Initial()                                                  *
* ���� ����ʼ��DS18B20��״̬
* ���� ��ʧ��ʱ����1���ɹ�����0�������⣩
============================================================================*/
INT8U DS18B20_Initial(void)
{
     INT16U CiShu;

    GPIO_Init(PORT_CTL, PIN_CTL, GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_Out_PP_Low_Slow);

    Delay_10us(1);
    DQ_L() ;//    GPIO_ResetBits(PORT_DQ,PIN_DQ);//����DQ����λ
    Delay_10us(80);//�ȴ�����������������λ
    DQ_H();//�ͷ�  ���ڲ����������룬���DQ�ߵ͵�ƽ
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
* ���� ��uint8_t Read_Byte(void)                                            *
* ���� ����ȡ������һ���ֽڵ�����                                           *
============================================================================*/
uint8_t DS18B20_ReadByte(void)//��һ���ֽ�
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
        if(PORT_DQ->IDR & PIN_DQ)//�ж��Ƿ�Ϊ�ߵ�ƽ
        {
            data|=0x80;
        }
        Delay_10us(2);
    }
    return data;
}
/*===========================================================================
* ���� ��Write_Byte(uint8_t dat)                                            *
* ���� ����ȡ������һ���ֽڵ�����                                           *
============================================================================*/
void DS18B20_WriteByte(uint8_t dat)
{
    uint8_t temp,i=0;
    DQ_H();//�ͷ�

#if 0
    while(1)
    {
        Delay_10us(1);
        DQ_L();
        Delay_10us(1);
        DQ_H();//�ͷ�
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
        DQ_H();//�ͷ�
        dat>>=1;
    }
    DQ_H();
}
/*===========================================================================
������uint8_t DS18B20_getTempValue(unsigned short *TempBuff,uint8_t TBLen)                                        *
���ܣ���ȡ�¶ȴ���ȥ18B20������
��Σ�   TempBuff:�¶ȴ�ŵ� λ�ã�
         TBLen    TempBuff�ĳ���
         ���˵����ÿ�ζ�ȡ�����ݶ������TempBuff[0]
                   ǰ���������������ƶ�
���Σ� ����1ʱ����ʾ��ȷ�������̡���ʼ��ʧ�ܵȶ�����0��
============================================================================*/
INT8U DS18B20_getTempValue(INT16U *TempBuff,INT8U TBLen)
{
    INT16U TempH=0,TempL=0;
    INT8U Over;

    if(DS18B20_Initial())							//����1ʱ��ʼ��ʧ�ܣ��ϱ�4016 0x0FB0
    {
        TempBuff[0]=4016;
        return 0;
    }

    DS18B20_WriteByte(0xcc);						//���������кŵĲ���
    DS18B20_WriteByte(0x44);						//�����¶�ת��
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_In_FL_No_IT);//�л�Ϊ�������룬��ֹ�ⲿ�������ģ�

    //��ʱ750Ms�ȴ�ת����� ʵ�ʸ�1.1S
	Feed_Watchdog();								//���Ź�ι��
    Delay_10us(55000);
	Feed_Watchdog();								//���Ź�ι��
    Delay_10us(55000);
	Feed_Watchdog();								//���Ź�ι��

    DS18B20_Initial();
    DS18B20_WriteByte(0xcc);						//���������к�
    DS18B20_WriteByte(0xbe);						//����д���������¶ȼĴ���
    if(PORT_DQ->IDR & PIN_DQ)						//�ж��Ƿ�Ϊ�ߵ�ƽ
       Over=1;
    Delay_10us(20000);								//150ms(���¶ȼĴ���ǰ�ȴ�ʱ�䣬ԭΪ100ms��������̽ͷ�ȴ�ʱ�䲻�����ᵼ���¶��쳣����������200ms)
    TempL=DS18B20_ReadByte();						//�����¶ȵĵ�λ
    TempH=DS18B20_ReadByte();						//�����¶ȵĸ�λ

    DS18B20_OFF();                      			//��TCL��Դ
    GPIO_Init(PORT_DQ, PIN_DQ, GPIO_Mode_In_FL_No_IT);//�л�Ϊ�������룬��ֹ�ⲿ�������ģ�

	//TempBuff[0]=(uint16_t)((TempH*256+TempL)*0.625);
	TempBuff[0]=((TempH<<8)+TempL);
	if((TempBuff[0]&0xf800)==0xf800)				//����
	{
		TempBuff[0]=(~TempBuff[0]+1);
		TempBuff[0]=TempBuff[0]*5;
		TempBuff[0]=TempBuff[0]>>3;  				//������������Ҫ*0.625  = 5/8
		if(TempBuff[0]>550)
		{
			TempBuff[0]=550;
			Over=0;									//����0����������쳣
		}
		TempBuff[0]|=0xf800;
    }
    else    //�Ǹ���
    {
		TempBuff[0]=TempBuff[0]*5;
		TempBuff[0]=TempBuff[0]>>3;
		if(TempBuff[0]>1250)
		{
			TempBuff[0]=4017;						//DS18B20������ʱ��4017 0x0FB1
			Over=0;									//����0����������쳣
		}
    }
    return Over;
}