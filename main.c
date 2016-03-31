#include <reg52.h>
#include "config.h"
#include "max7219.h"
#include "led.h"
#include "key.h"
#include "uart.h"
#include "infrared_tube.h"

//串口需要变量
volatile uchar uartState = UART_IDLE;
volatile uint timeout;
volatile uint pointer;

volatile uchar g_cCommand; 
volatile uchar send_Buf[16] = {0};//通过串口转蓝牙发送给手机的缓冲区
volatile uchar recieve_Buf[16] = {0};//数据接收的缓冲区
/*自动模式,默认各方向均为20s,同时开始以南北方向为准，南北为绿，东西为红
 *手动模式,操作者自己设置南北东西方向上的绿灯时间
 *注意：南北方向上的绿灯时间和东西方向上的红灯时间相同
 *      东西方向上的绿灯时间和南北方向上的红灯时间相同
 */
//红绿黄灯时间全局易变变量
volatile uchar Sou_Nor_Red_Time = 5;
volatile uchar Sou_Nor_Green_Time = 5;
volatile uchar Sou_Nor_Yellow_Time = 2;
volatile uchar West_East_Yellow_Time = 2;

//红绿灯时时间备份
volatile uchar Sou_Nor_Red_Time_Temp;
volatile uchar Sou_Nor_Green_Time_Temp;

uchar code big_time = 10;//长时间
uchar code small_time = 5;//短时间

volatile uchar night_flag = 0;//夜晚模式标志
volatile uchar Phone_Set_Finish = 0;//手机设置完毕标记
//外部共享变量
extern uchar stop_flag;//禁止通行标记
extern uchar car_flow_detection_flag;
//车流量检测
volatile uint car_num[4] = {0,0,0,0};//0-东方向上的车流量
										//1-南方向
										//2-西方向
										//3-北方向
extern volatile uchar cycle_nums;//from interrupt.c file   

//函数声明
void init_register_s52();
void init_Numeric_Display();
void phone_set();


int main()
{
	uchar key_num = 17;/*当前按键值*/
	uint run_red_light_result = 0;/*闯红绿灯的结果*/
	u32 sou_nor_nums = 0;/* 南北方向上的车流量总和*/
	u32 eas_wes_nums = 0;/* 东西方向上的车流量总和*/
	uint cycle_time = 0;/* 循环周期的时间*/
	init_register_s52();
	init_max7219();
	quit_led();
	buzzer();

	while(1)
	{	 
		/*系统开始检测系统模式，当OK键在所有键以及手机控制之前按下时为自动模式,每个方向计时20s
		 *						否则通过其他方式（其他按键或者手机端）设置后再次按下系统启动
		 *
		 *开始检测按键或手机远程设置
		 */
		key_num = scankey();
		if((key_num==OK) || (Phone_Set_Finish==1))//按键端直接控制，系统正式启动 
		{
			init_led();
			init_Numeric_Display();
			TR0 = 1;
			while(1)
			{
				/*
				 *紧急情况模块
				 */
				if(stop_flag==1)//紧急情况标记
				{
					buzzer();			  
					all_led_red();
					while(stop_flag)
					{
						key_num = scankey();
						if(OK == key_num)
						{
							stop_flag = 0;
							TR0 = 1;//再次启动定时器
						}
					}
				}
				/*
				 * 闯红灯检测模块
				 */
				 run_red_light_result = run_red_light();
				 if(run_red_light_result != 0)
				 {
				 	
					//将闯红灯具体方位数据通过串口转蓝牙传送到手机终端
					//数据CRC封装

					//send_run_red_light_to_phone();
					run_red_light_result = 0;
					buzzer();
				 }
				/*
				 * 车辆检测模块
				 */
				/*得到每个方向上的车流量储存到car_num数组中*/
				car_flow_detection(car_num);
				/*计算南北、东西方向上的车辆数算数平均值，模拟取出整数值*/
				sou_nor_nums = (car_num[1]+car_num[3])/cycle_nums;
				eas_wes_nums = (car_num[0]+car_num[2])/cycle_nums;
				/*计算循环周期的时间和*/
				cycle_time = Sou_Nor_Green_Time + Sou_Nor_Red_Time + Sou_Nor_Yellow_Time + West_East_Yellow_Time;
				/* 计算车流量*/
				
				//将两个方向上的车流量数据通过串口转蓝牙传送到手机终端
				//数据CRC封装
				//car_num[0]--[3],sou_nor_nums,eas_wes_nums,cycle_nums，cycle_time

				//send_car_flow_to_phone(car_num);
				
				if(car_flow_detection_flag == 1)
				{ 
					 //模拟改变南北东西方向上的红绿灯时间
					 if(Sou_Nor_Green_Time <= small_time)
					 {
					 	if(sou_nor_nums > 10)//10辆
							Sou_Nor_Green_Time = big_time;
						else
							Sou_Nor_Green_Time = small_time;	
					 }
					 else if((Sou_Nor_Green_Time>small_time) && Sou_Nor_Green_Time<=big_time)
					 {
					 	if(sou_nor_nums <= 10)
							Sou_Nor_Green_Time = small_time; 
						else
							Sou_Nor_Green_Time = big_time;
					 }
					 else if(Sou_Nor_Green_Time > big_time)
					 {
					 	if(sou_nor_nums <= 20)
							Sou_Nor_Green_Time = small_time; 
						else if((sou_nor_nums>20) && (sou_nor_nums <= 40))
							Sou_Nor_Green_Time = big_time + 10;
						else if((sou_nor_nums>40) && (sou_nor_nums <= 70))
							Sou_Nor_Green_Time = big_time + 20;
						else
							Sou_Nor_Green_Time = 99;
					 }
					 //其他的正常

					 //模拟改变东西东西方向上的红绿灯时
					 if(Sou_Nor_Red_Time <= small_time)
					 {
					 	if(eas_wes_nums > 10)//10辆
							Sou_Nor_Red_Time = big_time;
						else
							Sou_Nor_Red_Time = small_time;	
					 }
					 else if((Sou_Nor_Red_Time>small_time) && (Sou_Nor_Red_Time<=big_time))
					 {
					 	if(eas_wes_nums <= 10)
							Sou_Nor_Red_Time = small_time;
						else
							Sou_Nor_Red_Time = big_time; 
					 }
					 else if(Sou_Nor_Red_Time > big_time)
					 {
					 	if(eas_wes_nums <= 20)
							Sou_Nor_Red_Time = small_time; 
						else if((eas_wes_nums>20) && (eas_wes_nums <= 40))
							Sou_Nor_Red_Time = big_time + 10;
						else if((eas_wes_nums>40) && (eas_wes_nums <= 70))
							Sou_Nor_Red_Time = big_time + 20;
						else
							Sou_Nor_Red_Time = 99;
					 } 
					 Sou_Nor_Green_Time_Temp = Sou_Nor_Green_Time;
					 Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;

					 car_flow_detection_flag = 0;//自动循环模式标记检测处理结束，置为0	
				}
				if((sou_nor_nums>10000) || (eas_wes_nums>10000))//重置车流量数据，并将循环次数设置为0
				{
					car_num[0] = 0;
					car_num[1] = 0;
					car_num[2] = 0;
					car_num[3] = 0;
					cycle_nums  = 0;
				}

				/*
				 * 手机端远程控制模块
				 */
				 //phone_set();
			}
		}
		/*
		 *	时间手动设置模块
		 *  1、下位机按键模块
		 *  2、手机端命令设置模块
		 */
		//OK键没有按下，此时是用户自己设置各方向的红黄绿灯时间,修改的是对应方向上的绿灯时间
		//1、下位机按键模块
		if(key_num == SET_SOU_NOR_TIME)//设置南北方向绿灯时间
		{
			wr_max7219(Addr_Digit2,0x00);	 
			wr_max7219(Addr_Digit3,0x00);	
			set_time(1);//南北	
		}
		if(key_num == SET_WEST_EAST_TIME)
		{
			wr_max7219(Addr_Digit0,0x00);	 
			wr_max7219(Addr_Digit1,0x00);				
			set_time(2);
		}
		//2、手机端命令设置模块
		phone_set();
	}
	return 0;
}

/*
 * 手机端
 */
void phone_set()
{
	uint i;
	switch(uartState)
	{
		case UART_IDLE://串口未有数据状态
		{
			pointer = 0;
			timeout = 0;
			g_cCommand = 0;
			if(checkByte())  
			{  
				recieve_Buf[pointer++] = getByte();   
				uartState = UART_RECEIVE;
			}
			break;
		}
		case UART_RECEIVE:
		{
			timeout++;
//			if ((timeout>0x8000) || (recieve_Buf[(pointer-1)]==0XEF) || (pointer>7) || (pointer >= recieve_Buf[0]))//[0]表示数据长度
//			{
//				uartState = UART_PROCESS; 
//				break;
//			}
			if ((timeout>0x8000) || (recieve_Buf[(pointer-1)]=='F') || (pointer>7) || (pointer >= recieve_Buf[0]))//[0]表示数据长度
			{
				uartState = UART_PROCESS; 
				break;
			}
			if(checkByte())  
			{
				recieve_Buf[pointer++] = getByte();
				timeout = 0;
				uartState = UART_RECEIVE;
			}
			break;
		}
		case UART_PROCESS:
		{
			for(i=0; i<pointer;i++)
			{
				sendByte(recieve_Buf[i]);
				delay(1);
			}
			//将手机端接收到的字符串转换为16进制
			uartState = UART_IDLE;
	  		g_cCommand = recieve_Buf[1] ; //取出功能命令字
//					i =	g_cReceBuf[0]-2;
//					CRCdata = CRC16(g_cReceBuf,i);//进行CRC循环校验

			if((timeout<0x8000)/* && (CRCdata==(u16)(g_cReceBuf[i]<<8)+g_cReceBuf[i+1])*/ )
			{
				switch(g_cCommand)
				{
					case '1'://设置时间
					{
						if(recieve_Buf[2] == '1')//设置南北绿灯时间
						{						
							Sou_Nor_Green_Time = recieve_Buf[3] - '0';//字符的ASCII
							Sou_Nor_Green_Time_Temp =  Sou_Nor_Green_Time;
							wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
							wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
						}
						if(recieve_Buf[2] == '2')//设置东西绿灯时间
						{							
							Sou_Nor_Red_Time = recieve_Buf[3] - '0';
							Sou_Nor_Red_Time_Temp =  Sou_Nor_Red_Time;
							wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
							wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
						}	
						break;
					}
					case '2'://禁止通行
						stop_flag = 1;
						break;
					case '3'://夜晚模式
						night_flag = 1;
						break;
					case '4'://手机确认设置完毕
						Phone_Set_Finish = 1;
						break;
				}
			}
			break;
		}
	} 
}

/*
 *初始化52单片机的寄存器，外部中断，定时器，以及相依优先级等
 */
void init_register_s52()
{	
	//CPU查询中断的顺序（INT0->T0->INT1->TI/RI->T2）
	IE = 0X97;//打开相应中断((EA(总开关)-1))--(不用-0)--(T2-0)--(串行口-1)
						 //--(T1-0)--(INT1-1)--(T0-1)--(INT0-1))
	TMOD = 0X21;//T1方式2，T0方式1设置（GATE-C/T-M1-M0--GATE-C/T-M1-M0）
	IT0 = 1;//INT0下降沿有效
	IT1 = 1;//下降沿有效
	TL1 = 0XFD;
	TH0 = 0X4C;//定时器
	TL0 = 0X00;
	//TR0 = 1;//启动定时器
	
	TH1 = 0XFD;//波特率9600,SMOD不加倍
	TL1 = 0XFD;
	TR1 = 1;//启动T1
	SCON = 0X50;//串口方式，方式1，允许接收

	PS = 1;	//串行口优先级
	PX0 = 1; 
	PX1 = 1;//INT1中断优先级
	//IP = 0X21;//(不用-不用-T2-串行口-T1-INT1-T0-INT0)
	Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;
	Sou_Nor_Green_Time_Temp = Sou_Nor_Green_Time;
}

/*
 *初始化数码管显示数字,以南北为基准显示，也就是显示的是当前南北通行时绿灯时间
 */
void init_Numeric_Display()
{
	wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
	wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
	wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
	wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
}












































/*
 *将所有用IO口扩展的按键全部清除，置为高电平
 */
//void remove_key()
//{
//	West_East_Add_Button = 1;
//	Sou_Nor_Add_Button = 1;
//	SUB_TIME = 1;
//	OK = 1;
//}
/*
 *手动控制模式下：设置南北绿灯时间
 */
//uchar set_sou_nor_time()
//{
//	uchar count = 20;
//	while(1)
//	{
//		if(Sou_Nor_Add_Button==0)
//		{
//			buzzer();
//			count++;			
//		}
//		if(SUB_TIME==0)
//		{
//			buzzer();  
//			count--;
//		}
//		if(OK == 0)
//		{
//			delay(100);
//			buzzer(); 
//			break;
//		}
//		remove_key();
//		
//	}
//}
/*
 *手动控制模式下：设置东西绿灯时间
 */
//uchar set_west_east_time()
//{
//	uchar count = 20;
//	while(1)	 
//	{
//		if(West_East_Add_Button==0)
//		{
//			buzzer(); 
//			count++;
//		}
//		if(SUB_TIME==0)
//		{
//			buzzer();
//			count--;
//		}
//		if(OK == 0)
//		{
//			delay(100);
//			buzzer();
//			break;
//		}
//		remove_key();
//	}
//	return count;	
//}
