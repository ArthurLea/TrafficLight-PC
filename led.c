#include "config.h"
#include "led.h"
#include "max7219.h"
extern volatile uchar Sou_Nor_Yellow_Time;
extern volatile uchar West_East_Yellow_Time;
extern volatile uchar Sou_Nor_Red_Time_Temp;
extern volatile uchar Sou_Nor_Green_Time_Temp;
void quit_led()
{
	South_North_Red = 0;//Ϩ��
	South_North_Yellow = 0; 
	South_North_Green = 0;

	East_West_Red = 0;	 
	East_West_Yellow = 0; 
	East_West_Green = 0;
	
	South_North_Footway_Red = 0;
	South_North_Footway_Green = 0;
	East_West_Footway_Red = 0;	
	East_West_Footway_Green = 0;
}
//��ʼ����ͨ��״̬�����ϱ�����Ϊ׼������Ϊ�̵�
void init_led()
{
	South_North_Red = 0;
	South_North_Yellow = 0; 
	South_North_Green = 1; 

	East_West_Red = 1;//��	 
	East_West_Yellow = 0; 
	East_West_Green = 0;

	South_North_Footway_Red = 1;//��ʼ��ʱ���ϱ������̵��������Ϩ��
	South_North_Footway_Green = 0;
	East_West_Footway_Red = 0;	
	East_West_Footway_Green = 1;
}
//��������ȫ��Ϊ��
void all_led_red()
{
	South_North_Red = 1;//��
	South_North_Yellow = 0; 
	South_North_Green = 0; 

	East_West_Red = 1;//��	 
	East_West_Yellow = 0; 
	East_West_Green = 0;

	South_North_Footway_Red = 1;
	South_North_Footway_Green = 0;
	East_West_Footway_Red = 1;	
	East_West_Footway_Green = 0;
}
//����ܵ���ʾ
void numeric_led_display(uint flag)
{
	switch(flag)
	{
		case 1://�ϱ��̵���ʱ����������Ϊ��ƣ��������ʾ
		{
			South_North_Red = 0;
			South_North_Yellow = 0;
			South_North_Green = 1;//�̵��� 
		
			East_West_Red = 1;	 
			East_West_Yellow = 0; 
			East_West_Green = 0;

			South_North_Footway_Red = 1;//�ϱ��������е��̵�����������������
			South_North_Footway_Green = 0;
			East_West_Footway_Red = 0;	
			East_West_Footway_Green = 1;

			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
			break;
		}
		case 2://�ϱ��Ƶ���5��ʱ�����������Ǻ�ƣ��������ʾ
		{
			South_North_Red = 0;
			South_North_Yellow = 1;//�ϱ��Ƶ��� 
			South_North_Green = 0; 
		
			East_West_Red = 1;	 
			East_West_Yellow = 0; 
			East_West_Green = 0;

			South_North_Footway_Red = 1;//�ϱ��������е��̵�����������������
			South_North_Footway_Green = 0;
			East_West_Footway_Red = 0;	
			East_West_Footway_Green = 1;

			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Yellow_Time/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Yellow_Time%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Yellow_Time/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Yellow_Time%10]);
			break;
		}
		case 3://�ϱ������ʱ����������Ϊ�̵ƣ��������ʾ
		{
			South_North_Red = 1;//�����
			South_North_Yellow = 0; 
			South_North_Green = 0; 
			
			East_West_Red = 0;	 
			East_West_Yellow = 0; 
			East_West_Green = 1;
									
		    South_North_Footway_Red = 0;//�����������е��̵������ϱ���������
			South_North_Footway_Green = 1;
			East_West_Footway_Red = 1;	
			East_West_Footway_Green = 0;
			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
			break;
		}
		case 4://�ϱ������ʱ����������Ϊ�Ƶƣ��������ʾ
		{
			South_North_Red = 1;
			South_North_Yellow = 0;
			South_North_Green = 0; 
		
			East_West_Red = 0;	 
			East_West_Yellow = 1;//�����Ƶ���  
			East_West_Green = 0;

		    South_North_Footway_Red = 0;//�����������е��̵������ϱ���������
			South_North_Footway_Green = 1;
			East_West_Footway_Red = 1;	
			East_West_Footway_Green = 0;

			wr_max7219(Addr_Digit0,max7219_7led_code[West_East_Yellow_Time/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[West_East_Yellow_Time%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[West_East_Yellow_Time/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[West_East_Yellow_Time%10]);
			break;
		}
		default:
			break;
	}
}