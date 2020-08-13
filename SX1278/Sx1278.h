#ifndef __SX1278__H__
#define __SX1278__H__

/*ͷ�ļ�*/
#include "mytypedef.h"
#include "stdarg.h"
#include "stm8l15x.h"
#include "stm8l15x_gpio.h"
#include "main.h"
#include "bsp.h"


/*�궨��*/
						//���¼Ĵ�������ΪLORAģʽ��
#define REG_LR_FIFO                                  0x00    	//FIFO��/д����
 // Common settings
#define REG_LR_OPMODE                                0x01 		//����ģʽ&LoRaTM/FSK ѡ��
#define REG_LR_BANDSETTING                           0x04     	//LORA��δʹ��
#define REG_LR_FRFMSB                                0x06      	//��Ƶ�ز�Ƶ�ʣ������Чλ
#define REG_LR_FRFMID                                0x07		//��Ƶ�ز�Ƶ�ʣ��м�λ
#define REG_LR_FRFLSB                                0x08 		//��Ƶ�ز�Ƶ�ʣ������Чλ
 // Tx settings
#define REG_LR_PACONFIG                              0x09     	//PA ѡ���������ʿ���
#define REG_LR_PARAMP                                0x0A     	//PA б��/б��ʱ��͵�����PLL �Ŀ���
#define REG_LR_OCP                                   0x0B 		//������������
 // Rx settings
#define REG_LR_LNA                                   0x0C 		//LNA ����
 // LoRa registers
#define REG_LR_FIFOADDRPTR                           0x0D		//FIFO SPI ָ��
#define REG_LR_FIFOTXBASEADDR                        0x0E    	//��ʼTx ����
#define REG_LR_FIFORXBASEADDR                        0x0F 		//��ʼRx ����
#define REG_LR_FIFORXCURRENTADDR                     0x10 		//���������ݰ�����ʼ��ַ
#define REG_LR_IRQFLAGSMASK                          0x11 		//��ѡIRQ ��־����
#define REG_LR_IRQFLAGS                              0x12 		//IRQ ��־  �ж�״̬�Ĵ���
#define REG_LR_NBRXBYTES                             0x13 		//���յ����ֽ���
#define REG_LR_RXHEADERCNTVALUEMSB                   0x14 		//���յ�����Ч��ͷ��
#define REG_LR_RXHEADERCNTVALUELSB                   0x15 		//���յ�����Ч��ͷ��
#define REG_LR_RXPACKETCNTVALUEMSB                   0x16 		//���յ�����Ч���ݰ���
#define REG_LR_RXPACKETCNTVALUELSB                   0x17		//���յ�����Ч���ݰ���
#define REG_LR_MODEMSTAT                             0x18		//LoRaTM ���ƽ����״̬
#define REG_LR_PKTSNRVALUE                           0x19		//���һ�����ݰ�������ȹ�ֵ
#define REG_LR_PKTRSSIVALUE                          0x1A		//���һ�����ݰ���RSSI
#define REG_LR_RSSIVALUE                             0x1B		//��ǰRSSI
#define REG_LR_HOPCHANNEL                            0x1C		//FHSS ��Ƶ��ʼ�ŵ�
#define REG_LR_MODEMCONFIG1                          0x1D		//���ƽ�������������1
#define REG_LR_MODEMCONFIG2                          0x1E		//���ƽ�������������2
#define REG_LR_SYMBTIMEOUTLSB                        0x1F 		//���ջ���ʱֵ ��λsymbols ��Χ4~1023 ȱʡֵ5
#define REG_LR_PREAMBLEMSB                           0x20 		//ǰ���볤��
#define REG_LR_PREAMBLELSB                           0x21		//ǰ���볤��
#define REG_LR_PAYLOADLENGTH                         0x22		//LoRaTM ���س���
#define REG_LR_PAYLOADMAXLENGTH                      0x23		//LoRaTM ���س������ֵ
#define REG_LR_HOPPERIOD                             0x24		//FHSS ��Ƶ����
#define REG_LR_FIFORXBYTEADDR                        0x25		//FIFO �����д���ֽڵĵ�ַ
#define REG_LR_MODEMCONFIG3                          0x26		//���ƽ�������������3
 // I/O settings
#define REG_LR_DIOMAPPING1                           0x40		//DIO0-DIO3����ӳ��
#define REG_LR_DIOMAPPING2                           0x41     	//DIO4-DIO5����ӳ�䡢ClkOut Ƶ��
 // Version
#define REG_LR_VERSION                               0x42    	//оƬ�汾--���Semtech ID
 // Additional settings
#define REG_LR_PLLHOP                                0x44    	//LORA��δʹ��
#define REG_LR_TCXO                                  0x4B    	//TCXO ��XTAL ��������
#define REG_LR_PADAC                                 0x4D     	//PA ����������
#define REG_LR_FORMERTEMP                            0x5B     	//IQ У׼�ڼ�洢�¶�
#define REG_LR_BITRATEFRAC                           0x5D		//LORA��δʹ��
#define REG_LR_AGCREF                                0x61		//AGC ��ֵ����
#define REG_LR_AGCTHRESH1                            0x62		//AGC ��ֵ����
#define REG_LR_AGCTHRESH2                            0x63		//AGC ��ֵ����
#define REG_LR_AGCTHRESH3                            0x64		//AGC ��ֵ����
//													 0x70		//PLL �������

						//����������
#define GPIO_VARE_1                                  0X00   	//DIO0-DIO3����ӳ�� д���ֵ
#define GPIO_VARE_2                                  0X00		//DIO4-DIO5����ӳ�� д���ֵ
#define RFLR_MODEMCONFIG2_SF_MASK                    0x0f		//
#define RFLR_MODEMCONFIG1_CODINGRATE_MASK            0xF1		//
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK          0xFB		//
#define RFLR_MODEMCONFIG1_BW_MASK                    0x0F		//
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK        0xFE		//����RFLR_MODEMCONFIG1�Ĵ�����ͬ��ͷ����������
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

#define IRQN_TXD_Value                               0xF7		//����ʱҪ�����ж�
#define IRQN_RXD_Value                               0x1F		//����ʱҪ�����ж� 9F
#define IRQN_CAD_Value                               0xFA		//�ŵ����Ҫ�����ж�
#define IRQN_SEELP_Value                             0xFF		//˯��ʱ�ж�ȫ��
#define PACKET_MIAX_Value                            0xff		//


/*���Ͷ���*/
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



/*��������*/
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

//��ӱ��
unsigned char SX1278_RX(unsigned char *buf,unsigned int time,unsigned char *rssi);
unsigned char SX1278_TX(unsigned char *buf,unsigned int time,unsigned char len);
unsigned char RF_Wait_PacketSent (unsigned int time);
unsigned char RF_Wait_PacketReceive(unsigned int time);
unsigned char RF_Wait_Receive_single(unsigned int time);
unsigned char SX1278_CAD(void);

#endif
