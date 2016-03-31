#include <reg52.h>
#include "max7219.h"
#include "config.h"
unsigned char code max7219_7led_code[18] = {0x7e,0x30,0x6d,0x79, //0-1-2-3
									0x33,0x5b,0x5f,0x70, //4-5-6-7
									0x7f,0x7b,0x01,0x4e, //8-9-"-"-E
									0x37,0x0e,0x67,0x00}; //H-L-P-空白
//max7219的初始化
void init_max7219()
{
	wr_max7219(Addr_Shutdowm,NormalOperation);
	wr_max7219(Addr_Scan_Limit,ScanDigit);
	wr_max7219(Addr_Intensity,DecodeDigit);
	wr_max7219(Addr_Decode_Mode,IntensityGrade); 
	wr_max7219(Addr_Digit0,0x00);	 
	wr_max7219(Addr_Digit1,0x00);
	wr_max7219(Addr_Digit2,0x00);	 
	wr_max7219(Addr_Digit3,0x00);
}

//max219寄存器写入值
void wr_max7219(uchar addr,uchar dat)
{
	uchar i;
	Max7219_Load=0;
	Max7219_Clk=0;
	for(i=0;i<8;i++)
	{
		Max7219_Din = (bit)(addr&0x80);
		addr=addr<<1;
		Max7219_Clk=1;
		Max7219_Clk=0;
	}
	for(i=0;i<8;i++)
	{
		Max7219_Din=(bit)(dat&0x80);
		dat=dat<<1;
		Max7219_Clk=1;
		Max7219_Clk=0;
	}
	Max7219_Load=1;
	Max7219_Load=0;
}