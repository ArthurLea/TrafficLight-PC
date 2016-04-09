#include "config.h"
#include "led.h"
#include "max7219.h"
extern volatile uchar Sou_Nor_Yellow_Time;
extern volatile uchar West_East_Yellow_Time;
extern volatile uchar Sou_Nor_Red_Time_Temp;
extern volatile uchar Sou_Nor_Green_Time_Temp;
void quit_led()
{
	South_North_Red = 0;//熄灭
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
//初始化交通灯状态，以南北方向为准，首先为绿灯
void init_led()
{
	South_North_Red = 0;
	South_North_Yellow = 0; 
	South_North_Green = 1; 

	East_West_Red = 1;//亮	 
	East_West_Yellow = 0; 
	East_West_Green = 0;

	South_North_Footway_Red = 1;//初始化时，南北方向绿灯亮，红灯熄灭
	South_North_Footway_Green = 0;
	East_West_Footway_Red = 0;	
	East_West_Footway_Green = 1;
}
//东南西北全部为红
void all_led_red()
{
	South_North_Red = 1;//亮
	South_North_Yellow = 0; 
	South_North_Green = 0; 

	East_West_Red = 1;//亮	 
	East_West_Yellow = 0; 
	East_West_Green = 0;

	South_North_Footway_Red = 1;
	South_North_Footway_Green = 0;
	East_West_Footway_Red = 1;	
	East_West_Footway_Green = 0;
}
//数码管的显示
void numeric_led_display(uint flag)
{
	switch(flag)
	{
		case 1://南北绿灯亮时，东西方向为红灯，数码管显示
		{
			South_North_Red = 0;
			South_North_Yellow = 0;
			South_North_Green = 1;//绿灯亮 
		
			East_West_Red = 1;	 
			East_West_Yellow = 0; 
			East_West_Green = 0;

			South_North_Footway_Red = 1;//南北方向人行道绿灯亮、东西方向红灯亮
			South_North_Footway_Green = 0;
			East_West_Footway_Red = 0;	
			East_West_Footway_Green = 1;

			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
			break;
		}
		case 2://南北黄灯亮5秒时，东西方向还是红灯，数码管显示
		{
			South_North_Red = 0;
			South_North_Yellow = 1;//南北黄灯亮 
			South_North_Green = 0; 
		
			East_West_Red = 1;	 
			East_West_Yellow = 0; 
			East_West_Green = 0;

			South_North_Footway_Red = 1;//南北方向人行道绿灯亮、东西方向红灯亮
			South_North_Footway_Green = 0;
			East_West_Footway_Red = 0;	
			East_West_Footway_Green = 1;

			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Yellow_Time/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Yellow_Time%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Yellow_Time/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Yellow_Time%10]);
			break;
		}
		case 3://南北红灯亮时，东西方向为绿灯，数码管显示
		{
			South_North_Red = 1;//红灯亮
			South_North_Yellow = 0; 
			South_North_Green = 0; 
			
			East_West_Red = 0;	 
			East_West_Yellow = 0; 
			East_West_Green = 1;
									
		    South_North_Footway_Red = 0;//东西方向人行道绿灯亮、南北方向红灯亮
			South_North_Footway_Green = 1;
			East_West_Footway_Red = 1;	
			East_West_Footway_Green = 0;
			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
			break;
		}
		case 4://南北红灯亮时，东西方向为黄灯，数码管显示
		{
			South_North_Red = 1;
			South_North_Yellow = 0;
			South_North_Green = 0; 
		
			East_West_Red = 0;	 
			East_West_Yellow = 1;//东西黄灯亮  
			East_West_Green = 0;

		    South_North_Footway_Red = 0;//东西方向人行道绿灯亮、南北方向红灯亮
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