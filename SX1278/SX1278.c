#include "Sx1278.h"

#if	test
//unsigned char	Frequency[3] = {0x6d,0x00,0x00};	//设置频率436M
//unsigned char	Frequency[3] = {0x6c,0x80,0x00};	//设置频率434M
unsigned char	Frequency[3] = {0x77,0x40,0x14};	//设置频率477M
unsigned char	SpreadingFactor = 7;  				//7-12,扩频因子小些，发送时间快
unsigned char	CodingRate = 2;        				//1-4  误码率
unsigned char	Bw_Frequency = 8;      				//6-9  8带宽250k
unsigned char	PowerValue = 14;    				//开关柜探头	发射功率14dBm
unsigned char	ReamLength = 20;    				//前导码长度--->>20
unsigned int	Timeout = 4;						//单次接收检测超时时间	4symbols
BOOL_t			Low_Datarate = Fl; 			 		//低数据优化	关

#elif BATTERY
unsigned char	Frequency[3] = {0x6c,0x80,0x00};	//设置频率434M
unsigned char	SpreadingFactor = 9;  				//7-12,扩频因子小些，发送时间快
unsigned char	CodingRate = 2;        				//1-4  误码率
unsigned char	Bw_Frequency = 7;      				//6-9  7带宽125k
unsigned char	PowerValue = 0;    					//电池探头		发射功率0dBm 2dBm?
unsigned char	ReamLength = 48;    				//前导码长度48
unsigned int	Timeout = 5;						//单次接收检测超时时间
BOOL_t			Low_Datarate = Tr; 			 		//低数据优化	开

#elif KAIGUAN
unsigned char	Frequency[3] = {0x77,0x40,0x14};	//设置频率477M
unsigned char	SpreadingFactor = 7;  				//7-12,扩频因子小些，发送时间快
unsigned char	CodingRate = 2;        				//1-4  误码率
unsigned char	Bw_Frequency = 8;      				//6-9  8带宽250k
unsigned char	PowerValue = 14;    				//开关柜探头	发射功率14dBm
unsigned char	ReamLength = 0x05;    				//前导码长度--->>5
unsigned int	Timeout = 4;						//单次接收检测超时时间	4symbols
BOOL_t			Low_Datarate = Fl; 			 		//低数据优化	关

#elif ZIQUDIAN
unsigned char	Frequency[3] = {0x6c,0x80,0x00};	//设置频率434M
unsigned char	SpreadingFactor = 9;  		 		//7-12,扩频因子小些，发送时间快
unsigned char	CodingRate = 2;        		 		//1-4  误码率
unsigned char	Bw_Frequency = 7;      		 		//6-9  7带宽125k
unsigned char	PowerValue = 15;    			 	//自取电探头	发射功率20dBm     第16个，0xcf
unsigned char	ReamLength = 48;    		 		//前导码长度48
unsigned int	Timeout = 5;				 		//单次接收检测超时时间
BOOL_t			Low_Datarate = Tr; 			 		//低数据优化	开
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
函数：void RF_Initial(void)
说明：射频模块上电并进行初始化等准备工作。
入参：无
出参：无
*****************************************************************/
void RF_Initial(void)
{
	RF_ON();													//模块上电
    SX1276Reset();												//复位SX1278
    SX1276LORA_INT();											//初始化SX1278
}
																
/*****************************************************************
函数：void RF_WriteCmd(unsigned char Cmd)
说明：写命令
入参：Cmd  命令
出参：
*****************************************************************/
void RF_WriteCmd(unsigned char Cmd)
{
	 RF_CSN_LOW( );												//片选使能
	 SPI_ExchangeByte(SPI1,Cmd|0X80);							//写数据
	 RF_CSN_HIGH( );											//片选释放
}

/*****************************************************************
函数：void RF_ReadMultiReg(unsigned char addr,unsigned char *buf,unsigned char size)
说明：向寄存器地址读数据
入参：addr  寄存器地址   buf  数据   size数据长度
出参：
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
函数：void RF_ReadMultiReg(unsigned char addr,unsigned char *buf,unsigned char size)
说明：向寄存器地址写数据
入参：addr  寄存器地址   buf  数据   size数据长度
出参：
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
函数：void SX1276Reset(void)
说明：复位SX1278
入参：
出参：
*****************************************************************/
void SX1276Reset(void)
{
  	GPIO_Init(Reset_Port, Reset_Pin, GPIO_Mode_Out_OD_HiZ_Slow);	//已测试可拉低
    Reset_L();
    Delay_1Ms(1);   //需要100us以上
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
函数：void SX1276WriteBuffer( unsigned char addr, unsigned char value )
说明：写寄存器数据
入参：addr  寄存器地址  value  写入值
出参：
*****************************************************************/
void SX1276WriteBuffer( unsigned char addr, unsigned char value )
{
    RF_CSN_LOW( );
    SPI_ExchangeByte( SPI1,addr|0x80);//D′
    SPI_ExchangeByte(SPI1, value );

    RF_CSN_HIGH( );
}
/*****************************************************************
函数：unsigned char SX1276ReadBuffer( unsigned char addr )
说明：读寄存器地址值
入参：addr  地址
出参：读取的值
*****************************************************************/
unsigned char SX1276ReadBuffer( unsigned char addr )
{
    unsigned char i = 0;
    RF_CSN_LOW( );
    SPI_ExchangeByte( SPI1, addr & 0x7f);//?á
    i = SPI_ExchangeByte( SPI1,0xFF );
    RF_CSN_HIGH( );
    return i;
}

/******************************************************************************
函数：void SX1276LoRaSetOpMode( RFMode_SET opMode )
说明：设置SX1278工作方式。通过变更RegOpMode寄存器的值，可以从任意一种模式切换到另一种模式
入参：
出参：
******************************************************************************/
void SX1276LoRaSetOpMode( RFMode_SET opMode )
{
	unsigned char opModePrev;
	opModePrev=SX1276ReadBuffer(REG_LR_OPMODE);			//读0x01模式寄存器
	opModePrev &= 0xf8;									//清零低三位
	opModePrev |= (unsigned char)opMode;				//或上形参
	SX1276WriteBuffer( REG_LR_OPMODE, opModePrev);		//重新写回去
}
/******************************************************************************
函数：void SX1276LoRaFsk( Debugging_fsk_ook opMode )
说明：设置SX11278工作模式
入参：
出参：
******************************************************************************/
void SX1276LoRaFsk( Debugging_fsk_ook opMode )
{
	unsigned char opModePrev;
	opModePrev=SX1276ReadBuffer(REG_LR_OPMODE);			//读0x01模式寄存器
	opModePrev &=0x7F;									//清零最高位
	opModePrev |= (unsigned char)opMode;				//或上形参
	SX1276WriteBuffer( REG_LR_OPMODE, opModePrev);		//重新写回去
}
/*****************************************************************************
函数：void SX1276LoRaSetRFFrequency(void)
说明：设置工作频率
入参：
出参：
******************************************************************************/
void SX1276LoRaSetRFFrequency(void)
{
	SX1276WriteBuffer( REG_LR_FRFMSB, Frequency[0]);  //0x06寄存器
	SX1276WriteBuffer( REG_LR_FRFMID, Frequency[1]);  //0x07寄存器
	SX1276WriteBuffer( REG_LR_FRFLSB, Frequency[2]);  //0x08寄存器
}

/*******************************************************************************
函数：void SX1276LoRaSetRFPower(unsigned char power )
说明：设置功率
入参：
出参：
******************************************************************************/
void SX1276LoRaSetRFPower(unsigned char power )
{
	//Set Pmax to +20dBm for PA_HP, Must turn off PA_LF or PA_HF, and set RegOcp
	//SX1276WriteBuffer( REG_LR_PACONFIG,  power_data[power] );
	//SX1276WriteBuffer( REG_LR_OCP, 0x3f);  //add by skay,20160810,写电流保护寄存器
	
//	SX1276WriteBuffer( REG_LR_PADAC, 0x87);  									//high power，开关柜探头不需要
	SX1276WriteBuffer( REG_LR_PACONFIG,  power_data[power] );
}

/*******************************************************************************
函数：void SX1276LoRaSetSpreadingFactor(unsigned char factor )
说明：设置扩频因子
入参：
出参：
******************************************************************************/
void SX1276LoRaSetSpreadingFactor(unsigned char factor )
{
	unsigned char RECVER_DAT;
	SX1276LoRaSetNbTrigPeaks( 3 ); //0x03-->SF7 to SF12
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG2);	//读0x1E寄存器
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
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG1);	//读0x1D寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT);
	// LoRaSettings.ErrorCoding = value;
}
/***************************************************************************
函数：void SX1276LoRaSetPacketCrcOn(BOOL_t enable )
说明：CRC校正
入参:
出参：
******************************************************************************/
void SX1276LoRaSetPacketCrcOn(BOOL_t enable )
{
	unsigned char RECVER_DAT;
	RECVER_DAT= SX1276ReadBuffer( REG_LR_MODEMCONFIG2);	//读0x1E寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT );
}
/********************************************************************************
函数：void SX1276LoRaSetSignalBandwidth(unsigned char bw )
说明：设置带宽
入参：
出参：
******************************************************************************/
void SX1276LoRaSetSignalBandwidth(unsigned char bw )
{
	unsigned char RECVER_DAT;
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG1); 	//读0x1D寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT );
	// LoRaSettings.SignalBw = bw;
}
/********************************************************************************
函数：void SX1276LoRaSetImplicitHeaderOn(BOOL_t enable )
说明：显隐性
入参：
出参：
******************************************************************************/
void SX1276LoRaSetImplicitHeaderOn(BOOL_t enable )
{
	unsigned char RECVER_DAT;
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG1 );	//读0x1D寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );	//计算&0xFE|enable，由于|运算，enable值会生效
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT );
}
/********************************************************************************
函数：void SX1276LoRaSetSymbTimeout(unsigned int value )
说明：接收超时时间
入参：
出参：
******************************************************************************/
void SX1276LoRaSetSymbTimeout(unsigned int value )
{
	unsigned char RECVER_DAT[2];
	RECVER_DAT[0]=SX1276ReadBuffer( REG_LR_MODEMCONFIG2 );    //读取调制解调器物理层配置2（1-0是RX 超时最高有效位）
	RECVER_DAT[1]=SX1276ReadBuffer( REG_LR_SYMBTIMEOUTLSB );  //读取接收机超时值
	RECVER_DAT[0] = ( RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
	RECVER_DAT[1] = value & 0xFF;
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
	SX1276WriteBuffer( REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}
/********************************************************************************
函数：void SX1276LoRaSetPayloadLength(unsigned char value )
说明：设置接收长度
入参：
出参：
******************************************************************************/
void SX1276LoRaSetPayloadLength(unsigned char value )
{
	SX1276WriteBuffer( REG_LR_PAYLOADLENGTH, value );  //写0x22寄存器，playload长度
}

#if 1
/********************************************************************************
函数：void SX1276LoRaSetPreamLength(unsigned int value )
说明：前导码位数
入参：
出参：
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
	RECVER_DAT=SX1276ReadBuffer( REG_LR_MODEMCONFIG3 );	//读0x26寄存器，低数据率优化
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK ) | ( enable << 3 );
	SX1276WriteBuffer( REG_LR_MODEMCONFIG3, RECVER_DAT );
}
/**************************************************************
函数：void FUN_RF_SENDPACKET(unsigned char *RF_TRAN_P,unsigned char LEN)
说明：发送数据，最多255字节
入参：
出参：
******************************************************************************/
void FUN_RF_SENDPACKET(unsigned char *RF_TRAN_P,unsigned char LEN)
{
  	SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_TXD_Value );	//可选IRQ标志屏蔽
    SX1276LoRaSetOpMode( TX_mode );								//查下功耗之类的
    SX1276WriteBuffer( REG_LR_FIFOTXBASEADDR, 0); 				//写TXFIFO基地址（0时最多255字节）（睡眠模式时，FIFO数据缓存被清空）
    SX1276WriteBuffer( REG_LR_FIFOADDRPTR, 0 ); 				//SPI interface address pointer in FIFO data buffer
    SX1276WriteBuffer( REG_LR_PAYLOADLENGTH, LEN);				//数据包长度
    RF_WriteMultiReg(0x80,RF_TRAN_P,LEN );						//写入发送数据
    SX1276LoRaSetOpMode( Transmitter_mode );					//此时发出
}

//杜颖成=====================开始
/**********************************************************************
函数：unsigned char SX1278_TX(unsigned char *buf,unsigned int time,unsigned char len)
功能：SX1278进行CAD检测，空闲时以当前配置启动发射，最多time秒；信道忙时延时约0.9S。完成后进入Sleep_mode。
入参：unsigned char *buf，发射数据存放地址；unsigned int time，发射超时时间，单位ms；unsigned char len，发送的字节数
出参：发射完成，返回1；信道忙未发射，返回0；发射超时未完成，返回0xff
***********************************************************************/
unsigned char SX1278_TX(unsigned char *buf,unsigned int time,unsigned char len)
{
	RF_CAD_RECEIVE();									//CAD配置
	if( SX1278_CAD()==0 )		//检测到信道空闲
	{
//		RF_Tx_Cfg();									//测试用，频率点为436M。目前接收板为这个频点
		FUN_RF_SENDPACKET( buf , len );					//射频发送数据，共len字节
		if( RF_Wait_PacketSent( time ) )				//等待发送完成，最多1000ms（里面无喂狗）
		{
			SX1276LoRaSetOpMode( Sleep_mode );
			return 1;									//发送完成
		}
		return 0xff;									//进行了发射但超时了
	}
	else						//本次检测到信道被占用
	{
		SX1276LoRaSetOpMode( Sleep_mode );
		WaitSleep( 5, 400 );							//延时约0.9S
		return 0;										//信道占用，未发射
	}
}

/**********************************************************************
函数：unsigned char SX1278_RX(unsigned char *buf,unsigned int time,unsigned char *rssi)
功能：SX1278以当前配置开启接收（FIFO指针0x00），最多time秒，接收完后进入Sleep_mode。
入参：unsigned char *buf，接收数据存放地址；unsigned int time，接收超时时间，单位ms
出参：接收完成，返回接收到的数据长度；超时未完成，返回0
***********************************************************************/
unsigned char SX1278_RX(unsigned char *buf,unsigned int time,unsigned char *rssi)
{
  	INT8U Len=0;

#if 0   /*从CAD模式切过来*/
	SX1276LoRaSetOpMode( Receiver_mode );				//设置为连续接收模式（默认为CAD检测到后开始接收，此时REG_LR_IRQFLAGSMASK已在CAD设置打开接收中断）
#else   /*连续接收模式*/
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);//0x11,打开接收中断
	SX1276WriteBuffer( REG_LR_OPMODE, 0x85);		    //连续接收模式
#endif
    if( RF_Wait_PacketReceive( time ) )					//等待接收成功，最多timems，单一接收模式时，完成后自动进入Stdby_mode
	{
        SX1276LoRaSetOpMode( Stdby_mode );				//连续模式时，手动进入Stdby_mode以节能
		SX1276WriteBuffer( REG_LR_FIFOADDRPTR , 0x00 );	//FIFO基地址（0时最多255字节）（睡眠模式时，FIFO数据缓存被清空）
		Len = SX1276ReadBuffer( REG_LR_NBRXBYTES ); 	//读取接收到的字节数
		*rssi = SX1276ReadBuffer( REG_LR_PKTRSSIVALUE );//读取RegPktRssiValue
		RF_ReadMultiReg( 0x00 , buf , Len );			//从sx1278读取数据     
	}
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff  );		//清零所有标志位，所有的DIOx口都会恢复低电平
    SX1276LoRaSetOpMode( Sleep_mode );				    //接收结束，进入睡眠模式（同时清空FIFO）
	return Len;									    //返回接收到的数据长度；未接收到有效数据时返回0
}

/**************************************************************
函数：unsigned char RF_Wait_PacketSent (unsigned int time)
说明：等待数据发送完成，最多time毫秒。每隔1S喂狗。
入参：time为等待的时间，单位毫秒
出参：发送完成，返回1；超时未发送完成，返回0
******************************************************************************/
unsigned char RF_Wait_PacketSent (unsigned int time)
{
    while(time--)
    {
		if( SX1276ReadBuffer( REG_LR_IRQFLAGS )&0x08 )	//读中断状态寄存器，TxDone 位，FIFO 负载发送完成中断时置位，一次写操作后自动清0
        {
            return 1;           						//发送完成，返回1
        }
        Delay_1Ms(1);
		
		if(0==time%1000)								//每隔1S喂狗
		{
			WDI();										
			IWDG_ReloadCounter();
		}
    }
    return 0;                   						//超时未发送完成，返回0
}


/************************************************************************
函数：unsigned char RF_Wait_PacketReceive (unsigned int time)
说明：等待数据接收完成，最多time毫秒。每隔1S喂狗。
入参：time为等待的时间，单位毫秒
出参：接收完成，返回1；超时未完成，返回0
*************************************************************************/
unsigned char RF_Wait_PacketReceive(unsigned int time)
{
	INT8U IRQ_Flags = 0;
    while(time--)
    {
		IRQ_Flags = SX1276ReadBuffer( REG_LR_IRQFLAGS );
        if( IRQ_Flags&0x50 )							//读中断状态寄存器，RxDone、ValidHeader 位，接收完成中断时置位，一次写操作后自动清0
        {
			if( IRQ_Flags & 0x20 )						//PayloadCrcError 位，为1则CRC核验失败
			{	
			#if rate_test || host
				//CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);		//调用过wailsleep后要初始化串口时钟
				USART1_Send_Data( "\r\nPayloadCrcError!\r\n", 20 );				//会出现地址0D0A=3338、温度5061=2057.7的数据、RSSI为-16的数据显示
			#endif
				break;
			}
            return 1;           						//接收完成，返回1
        }
        Delay_1Ms(1);
		
		if(0==time%1000)								//每隔1S喂狗
		{
			WDI();										
			IWDG_ReloadCounter();
		}
    }
    return 0;                   						//超时未接收完成，返回0
}

/************************************************************************
函数：unsigned char RF_Wait_PacketReceive (unsigned int time)
说明：单次接收，等待数据接收完成或接收超时中断，最多time毫秒（防止死循环）。每隔1S喂狗。
入参：time为等待的时间，单位毫秒
出参：接收完成，返回1；接收超时，返回0xff；限时内无中断产生，返回0
*************************************************************************/
unsigned char RF_Wait_Receive_single(unsigned int time)
{
  	INT8U IRQ_Flags = 0;
    while(time--)
    {
	  	IRQ_Flags = SX1276ReadBuffer( REG_LR_IRQFLAGS );
        if( IRQ_Flags&0x50 )							//读中断状态寄存器，RxDone、ValidHeader 位，接收完成中断时置位，一次写操作后自动清0
        {
			if( IRQ_Flags & 0x20 )						//PayloadCrcError 位，为1则CRC核验失败
			{	
			#if rate_test || host
				//CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);		//调用过wailsleep后要初始化串口时钟
				USART1_Send_Data( "\r\nPayloadCrcError!\r\n", 20 );				//会出现地址0D0A=3338、温度5061=2057.7的数据、RSSI为-16的数据显示
			#endif
				break;
			}
            return 1;           						//接收完成，返回1
        }
		else if( IRQ_Flags & 0x80 )					//读中断状态寄存器，RxTimeout 位，接收超时中断时置位，一次写操作后自动清0
		{
			return 0xff;           					//接收超时，返回0xff
		}
        Delay_1Ms(1);
		
		if(0==time%1000)								//每隔1S喂狗
		{
			WDI();										
			IWDG_ReloadCounter();
		}
    }
    return 0;                   						//限时内无中断产生，返回0
}

/************************************************************************
函数：unsigned char SX1278_CAD(void)
说明：SX1278进行CAD，判断是否有信道活动。时间在几个毫秒，不用喂狗。
入参：无
出参：有信道活动返回1；无信道活动返回0；超时未完成CAD返回0xff
*************************************************************************/
unsigned char SX1278_CAD(void)
{
//	static INT8U	n=0;
//	static INT8U	value[100]=0;
	INT8U	Overtime=25;			//CAD检测超时 次数
	INT8U	value=0;

	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff  );	//清零所有标志位，所有的DIOx口都会恢复低电平	不清除会导致检测不到
	RF_CAD_RECEIVE();				//CAD配置
	while(Overtime--)  			//接收超时 次数
	{
		value=SX1276ReadBuffer( REG_LR_IRQFLAGS );	//读寄存器
		if( value==0 )				//CAD没检测没完成（完成会有0x40）
		{
//		  	value[n]=SX1276ReadBuffer( REG_LR_IRQFLAGS );
//			n++;
			;						//NOP，继续读，直到超时
		}
		else						//CAD完成
		{
//		  	n=0;
			if( value&=0x01 )		//信道忙
			{
				return 1;			//结束本次CAD，返回信道忙
			}
			else					//信道空闲
			{
				return 0;			//结束本次CAD，返回信道空闲
			}
		}
	}
	return 0xff;					//超时未完成CAD返回0xff
}
//杜颖成=====================结束



void RF_Tx_Cfg(void)	//目前仅用于重新设置频点
{
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, 0x87 ); 	//设置接收同步头中断    、发送中断也打开
	//射频载波频率
	SX1276WriteBuffer( REG_LR_FRFMSB, 0x6D );
	SX1276WriteBuffer( REG_LR_FRFMID, 0x00 );
	SX1276WriteBuffer( REG_LR_FRFLSB, 0x00 );
}

void RF_Rx_Cfg(void)
{
   SX1276WriteBuffer( REG_LR_IRQFLAGSMASK,0x8f); //设置接收同步头中断    、发送中断也打开

   SX1276LoRaSetRFFrequency();  //频率设置434M
}

/**************************************************************
函数：unsigned char SX1276LORA_INT(void)
说明：初始化SX1278
入参：无
出参：成功返回1；失败返回0
******************************************************************************/
unsigned char SX1276LORA_INT(void)
{
    unsigned char Version;

    SX1276LoRaSetOpMode( Sleep_mode );         				//设置睡眠模式
    SX1276LoRaFsk( LORA_mode );	               				//设置扩频模式(LORA)
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, 0x87 );			//设置接收同步头中断、发送中断也打开
    SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0x4c );			//DIO0-DIO3引脚映射
    SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0xc1 );			//DIO4-DIO5引脚映射
	SX1276LoRaSetPreamLength( ReamLength );					//前导码长度 	2symbols
    SX1276LoRaSetRFFrequency();  							//频率设置		Frequency[3]
    SX1276LoRaSetRFPower( PowerValue );  					//功率设置		20dBm
    SX1276LoRaSetSpreadingFactor( SpreadingFactor );	 	//扩频因子SF	7
    SX1276LoRaSetErrorCoding( CodingRate );		 			//有效数据比DR	2
    SX1276LoRaSetPacketCrcOn( Tr );			 				//CRC校验		打开
    SX1276LoRaSetSignalBandwidth( Bw_Frequency );	 		//扩频带宽BW  	125KHz
    SX1276LoRaSetImplicitHeaderOn( Fl );		 			//同步头		隐性
    SX1276LoRaSetPayloadLength( 0xff );						//负载长度		255字节
    SX1276LoRaSetSymbTimeout( Timeout );					//接收超时时间	4symbols	指检测时间，在此时间窗口内检测前导码。0x04是调制解调器获取前导码锁的最短时间（在连续Rx模式下，产生超时中断不会进入待机模式  例程上是0x3FF，为最大值，共9~0位）
    SX1276LoRaSetMobileNode( Low_Datarate ); 			 	//低数据优化

    Version=SX1276ReadBuffer( REG_LR_VERSION );  			//版本号应是0x12
    if(Version==0x12)
    {
        SX1276LoRaSetOpMode( Sleep_mode );         			//睡眠模式  Sleep_mode   Stdby_mode
        return 1;
    }
    return 0;
}

/************************************************************************
函数：unsigned char SX1278_SoftWOR(void)
说明：进入接收模式等待前导码（老方式，现用SX1278_CAD(void)）
入参：无
出参：检测到前导码时返回1；否则0
*************************************************************************/
unsigned char SX1278_SoftWOR(void)	//没在用
{
#if 1	//CAD检测并接收，平均电流530uA，无丢包	1S转发无法完成，丢包严重   原因：发送接收切换给的时间太短
	unsigned short CS_Time=25;

 	RF_CAD_RECEIVE();								//开启CAD
  	while(CS_Time--)  								//接收time*0.1 ms
    {
	  	if( SX1276ReadBuffer( REG_LR_IRQFLAGS )&0x01 )	//读中断状态寄存器，CadDetected 位0x01	CadDone 0x04
		{
			return 1;								//检测到前导码
		}
		Delay_100us(1);	//++++++++++++++++++++++++++++++++这里应该可以优化，防止CAD到后等太久收不到
    }
  	return 0;

#else	//接收模式检测并接收，平均电流735uA  1S转发约770uA（好像有问题）
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value );	//可选IRQ标志屏蔽
    SX1276LoRaSetOpMode( Receiver_mode );	//切换到接收模式

    if(RF_SignCS(25))  //接收time*0.1 ms（见下）
    {
          return 1;//检测到前导码
    }

//    SX1276LoRaSetOpMode( Sleep_mode );

    return 0;
#endif
}

void RF_RECEIVE ( RFMode_SET opMode )				//接收模式配置
{
//	SX1276LoRaSetOpMode(Stdby_mode );		Sleep_mode可读写，去掉模式切换，可省下20uA
	SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);  //0x11,打开接收中断
//	SX1276WriteBuffer(REG_LR_HOPPERIOD,	PACKET_MIAX_Value );//0x24寄存器，设置频率跳变周期为最大
//	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 ); //DIO引脚映射设置，按默认		（这两够打开功耗增加了10多uA）
//	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
	SX1276LoRaSetOpMode( receive_single );  //设置为单次接收模式
}

void RF_CAD_RECEIVE (void)  		//CAD 信道活动检测配置
{
//	SX1276LoRaSetOpMode( Stdby_mode );		Sleep_mode可读写，去掉模式切换，可省下20uA
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, IRQN_CAD_Value & IRQN_RXD_Value );	//可选IRQ标志屏蔽  除CadDetectedMask、CadDoneMask、PayloadCrcError、RxDone外全屏蔽  IRQN_CAD_Value IRQN_RXD_Value
//	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );			//0X80 DIO0映射到CadDone	（这两句注释掉时电流是降了20uA，但12mA持续了很久）
//	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );			//
	SX1276LoRaSetOpMode( CAD_mode );
}

void RF_SEELP(void)
{
//	SX1276LoRaSetOpMode( Stdby_mode );		Sleep_mode可读写，去掉模式切换，可省下20uA
	SX1276WriteBuffer( REG_LR_IRQFLAGSMASK, IRQN_SEELP_Value ); 	//允许所有中断
//	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
//	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
	SX1276LoRaSetOpMode( Sleep_mode );
}

//**************下面是中断里面处理的代码*******************************
unsigned char recv[10];
unsigned char RF_REC_RLEN_i;
unsigned char SX1278_Interupt(void)
{//RegIrqFlags的八位依次为： RxTimeout, RxDone, PayloadCrcError,ValidHeader, TxDone, CadDone, FhssChangeChannel, CadDetected.
//    unsigned char Rssi;
//    Rssi=SX1276ReadBuffer(REG_LR_RSSIVALUE);
    RF_EX0_STATUS=SX1276ReadBuffer( REG_LR_IRQFLAGS );	//读取0x12，中断标志寄存器
    if((RF_EX0_STATUS&0x40)==0x40)  //接收完成
    {
        CRC_Value=SX1276ReadBuffer( REG_LR_MODEMCONFIG2 );
        if((CRC_Value&0x04)==0x04) 	//是否CRC校验完成
        {
            SX1276WriteBuffer (REG_LR_FIFOADDRPTR,0x00);			//设置SPI接口在FIFO缓冲区中的地址指针
            SX1278_RLEN = SX1276ReadBuffer(REG_LR_NBRXBYTES);		//读取最后一个包的字节数
            RF_ReadMultiReg(0x00,recv,SX1278_RLEN);
        }

        SX1276LoRaSetOpMode( Stdby_mode );
        SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);		//打开接收中断
        SX1276WriteBuffer(REG_LR_HOPPERIOD,    PACKET_MIAX_Value);	//0x24寄存器，设置频率跳变周期为最大
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );				//端口映射恢复为默认
        SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
        SX1276LoRaSetOpMode( Receiver_mode );
        return SX1278_RLEN;
    }
    else if((RF_EX0_STATUS&0x08)==0x08)	//发射完成
    {//发射完成后，应设置为接收模式，才能接收到信号
        SX1276LoRaSetOpMode( Stdby_mode );
        SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);		//打开接收中断
        SX1276WriteBuffer(REG_LR_HOPPERIOD,   PACKET_MIAX_Value );	//0x24寄存器，设置频率跳变周期为最大
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
        SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
        SX1276LoRaSetOpMode( Receiver_mode );
    }
    else if((RF_EX0_STATUS&0x04)==0x04)	//cad完成
    {
        if((RF_EX0_STATUS&0x01)==0x01)
        {
            //表示CAD 检测到扩频信号 模块进入了接收状态来接收数据
            SX1276LoRaSetOpMode( Stdby_mode );
            SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);		//打开接收中断
            SX1276WriteBuffer(REG_LR_HOPPERIOD,   PACKET_MIAX_Value );	//0x24寄存器，设置频率跳变周期为最大
            SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X02 );				//PayloadCrcError映射到DIO3
            SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
            SX1276LoRaSetOpMode( Receiver_mode );
        }
        else
        {
            // 没检测到
            SX1276LoRaSetOpMode( Stdby_mode );
            SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,	IRQN_SEELP_Value);	//允许所有中断
            SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
            SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
            SX1276LoRaSetOpMode( Sleep_mode );
        }
    }
	else	//其他中断
	{
		SX1276LoRaSetOpMode( Stdby_mode );
		SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);	        //打开接收中断
		SX1276WriteBuffer(REG_LR_HOPPERIOD,   PACKET_MIAX_Value );      //0x24寄存器，设置频率跳变周期为最大
		SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X02 );                  //PayloadCrcError映射到DIO3
		SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00 );
		SX1276LoRaSetOpMode( Receiver_mode );

	}
    SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff  );	//清零所有标志位，所有的DIOx口都会恢复低电平（注意！要写0xff，写0x00无用）
	return 0;	//不是接收中断，返回0
}
