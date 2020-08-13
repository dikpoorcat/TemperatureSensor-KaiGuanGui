#ifndef __SX1278__H__
#define __SX1278__H__

/*头文件*/
#include "mytypedef.h"
#include "stdarg.h"
#include "stm8l15x.h"
#include "stm8l15x_gpio.h"
#include "main.h"
#include "bsp.h"


/*宏定义*/
						//以下寄存器配置为LORA模式下
#define REG_LR_FIFO                                  0x00    	//FIFO读/写访问
 // Common settings
#define REG_LR_OPMODE                                0x01 		//运行模式&LoRaTM/FSK 选择
#define REG_LR_BANDSETTING                           0x04     	//LORA下未使用
#define REG_LR_FRFMSB                                0x06      	//射频载波频率，最高有效位
#define REG_LR_FRFMID                                0x07		//射频载波频率，中间位
#define REG_LR_FRFLSB                                0x08 		//射频载波频率，最低有效位
 // Tx settings
#define REG_LR_PACONFIG                              0x09     	//PA 选择和输出功率控制
#define REG_LR_PARAMP                                0x0A     	//PA 斜升/斜降时间和低相噪PLL 的控制
#define REG_LR_OCP                                   0x0B 		//过流保护控制
 // Rx settings
#define REG_LR_LNA                                   0x0C 		//LNA 设置
 // LoRa registers
#define REG_LR_FIFOADDRPTR                           0x0D		//FIFO SPI 指针
#define REG_LR_FIFOTXBASEADDR                        0x0E    	//起始Tx 数据
#define REG_LR_FIFORXBASEADDR                        0x0F 		//起始Rx 数据
#define REG_LR_FIFORXCURRENTADDR                     0x10 		//最后接收数据包的起始地址
#define REG_LR_IRQFLAGSMASK                          0x11 		//可选IRQ 标志屏蔽
#define REG_LR_IRQFLAGS                              0x12 		//IRQ 标志  中断状态寄存器
#define REG_LR_NBRXBYTES                             0x13 		//接收到的字节数
#define REG_LR_RXHEADERCNTVALUEMSB                   0x14 		//接收到的有效报头数
#define REG_LR_RXHEADERCNTVALUELSB                   0x15 		//接收到的有效报头数
#define REG_LR_RXPACKETCNTVALUEMSB                   0x16 		//接收到的有效数据包数
#define REG_LR_RXPACKETCNTVALUELSB                   0x17		//接收到的有效数据包数
#define REG_LR_MODEMSTAT                             0x18		//LoRaTM 调制解调器状态
#define REG_LR_PKTSNRVALUE                           0x19		//最后一个数据包的信噪比估值
#define REG_LR_PKTRSSIVALUE                          0x1A		//最后一个数据包的RSSI
#define REG_LR_RSSIVALUE                             0x1B		//当前RSSI
#define REG_LR_HOPCHANNEL                            0x1C		//FHSS 跳频起始信道
#define REG_LR_MODEMCONFIG1                          0x1D		//调制解调器物理层配置1
#define REG_LR_MODEMCONFIG2                          0x1E		//调制解调器物理层配置2
#define REG_LR_SYMBTIMEOUTLSB                        0x1F 		//接收机超时值 单位symbols 范围4~1023 缺省值5
#define REG_LR_PREAMBLEMSB                           0x20 		//前导码长度
#define REG_LR_PREAMBLELSB                           0x21		//前导码长度
#define REG_LR_PAYLOADLENGTH                         0x22		//LoRaTM 负载长度
#define REG_LR_PAYLOADMAXLENGTH                      0x23		//LoRaTM 负载长度最大值
#define REG_LR_HOPPERIOD                             0x24		//FHSS 跳频周期
#define REG_LR_FIFORXBYTEADDR                        0x25		//FIFO 中最后写入字节的地址
#define REG_LR_MODEMCONFIG3                          0x26		//调制解调器物理层配置3
 // I/O settings
#define REG_LR_DIOMAPPING1                           0x40		//DIO0-DIO3引脚映射
#define REG_LR_DIOMAPPING2                           0x41     	//DIO4-DIO5引脚映射、ClkOut 频率
 // Version
#define REG_LR_VERSION                               0x42    	//芯片版本--相关Semtech ID
 // Additional settings
#define REG_LR_PLLHOP                                0x44    	//LORA下未使用
#define REG_LR_TCXO                                  0x4B    	//TCXO 或XTAL 输入设置
#define REG_LR_PADAC                                 0x4D     	//PA 更大功率设置
#define REG_LR_FORMERTEMP                            0x5B     	//IQ 校准期间存储温度
#define REG_LR_BITRATEFRAC                           0x5D		//LORA下未使用
#define REG_LR_AGCREF                                0x61		//AGC 阈值调整
#define REG_LR_AGCTHRESH1                            0x62		//AGC 阈值调整
#define REG_LR_AGCTHRESH2                            0x63		//AGC 阈值调整
#define REG_LR_AGCTHRESH3                            0x64		//AGC 阈值调整
//													 0x70		//PLL 带宽控制

						//？？？？？
#define GPIO_VARE_1                                  0X00   	//DIO0-DIO3引脚映射 写入的值
#define GPIO_VARE_2                                  0X00		//DIO4-DIO5引脚映射 写入的值
#define RFLR_MODEMCONFIG2_SF_MASK                    0x0f		//
#define RFLR_MODEMCONFIG1_CODINGRATE_MASK            0xF1		//
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK          0xFB		//
#define RFLR_MODEMCONFIG1_BW_MASK                    0x0F		//
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK        0xFE		//屏蔽RFLR_MODEMCONFIG1寄存器的同步头显隐性设置
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK        0xfc		//
#define RFLR_MODEMCONFIG3_MOBILE_NODE_MASK           0xF7		//

#define TIME_OUT_INT                                 0x80		//
#define PACKET_RECVER_INT                            0x40		//
#define CRC_ERROR_INT                                0x20		//
#define RECVER_HEAR_INT                              0x10		//
#define FIFO_SEND_OVER                               0x08		//
#define RFLR_IRQFLAGS_CAD                            0x04		//
#define RFLR_IRQFLAGS_FHSS                           0x02		//
#define RFLR_IRQFLAGS_CADD                           0x01		//

#define IRQN_TXD_Value                               0xF7		//发送时要开的中断
#define IRQN_RXD_Value                               0x1F		//接收时要开的中断 9F
#define IRQN_CAD_Value                               0xFA		//信道检测要开的中断
#define IRQN_SEELP_Value                             0xFF		//睡眠时中断全开
#define PACKET_MIAX_Value                            0xff		//


/*类型定义*/
typedef enum
{
   Sleep_mode	        = (unsigned char)0x00,
   Stdby_mode	        = (unsigned char)0x01,
   TX_mode 	        	= (unsigned char)0x02,
   Transmitter_mode		= (unsigned char)0x03,
   RF_mode 				= (unsigned char)0x04,
   Receiver_mode		= (unsigned char)0x05,
   receive_single		= (unsigned char)0x06,
   CAD_mode				= (unsigned char)0x07,
}RFMode_SET;

typedef enum
{
   FSK_mode             = (unsigned char)0x00,
   LORA_mode            = (unsigned char)0x80,
}  Debugging_fsk_ook;

typedef enum{Fl=0,Tr=1}BOOL_t;



/*函数声明*/
void RF_Initial();
void RF_WriteReg( unsigned char addr, unsigned char value );
unsigned char RF_ReadReg( unsigned char addr );
void RF_WriteCmd(unsigned char Cmd);
void RF_ReadMultiReg(unsigned char addr,unsigned char *buf,unsigned char size);
void RF_WriteMultiReg( unsigned char addr, unsigned char *buff, unsigned char size );
void SX1276Reset(void);
void FSK_SEND_PACKET(void);
void SX1276WriteBuffer( unsigned char addr, unsigned char buffer);
unsigned char SX1276ReadBuffer(unsigned char addr);
void SX1276LoRaSetOpMode( RFMode_SET opMode );
void SX1276LoRaFsk( Debugging_fsk_ook opMode );
void SX1276LoRaSetRFFrequency(void);
void SX1276LoRaSetRFPower(unsigned char power );
void SX1276LoRaSetSpreadingFactor(unsigned char factor );
void SX1276LoRaSetNbTrigPeaks(unsigned char value );
void SX1276LoRaSetErrorCoding(unsigned char value );
void SX1276LoRaSetPacketCrcOn(BOOL_t enable );
void SX1276LoRaSetSignalBandwidth(unsigned char bw );
void SX1276LoRaSetImplicitHeaderOn(BOOL_t enable );
void SX1276LoRaSetSymbTimeout(unsigned int value );
void SX1276LoRaSetPayloadLength(unsigned char value );
void SX1276LoRaSetPreamLength(unsigned int value );
void SX1276LoRaSetMobileNode(BOOL_t enable );
void FUN_RF_SENDPACKET(unsigned char *RF_TRAN_P,unsigned char LEN);
void RF_Tx_Cfg(void);
void RF_Rx_Cfg(void);
unsigned char SX1276LORA_INT(void);

//unsigned char RF_SignCS(unsigned short CS_Time);
//unsigned char SX1278_SoftWOR(void);

void RF_RECEIVE ( RFMode_SET opMode );
void RF_CAD_RECEIVE (void);
void RF_SEELP(void);
unsigned char SX1278_Interupt(void);

//杜颖成
unsigned char SX1278_RX(unsigned char *buf,unsigned int time,unsigned char *rssi);
unsigned char SX1278_TX(unsigned char *buf,unsigned int time,unsigned char len);
unsigned char RF_Wait_PacketSent (unsigned int time);
unsigned char RF_Wait_PacketReceive(unsigned int time);
unsigned char RF_Wait_Receive_single(unsigned int time);
unsigned char SX1278_CAD(void);

#endif
