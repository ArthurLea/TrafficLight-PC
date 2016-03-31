#ifndef _KEY_H_
#define _KEY_K_

#define SET_SOU_NOR_TIME 			0X0A
#define SET_WEST_EAST_TIME 			0X0B
#define DEL 					    0X0C
#define OK							0X0F
	 //sbit Stop = P3^3;
	 extern uchar scankey();
	 extern void set_time(uint flag);
#endif