#ifndef _EEPROM_H_
#define _EEPROM_H_
#include "stm8l15x_flash.h"
#include "stdio.h"
void EEPROM_Init();
unsigned char  E2P_Write(unsigned int addr,unsigned char *p_data,unsigned char Len );
void E2P_Read(unsigned int addr,unsigned char *OutData,unsigned char OutLen);
void Read_Addr(unsigned char  *OutAddr);
void Write_Addr(unsigned char  *InAddr);


#endif //_EEPROM_H_
