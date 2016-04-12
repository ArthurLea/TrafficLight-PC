#include "config.h"
#include "max7219.h"
#include "led.h"
uchar count = 20;

extern volatile uchar Sou_Nor_Red_Time;
extern volatile uchar Sou_Nor_Green_Time;
extern volatile uchar Sou_Nor_Yellow_Time;
extern volatile uchar West_East_Yellow_Time;

extern volatile uchar Sou_Nor_Red_Time_Temp;
extern volatile uchar Sou_Nor_Green_Time_Temp;
//系统在自动模式下
//volatile uchar befor_stop_led_state = 0;//0表示南北红，1表示南北黄，2表示南北绿
volatile uchar enter_stop_flag = 0;//紧急按钮标记，1表示已经触发
volatile uchar car_flow_detection_flag = 0;//自动模式下循环一次（南北绿灯-南北黄灯-南北红灯-南北绿灯），
								  //置为1，检测车流量
//闯红灯标志								  
volatile uchar s_n_stop_flag = 0;//南北红灯标志
volatile uchar e_w_stop_flag = 0;//东西红灯标志

volatile uchar cycle_nums = 0;/* 循环周期数目*/ 
void display()
{
	count--;
	if(count == 0) 
	{
		count = 20;
		//南北绿，东西红
		if(Sou_Nor_Green_Time_Temp != 0XFF)//还没减到0，0-1-->0XFF
		{
			s_n_stop_flag = 0;
			e_w_stop_flag = 1;	
			numeric_led_display(1);	
			Sou_Nor_Green_Time_Temp--;
		}
		//南北黄。东西红
		else if(Sou_Nor_Yellow_Time != 0XFF)
		{
			s_n_stop_flag = 1;
			e_w_stop_flag = 1;	
			numeric_led_display(2);  
			Sou_Nor_Yellow_Time--;	
		}
		//南北红，东西绿
		else if(Sou_Nor_Red_Time_Temp != 0XFF)
		{
			s_n_stop_flag = 1;
			e_w_stop_flag = 0;	 
			numeric_led_display(3);	 
			Sou_Nor_Red_Time_Temp--;
		}
		//南北红，东西黄
		else if(West_East_Yellow_Time != 0XFF)
		{
			s_n_stop_flag = 1;
			e_w_stop_flag = 1;
			numeric_led_display(4);
			West_East_Yellow_Time--;	
		}
		//重新计数
		if((Sou_Nor_Green_Time_Temp==0XFF) && (Sou_Nor_Yellow_Time==0XFF) && (Sou_Nor_Red_Time_Temp==0XFF) && (West_East_Yellow_Time==0XFF))
		{
			s_n_stop_flag = 0;
			e_w_stop_flag = 0;

			Sou_Nor_Green_Time_Temp =  Sou_Nor_Green_Time;
			Sou_Nor_Yellow_Time = 2;	
			West_East_Yellow_Time = 2;
			Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;

			cycle_nums++;//一个红绿灯周期达到
			car_flow_detection_flag = 1;//打开车流量检测标记
		}
	}
}

void int0() interrupt 0//外部中断INT0，对于特殊车总的车辆（120,110,119）等，禁止其他车辆通行信号
{
	enter_stop_flag = 1;
	buzzer();			 
}
void timer0() interrupt 1//定时器T0中断
{
	TH0 = 0X4C;//方式一需要重新对定时器赋值
	TL0 = 0X00;
	display();
}
void int1() interrupt 2//外部中断INT1
{
		
}

