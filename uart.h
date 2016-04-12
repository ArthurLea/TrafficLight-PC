#ifndef _UART_H_
#define _UART_H_
	#include "config.h"
	#define UART_IDLE               0x02
	#define UART_RECEIVE            0x03
	#define UART_PROCESS            0x04
	
	#define PHONE_SET_OK			'0'
	#define PHONE_SET_TIME			'1'
	#define PHONE_SET_S_N_TIME		'1'
	#define PHONE_SET_E_W_TIME 		'2'
	#define PHONE_SET_STOP			'2'
	#define PHONE_SET_STOP_QUIT		'3'
//下位机直接传给终端的命令
	#define PHONE_SET_RUN_RED_LIGHT	'4'
	#define PHONE_SET_CAR_FLOW		'5'

	extern uchar checkByte();
	extern uchar getByte();
	extern void sendByte(uchar value);
	extern void send_run_red_light_to_phone(uchar command,uchar run_red_light_result);
	extern void send_sou_nor_nums_to_phone(uchar command,uint sou_nor_nums);
	extern void send_eas_wes_nums_to_phone(uchar command,uint eas_wes_nums);
	extern void send_cycle_nums_to_phone(uchar command,uint cycle_nums);
	extern void send_cycle_time_to_phone(uchar command,uint cycle_time);
	extern void send_comm_to_phone(uchar command);
#endif