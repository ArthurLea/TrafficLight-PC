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
//ϵͳ���Զ�ģʽ��
//volatile uchar befor_stop_led_state = 0;//0��ʾ�ϱ��죬1��ʾ�ϱ��ƣ�2��ʾ�ϱ���
volatile uchar enter_stop_flag = 0;//������ť��ǣ�1��ʾ�Ѿ�����
volatile uchar car_flow_detection_flag = 0;//�Զ�ģʽ��ѭ��һ�Σ��ϱ��̵�-�ϱ��Ƶ�-�ϱ����-�ϱ��̵ƣ���
								  //��Ϊ1����⳵����
//����Ʊ�־								  
volatile uchar s_n_stop_flag = 0;//�ϱ���Ʊ�־
volatile uchar e_w_stop_flag = 0;//������Ʊ�־

volatile uchar cycle_nums = 0;/* ѭ��������Ŀ*/ 
void display()
{
	count--;
	if(count == 0) 
	{
		count = 20;
		//�ϱ��̣�������
		if(Sou_Nor_Green_Time_Temp != 0XFF)//��û����0��0-1-->0XFF
		{
			s_n_stop_flag = 0;
			e_w_stop_flag = 1;	
			numeric_led_display(1);	
			Sou_Nor_Green_Time_Temp--;
		}
		//�ϱ��ơ�������
		else if(Sou_Nor_Yellow_Time != 0XFF)
		{
			s_n_stop_flag = 1;
			e_w_stop_flag = 1;	
			numeric_led_display(2);  
			Sou_Nor_Yellow_Time--;	
		}
		//�ϱ��죬������
		else if(Sou_Nor_Red_Time_Temp != 0XFF)
		{
			s_n_stop_flag = 1;
			e_w_stop_flag = 0;	 
			numeric_led_display(3);	 
			Sou_Nor_Red_Time_Temp--;
		}
		//�ϱ��죬������
		else if(West_East_Yellow_Time != 0XFF)
		{
			s_n_stop_flag = 1;
			e_w_stop_flag = 1;
			numeric_led_display(4);
			West_East_Yellow_Time--;	
		}
		//���¼���
		if((Sou_Nor_Green_Time_Temp==0XFF) && (Sou_Nor_Yellow_Time==0XFF) && (Sou_Nor_Red_Time_Temp==0XFF) && (West_East_Yellow_Time==0XFF))
		{
			s_n_stop_flag = 0;
			e_w_stop_flag = 0;

			Sou_Nor_Green_Time_Temp =  Sou_Nor_Green_Time;
			Sou_Nor_Yellow_Time = 2;	
			West_East_Yellow_Time = 2;
			Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;

			cycle_nums++;//һ�����̵����ڴﵽ
			car_flow_detection_flag = 1;//�򿪳����������
		}
	}
}

void int0() interrupt 0//�ⲿ�ж�INT0���������⳵�ܵĳ�����120,110,119���ȣ���ֹ��������ͨ���ź�
{
	enter_stop_flag = 1;
	buzzer();			 
}
void timer0() interrupt 1//��ʱ��T0�ж�
{
	TH0 = 0X4C;//��ʽһ��Ҫ���¶Զ�ʱ����ֵ
	TL0 = 0X00;
	display();
}
void int1() interrupt 2//�ⲿ�ж�INT1
{
		
}

