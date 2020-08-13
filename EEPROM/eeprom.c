#include "EEPROM.H"
#include "string.h"
/********************************************************************
������void Read_Addr(INT8U *OutAddr)
���ܣ���ȡ�������ڲ�EEPROM�еĵ�ַ
�����OutAddr ȷ������Ϊ4����
**********************************************************************/
void Write_Addr(unsigned char  *InAddr)
{
      unsigned char  Temp[5];
       memcpy(Temp,InAddr,4);
       Temp[4]=(Temp[0]+Temp[1]+Temp[2]+Temp[3]);
       E2P_Write(0,Temp,5);
      
}
/********************************************************************
������void Read_Addr(INT8U *OutAddr)
���ܣ���ȡ�������ڲ�EEPROM�еĵ�ַ
�����OutAddr ȷ������Ϊ4����
**********************************************************************/
void Read_Addr(unsigned char  *OutAddr)
{
       unsigned char  Temp[5];
       unsigned char  CS;
       
       E2P_Read(0,Temp,5);
       CS=(Temp[0]+Temp[1]+Temp[2]+Temp[3]);
       if(Temp[4]!=CS)
       {
            memset(Temp,0xff,4);
            Write_Addr(Temp);
       }
       if(Temp[0]==0)
       {
            memset(Temp,0xff,4);
            Write_Addr(Temp);
       }
        memcpy(OutAddr,Temp,4);
}

void EEPROM_Init()
{
    FLASH_DeInit();
   // FLASH_Unlock( FLASH_MemType_Data);
   // while((FLASH->IAPSR & FLASH_IAPSR_DUL) == 0);
    FLASH_SetProgrammingTime(FLASH_ProgramMode_Standard );
}
/**********************************************************************
��������void  E2P_Write(unsigned int addr,unsigned char *p_data,unsigned char Len )
���ܣ�  ��EEPROM��ַ��д�룬һ��ֱ�ӳ��ȵ�����
��Σ�   addr��Ҫд�����ݵ���Ե�ַ��p_data�����Ҫд������ݵ�ָ�룬Len��Ҫд���
        ���ݵĳ��ȡ�
���Σ�
***********************************************************************/
unsigned char  E2P_Write(unsigned int addr,unsigned char *p_data,unsigned char Len )
{
    unsigned int i;
    FLASH_Unlock( FLASH_MemType_Data);
    while((FLASH->IAPSR & FLASH_IAPSR_DUL) == 0);
   
    if(p_data==NULL)
    {
      return -1;
    }
    
    for(i=0;i<Len;i++)
    {
      FLASH_ProgramByte(0x1000+addr+i, *(p_data+i));
   //   while((FLASH->IAPSR & FLASH_IAPSR_EOP) == 0);
      FLASH_WaitForLastOperation(FLASH_MemType_Data);
    }
    FLASH_Lock(FLASH_MemType_Data);
}
/**********************************************************************
��������void E2P_Read(unsigned int addr,unsigned char *OutData,unsigned char OutLen)
���ܣ�  ��EEPROM��ַ����һ��ֱ�ӳ��ȵ�����
��Σ�   addr��Ҫ�������ݵ���Ե�ַ��OutData�����Ҫ���������ݵ�ָ�룬OutLen��Ҫ������
        ���ݵĳ��ȡ�
���Σ�
***********************************************************************/
void E2P_Read(unsigned int addr,unsigned char *OutData,unsigned char OutLen)
{
    unsigned int i;
    FLASH_Unlock( FLASH_MemType_Data);
    while((FLASH->IAPSR & FLASH_IAPSR_DUL) == 0);
    
    for(i=0;i<OutLen;i++)
    {
      *(OutData+i)=FLASH_ReadByte(0x1000+addr+i);
    }
    FLASH_Lock(FLASH_MemType_Data);
}