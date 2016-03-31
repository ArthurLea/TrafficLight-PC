#ifndef _LED_H_
#define _LED_H_
	
	#include <reg52.h>

	sbit South_North_Red = P0^0;
	sbit South_North_Yellow = P0^1;
	sbit South_North_Green = P0^2;
	
	sbit East_West_Red = P0^3;
	sbit East_West_Yellow = P0^4;
	sbit East_West_Green = P0^5;

	sbit South_North_Footway_Red = P0^6;//南北人行道红灯
	sbit South_North_Footway_Green = P3^6;//南北人行道绿灯
	sbit East_West_Footway_Red = P0^7;//东西人行道红灯	
	sbit East_West_Footway_Green = P3^7;//东西人行道绿灯

//	typedef struct time 
//	{
//		uchar Sou_Nor_Red_Time;
//		uchar Sou_Nor_Green_Time;
//		uchar Sou_Nor_Yellow_Time;
//		uchar West_East_Yellow_Time;	
//	}light_time; 

	extern void quit_led();
	extern void init_led();
	extern void all_led_red();
	extern void buzzer();
	extern void numeric_led_display(uint flag);
#endif