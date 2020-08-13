#include "time.h"
#include "stdio.h"
const char DATIME[] ={__DATE__};
/**********************************************************************
函数名：void GetTime( unsigned char *BANBEN ,unsigned char Len)
功能：  通过__DATE__获取系统日期
入参：   BANBEN  存放日期的年月，Recv_Len  数组长度  
出参： 
***********************************************************************/
void GetTime( unsigned char *BANBEN ,unsigned char Len)
{
        char *tp;
        tp= (char *)DATIME;
        BANBEN[0]=tp[9]-0x30;
        BANBEN[0]= BANBEN[0]<<4;
        BANBEN[0]+=(tp[10]-0x30);
        if (0==memcmp(tp,"Jan",3))
        {
            BANBEN[1]=01;
        }
        else if (0==memcmp(tp,"Feb",3))
        {
            BANBEN[1]=02;
        }
        else if (0==memcmp(tp,"Mar",3))
        {
            BANBEN[1]=03;
        }
        else if (0==memcmp(tp,"Apr",3))
        {
            BANBEN[1]=04;
        }
        else if (0==memcmp(tp,"May",3))
        {
            BANBEN[1]=05;
        }
        else if (0==memcmp(tp,"Jun",3))
        {
            BANBEN[1]=06;
        }
        else if (0==memcmp(tp,"Jul",3))
        {
            BANBEN[1]=07;
        }
        else if (0==memcmp(tp,"Aug",3))
        {
            BANBEN[1]=8;
        }
        else if (0==memcmp(tp,"Sep",3))
        {
            BANBEN[1]=9;
        }
        else if (0==memcmp(tp,"Oct",3))
        {
            BANBEN[1]=10;
        }
        else if (0==memcmp(tp,"Nov",3))
        {
            BANBEN[1]=11;
        }
        else if (0==memcmp(tp,"Dec",3))
        {
            BANBEN[1]=12;
        }
}
