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
void send_run_red_light_to_phone(uchar command,uchar run_red_light_result)
{
	uchar dat[8] = {'0'};
	uint i = 0;
	dat[0] = '$';
	dat[1] = '8';
	dat[2] = command;
	dat[3] = run_red_light_result;
	dat[4] = '0';
	dat[5] = '0';
	dat[6] = '0';
	dat[7] = 'F';
	for(i=0;i<8;i++)
	{
		sendByte(dat[i]);
		delay(1);
	}	
}		  
void send_sou_nor_nums_to_phone(uchar command,uint sou_nor_nums)
{
	uchar dat[8] = {'0'};
	uint i = 0;
	dat[0] = '$';
	dat[1] = '8';
	dat[2] = command;
	dat[3] = sou_nor_nums/100 + '0';
	dat[4] = sou_nor_nums/10 + '0';
	dat[5] = sou_nor_nums%10 + '0';
	dat[6] = '0';
	dat[7] = 'F';
	for(i=0;i<8;i++)
	{
		sendByte(dat[i]);
		delay(1);
	}
}
void send_eas_wes_nums_to_phone(uchar command,uint eas_wes_nums)
{
	uchar dat[8] = {'0'};
	uint i = 0;
	dat[0] = '$';
	dat[1] = '8';
	dat[2] = command;
	dat[3] = eas_wes_nums/100 + '0';  
	dat[4] = eas_wes_nums/10 + '0';
	dat[5] = eas_wes_nums%10 + '0';
	dat[6] = '0';
	dat[7] = 'F';
	for(i=0;i<8;i++)
	{
		sendByte(dat[i]);
		delay(1);
	}
}
void send_cycle_nums_to_phone(uchar command,uint cycle_nums)
{
	uchar dat[8] = {'0'};
	uint i = 0;
	dat[0] = '$';
	dat[1] = '8';
	dat[2] = command;
	dat[3] = cycle_nums/100 + '0';	
	dat[4] = cycle_nums/10 + '0';
	dat[5] = cycle_nums%10 + '0';
	dat[6] = '0';
	dat[7] = 'F';
	for(i=0;i<8;i++)
	{
		sendByte(dat[i]);
		delay(1);	
	}	
}
void send_cycle_time_to_phone(uchar command,uint cycle_time)
{
	uchar dat[8] = {'0'};
	uint i = 0;
	dat[0] = '$';
	dat[1] = '8';
	dat[2] = command;
	dat[3] = cycle_time/100 + '0';	
	dat[4] = cycle_time/10 + '0';
	dat[5] = cycle_time%10 + '0';
	dat[6] = '0';
	dat[7] = 'F';
	for(i=0;i<8;i++)
	{
		sendByte(dat[i]);
		delay(1);	
	}
}
void send_reset_to_phone(uchar command)
{
		uchar dat[8] = {'0'};
	uint i = 0;
	dat[0] = '$';
	dat[1] = '8';
	dat[2] = command;
	dat[3] = '0';	
	dat[4] = '0';
	dat[5] = '0';
	dat[6] = '0';
	dat[7] = 'F';
	for(i=0;i<8;i++)
	{
		sendByte(dat[i]);
		delay(1);	
	}	
}
