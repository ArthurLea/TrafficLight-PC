#ifndef _CONFIG_H_
#define _CONFIG_H_

	#include <reg52.h>

	typedef unsigned char uchar; //#define uchar unsigned char
	typedef unsigned int uint; //#define uint unsigned int	
	typedef unsigned short ushort;
	typedef unsigned long u32;

	sbit Buzzer = P2^3;//·äÃùÆ÷½Ó¿Ú

	extern void delay(unsigned int t);
	extern void buzzer();
	extern ushort CRC16(uchar *puchMsg,ushort usDataLen);
#endif
