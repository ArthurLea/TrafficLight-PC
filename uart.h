#ifndef _UART_H_
#define _UART_H_
	#include "config.h"
	#define UART_IDLE               0x02
	#define UART_RECEIVE            0x03
	#define UART_PROCESS            0x04

	extern uchar checkByte();
	extern uchar getByte();
	extern void sendByte(uchar value);

#endif