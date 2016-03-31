#include <reg52.h>
#include "config.h"


uchar check_receive_ = 0;

void uart() interrupt 4//串口RI/TI中断
{
	TI = 0;
	if(RI == 1)
	{
		RI = 0;
		check_receive_ = 1;//表示上位机手机端有数据过来
	}
}
uchar checkByte()
{	
	return check_receive_;
}
uchar getByte()
{
	check_receive_ = 0;
	return SBUF;
}

//void sendArrayN(char *dat, unsigned char n)
//{
//
//}
//void sendArray(char *dat)
//{
//	
//}
void sendByte(uchar value)
{
   SBUF = value;
}
