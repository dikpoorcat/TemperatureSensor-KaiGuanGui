#include "EEPROM.H"
#include "string.h"
/********************************************************************
函数：void Read_Addr(INT8U *OutAddr)
功能：读取保存在内部EEPROM中的地址
输出：OutAddr 确保长度为4以上
**********************************************************************/
void Write_Addr(unsigned char  *InAddr)
{
      unsigned char  Temp[5];
       memcpy(Temp,InAddr,4);
       Temp[4]=(Temp[0]+Temp[1]+Temp[2]+Temp[3]);
       E2P_Write(0,Temp,5);
      
}
/********************************************************************
函数：void Read_Addr(INT8U *OutAddr)
功能：读取保存在内部EEPROM中的地址
输出：OutAddr 确保长度为4以上
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
函数名：void  E2P_Write(unsigned int addr,unsigned char *p_data,unsigned char Len )
功能：  向EEPROM地址内写入，一定直接长度的数据
入参：   addr，要写入数据的相对地址，p_data，存放要写入的数据的指针，Len，要写入的
        数据的长度。
出参：
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
函数名：void E2P_Read(unsigned int addr,unsigned char *OutData,unsigned char OutLen)
功能：  从EEPROM地址读出一定直接长度的数据
入参：   addr，要读出数据的相对地址，OutData，存放要读出的数据的指针，OutLen，要读出的
        数据的长度。
出参：
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