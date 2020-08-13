#include "Sx1278.h"

#if	test
//unsigned char	Frequency[3] = {0x6d,0x00,0x00};	//����Ƶ��436M
//unsigned char	Frequency[3] = {0x6c,0x80,0x00};	//����Ƶ��434M
unsigned char	Frequency[3] = {0x77,0x40,0x14};	//����Ƶ��477M
unsigned char	SpreadingFactor = 7;  				//7-12,��Ƶ����СЩ������ʱ���
unsigned char	CodingRate = 2;        				//1-4  ������
unsigned char	Bw_Frequency = 8;      				//6-9  8����250k
unsigned char	PowerValue = 14;    				//���ع�̽ͷ	���书��14dBm
unsigned char	ReamLength = 20;    				//ǰ���볤��--->>20
unsigned int	Timeout = 4;						//���ν��ռ�ⳬʱʱ��	4symbols
BOOL_t			Low_Datarate = Fl; 			 		//�������Ż�	��

#elif BATTERY
unsigned char	Frequency[3] = {0x6c,0x80,0x00};	//����Ƶ��434M
unsigned char	SpreadingFactor = 9;  				//7-12,��Ƶ����СЩ������ʱ���
unsigned char	CodingRate = 2;        				//1-4  ������
unsigned char	Bw_Frequency = 7;      				//6-9  7����125k
unsigned char	PowerValue = 0;    					//���̽ͷ		���书��0dBm 2dBm?
unsigned char	ReamLength = 48;    				//ǰ���볤��48
unsigned int	Timeout = 5;						//���ν��ռ�ⳬʱʱ��
BOOL_t			Low_Datarate = Tr; 			 		//�������Ż�	��

#elif KAIGUAN
unsigned char	Frequency[3] = {0x77,0x40,0x14};	//����Ƶ��477M
unsigned char	SpreadingFactor = 7;  				//7-12,��Ƶ����СЩ������ʱ���
unsigned char	CodingRate = 2;        				//1-4  ������
unsigned char	Bw_Frequency = 8;      				//6-9  8����250k
unsigned char	PowerValue = 14;    				//���ع�̽ͷ	���书��14dBm
unsigned char	ReamLength = 0x05;    				//ǰ���볤��--->>5
unsigned int	Timeout = 4;						//���ν��ռ�ⳬʱʱ��	4symbols
BOOL_t			Low_Datarate = Fl; 			 		//�������Ż�	��

#elif ZIQUDIAN
unsigned char	Frequency[3] = {0x6c,0x80,0x00};	//����Ƶ��434M
unsigned char	SpreadingFactor = 9;  		 		//7-12,��Ƶ����СЩ������ʱ���
unsigned char	CodingRate = 2;        		 		//1-4  ������
unsigned char	Bw_Frequency = 7;      		 		//6-9  7����125k
unsigned char	PowerValue = 15;    			 	//��ȡ��̽ͷ	���书��20dBm     ��16����0xcf
unsigned char	ReamLength = 48;    		 		//ǰ���볤��48
unsigned int	Timeout = 5;				 		//���ν��ռ�ⳬʱʱ��
BOOL_t			Low_Datarate = Tr; 			 		//�������Ż�	��
#endif



/*
RegPaconfig 0x09
bit			name 		discription
 7		 PaSelect		0--> RFO pin, Output power is limited to +14dBm
						1--> PA_BOOST pin, Output power is limited to +20dBm

6~4		 MaxPower		Select max output power:Pmax=10.8+0.6*MaxPower [dBm]

3~0		 OutputPower	if PaSelect=0, Pout=Pmax-(15-OutputPower)
						if PaSelect=1, Pout=17-(15-OutputPower)
*/
unsigned char   power_data[16]={0X80,0X81,0X82,0X83,0X84,0X85,0X86,0X87,0X88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0xcf};//0x8f

unsigned char   RF_EX0_STATUS;

unsigned char   CRC_Value;

unsigned char   SX1278_RLEN;
extern void Delay_1Ms(unsigned short time);



/*****************************************************************
������void RF_Initial(void)
˵������Ƶģ���ϵ粢���г�ʼ����׼��������
��Σ���
���Σ���
*****************************************************************/
void RF_Initial(void)
{
	RF_ON();													//ģ���ϵ�
    SX1276Reset();												//��λSX1278
    SX1276LORA_INT();											//��ʼ��SX1278
}
																
/*****************************************************************
������void RF_WriteCmd(unsigned char Cmd)
˵����д����
��Σ�Cmd  ����
���Σ�
*****************************************************************/
void RF_WriteCmd(unsigned char Cmd)
{
	 RF_CSN_LOW( );												//Ƭѡʹ��
	 SPI_ExchangeByte(SPI1,Cmd|0X80);							//д����
	 RF_CSN_HIGH( );											//Ƭѡ�ͷ�
}

/*****************************************************************
������void RF_ReadMultiReg(unsigned char addr,unsigned char *buf,unsigned char size)
˵������Ĵ�����ַ������
��Σ�addr  �Ĵ�����ַ   buf  ����   size���ݳ���
���Σ�
*****************************************************************/
void RF_ReadMultiReg(unsigned char addr,unsigned char *buf,unsigned char size)
{
    unsigned char i;
    RF_CSN_LOW( );
    SPI_ExchangeByte( SPI1, addr & 0x7f );

    for( i = 0; i < size; i ++ )
    {
        *( buf + i ) = SPI_ExchangeByte( SPI1, 0xFF );
    }
    RF_CSN_HIGH( );
}
/*****************************************************************
������void RF_ReadMultiReg(unsigned char addr,unsigned char *buf,unsigned char size)
˵������Ĵ�����ַд����
��Σ�addr  �Ĵ�����ַ   buf  ����   size���ݳ���
���Σ�
*****************************************************************/
void RF_WriteMultiReg( unsigned char addr, unsigned char *buff, unsigned char size )
{
    unsigned char i;
    RF_CSN_LOW( );
    SPI_ExchangeByte(  SPI1,addr );

    for( i = 0; i < size; i ++ )
    {
       SPI_ExchangeByte( SPI1, *( buff + i ) );
    }
    RF_CSN_HIGH( );
}

/*****************************************************************
������void SX1276Reset(void)
˵������λSX1278
��Σ�
���Σ�
*****************************************************************/
void SX1276Reset(void)
{
  	GPIO_Init(Reset_Port, Reset_Pin, GPIO_Mode_Out_OD_HiZ_Slow);	//�Ѳ��Կ�����
    Reset_L();
    Delay_1Ms(1);   //��Ҫ100us����
    Reset_H();
    Delay_1Ms(5);
}


#if 0
void FSK_SEND_PACKET(void)
{
	SX1276LoRaSetOpMode( Stdby_mode );
	SX1276WriteBuffer(REG_LR_DIOMAPPING1,0x01);
	SX1276WriteBuffer(REG_LR_DIOMAPPING2,0x20);
}
#endif

/*****************************************************************
������void SX1276WriteBuffer( unsigned char addr, unsigned char value )
˵����д�Ĵ�������
��Σ�addr  �Ĵ�����ַ  value  д��ֵ
���Σ�
*****************************************************************/
void SX1276WriteBuffer( unsigned char addr, unsigned char value )
{
    RF_CSN_LOW( );
    SPI_ExchangeByte( SPI1,addr|0x80);//D��
    SPI_ExchangeByte(SPI1, value );

    RF_CSN_HIGH( );
}
/*****************************************************************
������unsigned char SX1276ReadBuffer( unsigned char addr )
˵�������Ĵ�����ֵַ
��Σ�addr  ��ַ
���Σ���ȡ��ֵ
*****************************************************************/
unsigned char SX1276ReadBuffer( unsigned char addr )
{
    unsigned char i = 0;
    RF_CSN_LOW( );
    SPI_ExchangeByte( SPI1, addr & 0x7f);//?��
    i = SPI_ExchangeByte( SPI1,0xFF );
    RF_CSN_HIGH( );
    return i;
}

/******************************************************************************
������void SX1276LoRaSetOpMode( RFMode_SET opMode )
˵��������SX1278������ʽ��ͨ�����RegOpMode�Ĵ�����ֵ�����Դ�����һ��ģʽ�л�����һ��ģʽ
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetOpMode( RFMode_SET opMode )
{
	unsigned char opModePrev;
	opModePrev=SX1276ReadBuffer(REG_LR_OPMODE);			//��0x01ģʽ�Ĵ���
	opModePrev &= 0xf8;									//�������λ
	opModePrev |= (unsigned char)opMode;				//�����β�
	SX1276WriteBuffer( REG_LR_OPMODE, opModePrev);		//����д��ȥ
}
/******************************************************************************
������void SX1276LoRaFsk( Debugging_fsk_ook opMode )
˵��������SX11278����ģʽ
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaFsk( Debugging_fsk_ook opMode )
{
	unsigned char opModePrev;
	opModePrev=SX1276ReadBuffer(REG_LR_OPMODE);			//��0x01ģʽ�Ĵ���
	opModePrev &=0x7F;									//�������λ
	opModePrev |= (unsigned char)opMode;				//�����β�
	SX1276WriteBuffer( REG_LR_OPMODE, opModePrev);		//����д��ȥ
}
/*****************************************************************************
������void SX1276LoRaSetRFFrequency(void)
˵�������ù���Ƶ��
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetRFFrequency(void)
{
	SX1276WriteBuffer( REG_LR_FRFMSB, Frequency[0]);  //0x06�Ĵ���
	SX1276WriteBuffer( REG_LR_FRFMID, Frequency[1]);  //0x07�Ĵ���
	SX1276WriteBuffer( REG_LR_FRFLSB, Frequency[2]);  //0x08�Ĵ���
}

/*******************************************************************************
������void SX1276LoRaSetRFPower(unsigned char power )
˵�������ù���
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetRFPower(unsigned char power )
{
	//Set Pmax to +20dBm for PA_HP, Must turn off PA_LF or PA_HF, and set RegOcp
	//SX1276WriteBuffer( REG_LR_PACONFIG,  power_data[power] );
	//SX1276WriteBuffer( REG_LR_OCP, 0x3f);  //add by skay,20160810,д���������Ĵ���
	
//	SX1276WriteBuffer( REG_LR_PADAC, 0x87);  									//high power�����ع�̽ͷ����Ҫ
	SX1276WriteBuffer( REG_LR_PACONFIG,  power_data[power] );
}

/*******************************************************************************
������void SX1276LoRaSetSpreadingFactor(unsigned char factor )
˵����������Ƶ����
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetSpreadingFactor(unsigned char factor )
{
	unsigned char RECVER_DAT;
	SX1276LoRaSetNbTrigPeaks( 3 ); //0x03-->SF7 to SF12
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG2);	//��0x1E�Ĵ���
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT );
}

void SX1276LoRaSetNbTrigPeaks(unsigned char value )
{
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer( 0x31);  //Read RegDetectOptimize,
	RECVER_DAT = ( RECVER_DAT & 0xF8 ) | value; //process;
	SX1276WriteBuffer( 0x31, RECVER_DAT );  //write back;
}

void SX1276LoRaSetErrorCoding(unsigned char value )
{
	unsigned char RECVER_DAT;
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG1);	//��0x1D�Ĵ���
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT);
	// LoRaSettings.ErrorCoding = value;
}
/***************************************************************************
������void SX1276LoRaSetPacketCrcOn(BOOL_t enable )
˵����CRCУ��
���:
���Σ�
******************************************************************************/
void SX1276LoRaSetPacketCrcOn(BOOL_t enable )
{
	unsigned char RECVER_DAT;
	RECVER_DAT= SX1276ReadBuffer( REG_LR_MODEMCONFIG2);	//��0x1E�Ĵ���
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT );
}
/********************************************************************************
������void SX1276LoRaSetSignalBandwidth(unsigned char bw )
˵�������ô���
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetSignalBandwidth(unsigned char bw )
{
	unsigned char RECVER_DAT;
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG1); 	//��0x1D�Ĵ���
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT );
	// LoRaSettings.SignalBw = bw;
}
/********************************************************************************
������void SX1276LoRaSetImplicitHeaderOn(BOOL_t enable )
˵����������
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetImplicitHeaderOn(BOOL_t enable )
{
	unsigned char RECVER_DAT;
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG1 );	//��0x1D�Ĵ���
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );	//����&0xFE|enable������|���㣬enableֵ����Ч
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT );
}
/********************************************************************************
������void SX1276LoRaSetSymbTimeout(unsigned int value )
˵�������ճ�ʱʱ��
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetSymbTimeout(unsigned int value )
{
	unsigned char RECVER_DAT[2];
	RECVER_DAT[0]=SX1276ReadBuffer( REG_LR_MODEMCONFIG2 );    //��ȡ���ƽ�������������2��1-0��RX ��ʱ�����Чλ��
	RECVER_DAT[1]=SX1276ReadBuffer( REG_LR_SYMBTIMEOUTLSB );  //��ȡ���ջ���ʱֵ
	RECVER_DAT[0] = ( RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
	RECVER_DAT[1] = value & 0xFF;
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
	SX1276WriteBuffer( REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}
/********************************************************************************
������void SX1276LoRaSetPayloadLength(unsigned char value )
˵�������ý��ճ���
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetPayloadLength(unsigned char value )
{
	SX1276WriteBuffer( REG_LR_PAYLOADLENGTH, value );  //д0x22�Ĵ�����playload����
}

#if 1
/********************************************************************************
������void SX1276LoRaSetPreamLength(unsigned int value )
˵����ǰ����λ��
��Σ�
���Σ�
******************************************************************************/
void SX1276LoRaSetPreamLength(unsigned int value )
{
	unsigned char a[2];
	a[0]=(value&0xff00)>>8;
	a[1]=value&0xff;
	SX1276WriteBuffer( REG_LR_PREAMBLEMSB, a[0] );
	SX1276WriteBuffer( REG_LR_PREAMBLELSB, a[1] );
}
#endif

void SX1276LoRaSetMobileNode(BOOL_t enable )
{
	unsigned char RECVER_DAT;
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG3 );	//��0x26�Ĵ��������������Ż�
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK ) | ( enable << 3 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG3, RECVER_DAT );
}
/**************************************************************
������void FUN_RF_SENDPACKET(unsigned char *RF_TRAN_P,unsigned char LEN)
˵�����������ݣ����255�ֽ�
��Σ�
���Σ�
******************************************************************************/
void FUN_RF_SENDPACKET(unsigned char *RF_TRAN_P,unsigned char LEN)
{
  	SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_TXD_Value );	//��ѡIRQ��־����
    SX1276LoRaSetOpMode( TX_mode );								//���¹���֮���
    SX1276WriteBuffer( REG_LR_FIFOTXBASEADDR, 0); 				//дTXFIFO����ַ��0ʱ���255�ֽڣ���˯��ģʽʱ��FIFO���ݻ��汻��գ�
    SX1276WriteBuffer( REG_LR_FIFOADDRPTR, 0 ); 				//SPI interface address pointer in FIFO data buffer
    SX1276WriteBuffer( REG_LR_PAYLOADLENGTH, LEN);				//���ݰ�����
    RF_WriteMultiReg(0x80,RF_TRAN_P,LEN );						//д�뷢������
    SX1276LoRaSetOpMode( Transmitter_mode );					//��ʱ����
}

//��ӱ��=====================��ʼ
/**********************************************************************
������unsigned char SX1278_TX(unsigned char *buf,unsigned int time,unsigned char len)
���ܣ�SX1278����CAD��⣬����ʱ�Ե�ǰ�����������䣬���time�룻�ŵ�æʱ��ʱԼ0.9S����ɺ����Sleep_mode��
��Σ�unsigned char *buf���������ݴ�ŵ�ַ��unsigned int time�����䳬ʱʱ�䣬��λms��unsigned char len�����͵��ֽ���
���Σ�������ɣ�����1���ŵ�æδ���䣬����0�����䳬ʱδ��ɣ�����0xff
***********************************************************************/
unsigned char SX1278_TX(unsigned char *buf,unsigned int time,unsigned char len)
{
	RF_CAD_RECEIVE();									//CAD����
	if( SX1278_CAD()==0 )		//��⵽�ŵ�����
	{
//		RF_Tx_Cfg();									//�����ã�Ƶ�ʵ�Ϊ436M��Ŀǰ���հ�Ϊ���Ƶ��
		FUN_RF_SENDPACKET( buf , len );					//��Ƶ�������ݣ���len�ֽ�
		if( RF_Wait_PacketSent( time ) )				//�ȴ�������ɣ����1000ms��������ι����
		{
			SX1276LoRaSetOpMode( Sleep_mode );
			return 1;									//�������
		}
		return 0xff;									//�����˷��䵫��ʱ��
	}
	else						//���μ�⵽�ŵ���ռ��
	{
		SX1276LoRaSetOpMode( Sleep_mode );
		WaitSleep( 5, 400 );							//��ʱԼ0.9S
		return 0;										//�ŵ�ռ�ã�δ����
	}
}

/**********************************************************************
������unsigned char SX1278_RX(unsigned char *buf,unsigned int time,unsigned char *rssi)
���ܣ�SX1278�Ե�ǰ���ÿ������գ�FIFOָ��0x00�������time�룬����������Sleep_mode��
��Σ�unsigned char *buf���������ݴ�ŵ�ַ��unsigned int time�����ճ�ʱʱ�䣬��λms
���Σ�������ɣ����ؽ��յ������ݳ��ȣ���ʱδ��ɣ�����0
***********************************************************************/
unsigned char SX1278_RX(unsigned char *buf,unsigned int time,unsigned char *rssi)
{
  	INT8U Len=0;

#if 0   /*��CADģʽ�й���*/
	SX1276LoRaSetOpMode( Receiver_mode );				//����Ϊ��������ģʽ��Ĭ��ΪCAD��⵽��ʼ���գ���ʱREG_LR_IRQFLAGSMASK����CAD���ô򿪽����жϣ�
#else   /*��������ģʽ*/
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);//0x11,�򿪽����ж�
	SX1276WriteBuffer( REG_LR_OPMODE, 0x85);		    //��������ģʽ
#endif
    if( RF_Wait_PacketReceive( time ) )					//�ȴ����ճɹ������timems����һ����ģʽʱ����ɺ��Զ�����Stdby_mode
	{
        SX1276LoRaSetOpMode( Stdby_mode );				//����ģʽʱ���ֶ�����Stdby_mode�Խ���
		SX1276WriteBuffer( REG_LR_FIFOADDRPTR , 0x00 );	//FIFO����ַ��0ʱ���255�ֽڣ���˯��ģʽʱ��FIFO���ݻ��汻��գ�
		Len = SX1276ReadBuffer( REG_LR_NBRXBYTES ); 	//��ȡ���յ����ֽ���
		*rssi = SX1276ReadBuffer( REG_LR_PKTRSSIVALUE );//��ȡRegPktRssiValue
		RF_ReadMultiReg( 0x00 , buf , Len );			//��sx1278��ȡ����     
	}
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff  );		//�������б�־λ�����е�DIOx�ڶ���ָ��͵�ƽ
    SX1276LoRaSetOpMode( Sleep_mode );				    //���ս���������˯��ģʽ��ͬʱ���FIFO��
	return Len;									    //���ؽ��յ������ݳ��ȣ�δ���յ���Ч����ʱ����0
}

/**************************************************************
������unsigned char RF_Wait_PacketSent (unsigned int time)
˵�����ȴ����ݷ�����ɣ����time���롣ÿ��1Sι����
��Σ�timeΪ�ȴ���ʱ�䣬��λ����
���Σ�������ɣ�����1����ʱδ������ɣ�����0
******************************************************************************/
unsigned char RF_Wait_PacketSent (unsigned int time)
{
    while(time--)
    {
		if( SX1276ReadBuffer( REG_LR_IRQFLAGS )&0x08 )	//���ж�״̬�Ĵ�����TxDone λ��FIFO ���ط�������ж�ʱ��λ��һ��д�������Զ���0
        {
            return 1;           						//������ɣ�����1
        }
        Delay_1Ms(1);
		
		if(0==time%1000)								//ÿ��1Sι��
		{
			WDI();										
			IWDG_ReloadCounter();
		}
    }
    return 0;                   						//��ʱδ������ɣ�����0
}


/************************************************************************
������unsigned char RF_Wait_PacketReceive (unsigned int time)
˵�����ȴ����ݽ�����ɣ����time���롣ÿ��1Sι����
��Σ�timeΪ�ȴ���ʱ�䣬��λ����
���Σ�������ɣ�����1����ʱδ��ɣ�����0
*************************************************************************/
unsigned char RF_Wait_PacketReceive(unsigned int time)
{
	INT8U IRQ_Flags = 0;
    while(time--)
    {
		IRQ_Flags = SX1276ReadBuffer( REG_LR_IRQFLAGS );
        if( IRQ_Flags&0x50 )							//���ж�״̬�Ĵ�����RxDone��ValidHeader λ����������ж�ʱ��λ��һ��д�������Զ���0
        {
			if( IRQ_Flags & 0x20 )						//PayloadCrcError λ��Ϊ1��CRC����ʧ��
			{	
			#if rate_test || host
				//CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);		//���ù�wailsleep��Ҫ��ʼ������ʱ��
				USART1_Send_Data( "\r\nPayloadCrcError!\r\n", 20 );				//����ֵ�ַ0D0A=3338���¶�5061=2057.7�����ݡ�RSSIΪ-16��������ʾ
			#endif
				break;
			}
            return 1;           						//������ɣ�����1
        }
        Delay_1Ms(1);
		
		if(0==time%1000)								//ÿ��1Sι��
		{
			WDI();										
			IWDG_ReloadCounter();
		}
    }
    return 0;                   						//��ʱδ������ɣ�����0
}

/************************************************************************
������unsigned char RF_Wait_PacketReceive (unsigned int time)
˵�������ν��գ��ȴ����ݽ�����ɻ���ճ�ʱ�жϣ����time���루��ֹ��ѭ������ÿ��1Sι����
��Σ�timeΪ�ȴ���ʱ�䣬��λ����
���Σ�������ɣ�����1�����ճ�ʱ������0xff����ʱ�����жϲ���������0
*************************************************************************/
unsigned char RF_Wait_Receive_single(unsigned int time)
{
  	INT8U IRQ_Flags = 0;
    while(time--)
    {
	  	IRQ_Flags = SX1276ReadBuffer( REG_LR_IRQFLAGS );
        if( IRQ_Flags&0x50 )							//���ж�״̬�Ĵ�����RxDone��ValidHeader λ����������ж�ʱ��λ��һ��д�������Զ���0
        {
			if( IRQ_Flags & 0x20 )						//PayloadCrcError λ��Ϊ1��CRC����ʧ��
			{	
			#if rate_test || host
				//CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);		//���ù�wailsleep��Ҫ��ʼ������ʱ��
				USART1_Send_Data( "\r\nPayloadCrcError!\r\n", 20 );				//����ֵ�ַ0D0A=3338���¶�5061=2057.7�����ݡ�RSSIΪ-16��������ʾ
			#endif
				break;
			}
            return 1;           						//������ɣ�����1
        }
		else if( IRQ_Flags & 0x80 )					//���ж�״̬�Ĵ�����RxTimeout λ�����ճ�ʱ�ж�ʱ��λ��һ��д�������Զ���0
		{
			return 0xff;           					//���ճ�ʱ������0xff
		}
        Delay_1Ms(1);
		
		if(0==time%1000)								//ÿ��1Sι��
		{
			WDI();										
			IWDG_ReloadCounter();
		}
    }
    return 0;                   						//��ʱ�����жϲ���������0
}

/************************************************************************
������unsigned char SX1278_CAD(void)
˵����SX1278����CAD���ж��Ƿ����ŵ����ʱ���ڼ������룬����ι����
��Σ���
���Σ����ŵ������1�����ŵ������0����ʱδ���CAD����0xff
*************************************************************************/
unsigned char SX1278_CAD(void)
{
//	static INT8U	n=0;
//	static INT8U	value[100]=0;
	INT8U	Overtime=25;			//CAD��ⳬʱ ����
	INT8U	value=0;

	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff  );	//�������б�־λ�����е�DIOx�ڶ���ָ��͵�ƽ	������ᵼ�¼�ⲻ��
	RF_CAD_RECEIVE();				//CAD����
	while(Overtime--)  			//���ճ�ʱ ����
	{
		value=SX1276ReadBuffer( REG_LR_IRQFLAGS );	//���Ĵ���
		if( value==0 )				//CADû���û��ɣ���ɻ���0x40��
		{
//		  	value[n]=SX1276ReadBuffer( REG_LR_IRQFLAGS );
//			n++;
			;						//NOP����������ֱ����ʱ
		}
		else						//CAD���
		{
//		  	n=0;
			if( value&=0x01 )		//�ŵ�æ
			{
				return 1;			//��������CAD�������ŵ�æ
			}
			else					//�ŵ�����
			{
				return 0;			//��������CAD�������ŵ�����
			}
		}
	}
	return 0xff;					//��ʱδ���CAD����0xff
}
//��ӱ��=====================����



void RF_Tx_Cfg(void)	//Ŀǰ��������������Ƶ��
{
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, 0x87 ); 	//���ý���ͬ��ͷ�ж�    �������ж�Ҳ��
	//��Ƶ�ز�Ƶ��
	SX1276WriteBuffer( REG_LR_FRFMSB, 0x6D );
	SX1276WriteBuffer( REG_LR_FRFMID, 0x00 );
	SX1276WriteBuffer( REG_LR_FRFLSB, 0x00 );
}

void RF_Rx_Cfg(void)
{
   SX1276WriteBuffer( REG_LR_IRQFLAGSMASK,0x8f); //���ý���ͬ��ͷ�ж�    �������ж�Ҳ��

   SX1276LoRaSetRFFrequency();  //Ƶ������434M
}

/**************************************************************
������unsigned char SX1276LORA_INT(void)
˵������ʼ��SX1278
��Σ���
���Σ��ɹ�����1��ʧ�ܷ���0
******************************************************************************/
unsigned char SX1276LORA_INT(void)
{
    unsigned char Version;

    SX1276LoRaSetOpMode( Sleep_mode );         				//����˯��ģʽ
    SX1276LoRaFsk( LORA_mode );	               				//������Ƶģʽ(LORA)
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, 0x87 );			//���ý���ͬ��ͷ�жϡ������ж�Ҳ��
    SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0x4c );			//DIO0-DIO3����ӳ��
    SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0xc1 );			//DIO4-DIO5����ӳ��
	SX1276LoRaSetPreamLength( ReamLength );					//ǰ���볤�� 	2symbols
    SX1276LoRaSetRFFrequency();  							//Ƶ������		Frequency[3]
    SX1276LoRaSetRFPower( PowerValue );  					//��������		20dBm
    SX1276LoRaSetSpreadingFactor( SpreadingFactor );	 	//��Ƶ����SF	7
    SX1276LoRaSetErrorCoding( CodingRate );		 			//��Ч���ݱ�DR	2
    SX1276LoRaSetPacketCrcOn( Tr );			 				//CRCУ��		��
    SX1276LoRaSetSignalBandwidth( Bw_Frequency );	 		//��Ƶ����BW  	125KHz
    SX1276LoRaSetImplicitHeaderOn( Fl );		 			//ͬ��ͷ		����
    SX1276LoRaSetPayloadLength( 0xff );						//���س���		255�ֽ�
    SX1276LoRaSetSymbTimeout( Timeout );					//���ճ�ʱʱ��	4symbols	ָ���ʱ�䣬�ڴ�ʱ�䴰���ڼ��ǰ���롣0x04�ǵ��ƽ������ȡǰ�����������ʱ�䣨������Rxģʽ�£�������ʱ�жϲ���������ģʽ  ��������0x3FF��Ϊ���ֵ����9~0λ��
    SX1276LoRaSetMobileNode( Low_Datarate ); 			 	//�������Ż�

    Version=SX1276ReadBuffer( REG_LR_VERSION );  			//�汾��Ӧ��0x12
    if(Version==0x12)
    {
        SX1276LoRaSetOpMode( Sleep_mode );         			//˯��ģʽ  Sleep_mode   Stdby_mode
        return 1;
    }
    return 0;
}

/************************************************************************
������unsigned char SX1278_SoftWOR(void)
˵�����������ģʽ�ȴ�ǰ���루�Ϸ�ʽ������SX1278_CAD(void)��
��Σ���
���Σ���⵽ǰ����ʱ����1������0
*************************************************************************/
unsigned char SX1278_SoftWOR(void)	//û����
{
#if 1	//CAD��Ⲣ���գ�ƽ������530uA���޶���	1Sת���޷���ɣ���������   ԭ�򣺷��ͽ����л�����ʱ��̫��
	unsigned short CS_Time=25;

 	RF_CAD_RECEIVE();								//����CAD
  	while(CS_Time--)  								//����time*0.1 ms
    {
	  	if( SX1276ReadBuffer( REG_LR_IRQFLAGS )&0x01 )	//���ж�״̬�Ĵ�����CadDetected λ0x01	CadDone 0x04
		{
			return 1;								//��⵽ǰ����
		}
		Delay_100us(1);	//++++++++++++++++++++++++++++++++����Ӧ�ÿ����Ż�����ֹCAD�����̫���ղ���
    }
  	return 0;

#else	//����ģʽ��Ⲣ���գ�ƽ������735uA  1Sת��Լ770uA�����������⣩
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value );	//��ѡIRQ��־����
    SX1276LoRaSetOpMode( Receiver_mode );	//�л�������ģʽ

    if(RF_SignCS(25))  //����time*0.1 ms�����£�
    {
          return 1;//��⵽ǰ����
    }

//    SX1276LoRaSetOpMode( Sleep_mode );

    return 0;
#endif
}

void RF_RECEIVE ( RFMode_SET opMode )				//����ģʽ����
{
//	SX1276LoRaSetOpMode(Stdby_mode );		Sleep_mode�ɶ�д��ȥ��ģʽ�л�����ʡ��20uA
	SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);  //0x11,�򿪽����ж�
//	SX1276WriteBuffer(REG_LR_HOPPERIOD,	PACKET_MIAX_Value );//0x24�Ĵ���������Ƶ����������Ϊ���
//	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 ); //DIO����ӳ�����ã���Ĭ��		���������򿪹���������10��uA��
//	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
	SX1276LoRaSetOpMode( receive_single );  //����Ϊ���ν���ģʽ
}

void RF_CAD_RECEIVE (void)  		//CAD �ŵ���������
{
//	SX1276LoRaSetOpMode( Stdby_mode );		Sleep_mode�ɶ�д��ȥ��ģʽ�л�����ʡ��20uA
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, IRQN_CAD_Value & IRQN_RXD_Value );	//��ѡIRQ��־����  ��CadDetectedMask��CadDoneMask��PayloadCrcError��RxDone��ȫ����  IRQN_CAD_Value IRQN_RXD_Value
//	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );			//0X80 DIO0ӳ�䵽CadDone	��������ע�͵�ʱ�����ǽ���20uA����12mA�����˺ܾã�
//	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );			//
	SX1276LoRaSetOpMode( CAD_mode );
}

void RF_SEELP(void)
{
//	SX1276LoRaSetOpMode( Stdby_mode );		Sleep_mode�ɶ�д��ȥ��ģʽ�л�����ʡ��20uA
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, IRQN_SEELP_Value ); 	//���������ж�
//	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
//	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
	SX1276LoRaSetOpMode( Sleep_mode );
}

//**************�������ж����洦��Ĵ���*******************************
unsigned char recv[10];
unsigned char RF_REC_RLEN_i;
unsigned char SX1278_Interupt(void)
{//RegIrqFlags�İ�λ����Ϊ�� RxTimeout, RxDone, PayloadCrcError,ValidHeader, TxDone, CadDone, FhssChangeChannel, CadDetected.
//    unsigned char Rssi;
//    Rssi=SX1276ReadBuffer(REG_LR_RSSIVALUE);
    RF_EX0_STATUS=SX1276ReadBuffer( REG_LR_IRQFLAGS );	//��ȡ0x12���жϱ�־�Ĵ���
    if((RF_EX0_STATUS&0x40)==0x40)  //�������
    {
        CRC_Value=SX1276ReadBuffer( REG_LR_MODEMCONFIG2 );
        if((CRC_Value&0x04)==0x04) 	//�Ƿ�CRCУ�����
        {
            SX1276WriteBuffer (REG_LR_FIFOADDRPTR,0x00);			//����SPI�ӿ���FIFO�������еĵ�ַָ��
            SX1278_RLEN = SX1276ReadBuffer(REG_LR_NBRXBYTES);		//��ȡ���һ�������ֽ���
            RF_ReadMultiReg(0x00,recv,SX1278_RLEN);
        }

        SX1276LoRaSetOpMode( Stdby_mode );
        SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);		//�򿪽����ж�
        SX1276WriteBuffer(REG_LR_HOPPERIOD,    PACKET_MIAX_Value);	//0x24�Ĵ���������Ƶ����������Ϊ���
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );				//�˿�ӳ��ָ�ΪĬ��
        SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
        SX1276LoRaSetOpMode( Receiver_mode );
        return SX1278_RLEN;
    }
    else if((RF_EX0_STATUS&0x08)==0x08)	//�������
    {//������ɺ�Ӧ����Ϊ����ģʽ�����ܽ��յ��ź�
        SX1276LoRaSetOpMode( Stdby_mode );
        SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);		//�򿪽����ж�
        SX1276WriteBuffer(REG_LR_HOPPERIOD,   PACKET_MIAX_Value );	//0x24�Ĵ���������Ƶ����������Ϊ���
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
        SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
        SX1276LoRaSetOpMode( Receiver_mode );
    }
    else if((RF_EX0_STATUS&0x04)==0x04)	//cad���
    {
        if((RF_EX0_STATUS&0x01)==0x01)
        {
            //��ʾCAD ��⵽��Ƶ�ź� ģ������˽���״̬����������
            SX1276LoRaSetOpMode( Stdby_mode );
            SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);		//�򿪽����ж�
            SX1276WriteBuffer(REG_LR_HOPPERIOD,   PACKET_MIAX_Value );	//0x24�Ĵ���������Ƶ����������Ϊ���
            SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X02 );				//PayloadCrcErrorӳ�䵽DIO3
            SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
            SX1276LoRaSetOpMode( Receiver_mode );
        }
        else
        {
            // û��⵽
            SX1276LoRaSetOpMode( Stdby_mode );
            SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,	IRQN_SEELP_Value);	//���������ж�
            SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
            SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
            SX1276LoRaSetOpMode( Sleep_mode );
        }
    }
	else	//�����ж�
	{
		SX1276LoRaSetOpMode( Stdby_mode );
		SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);	        //�򿪽����ж�
		SX1276WriteBuffer(REG_LR_HOPPERIOD,   PACKET_MIAX_Value );      //0x24�Ĵ���������Ƶ����������Ϊ���
		SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X02 );                  //PayloadCrcErrorӳ�䵽DIO3
		SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
		SX1276LoRaSetOpMode( Receiver_mode );

	}
    SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff  );	//�������б�־λ�����е�DIOx�ڶ���ָ��͵�ƽ��ע�⣡Ҫд0xff��д0x00���ã�
	return 0;	//���ǽ����жϣ�����0
}
