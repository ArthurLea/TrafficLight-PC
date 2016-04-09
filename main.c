#include <reg52.h>
#include "config.h"
#include "max7219.h"
#include "led.h"
#include "key.h"
#include "uart.h"
#include "infrared_tube.h"

//´®¿ÚĞèÒª±äÁ¿
volatile uchar uartState = UART_IDLE;
volatile uint timeout;
volatile uint pointer;

volatile uchar g_cCommand; 
volatile uchar send_Buf[16] = {0};//Í¨¹ı´®¿Ú×ªÀ¶ÑÀ·¢ËÍ¸øÊÖ»úµÄ»º³åÇø
volatile uchar recieve_Buf[16] = {0};//Êı¾İ½ÓÊÕµÄ»º³åÇø
/*×Ô¶¯Ä£Ê½,Ä¬ÈÏ¸÷·½Ïò¾ùÎª20s,Í¬Ê±¿ªÊ¼ÒÔÄÏ±±·½ÏòÎª×¼£¬ÄÏ±±ÎªÂÌ£¬¶«Î÷Îªºì
 *ÊÖ¶¯Ä£Ê½,²Ù×÷Õß×Ô¼ºÉèÖÃÄÏ±±¶«Î÷·½ÏòÉÏµÄÂÌµÆÊ±¼ä
 *×¢Òâ£ºÄÏ±±·½ÏòÉÏµÄÂÌµÆÊ±¼äºÍ¶«Î÷·½ÏòÉÏµÄºìµÆÊ±¼äÏàÍ¬
 *      ¶«Î÷·½ÏòÉÏµÄÂÌµÆÊ±¼äºÍÄÏ±±·½ÏòÉÏµÄºìµÆÊ±¼äÏàÍ¬
 */
//ºìÂÌ»ÆµÆÊ±¼äÈ«¾ÖÒ×±ä±äÁ¿
volatile uchar Sou_Nor_Red_Time = 5;
volatile uchar Sou_Nor_Green_Time = 5;
volatile uchar Sou_Nor_Yellow_Time = 2;
volatile uchar West_East_Yellow_Time = 2;

//ºìÂÌµÆÊ±Ê±¼ä±¸·İ
volatile uchar Sou_Nor_Red_Time_Temp;
volatile uchar Sou_Nor_Green_Time_Temp;

uchar code big_time = 10;//³¤Ê±¼ä
uchar code small_time = 5;//¶ÌÊ±¼ä

volatile uchar Phone_OK_Finish = 0;//ÊÖ»úÉèÖÃÍê±Ï±ê¼Ç
//Íâ²¿¹²Ïí±äÁ¿
extern uchar enter_stop_flag;//½ûÖ¹Í¨ĞĞ±ê¼Ç
volatile uchar quit_stop_flag = 0;
extern uchar car_flow_detection_flag;
//³µÁ÷Á¿¼ì²â
volatile uint car_num[4] = {0,0,0,0};//0-¶«·½ÏòÉÏµÄ³µÁ÷Á¿
										//1-ÄÏ·½Ïò
										//2-Î÷·½Ïò
										//3-±±·½Ïò
extern volatile uchar cycle_nums;//from interrupt.c file   

//º¯ÊıÉùÃ÷
void init_register_s52();
void init_Numeric_Display();
void phone_remote_set();
void phone_remote_control();

void clearFlag()
{
	Phone_OK_Finish = 0;
	enter_stop_flag	= 0;
	quit_stop_flag = 0;
	car_flow_detection_flag = 0;	
}
int main()
{
	uchar key_num = 17;/*µ±Ç°°´¼üÖµ*/
	uint run_red_light_result = 0;/*´³ºìÂÌµÆµÄ½á¹û*/
	uint sou_nor_nums = 0;/* ÄÏ±±·½ÏòÉÏµÄ³µÁ÷Á¿×ÜºÍ*/
	uint eas_wes_nums = 0;/* ¶«Î÷·½ÏòÉÏµÄ³µÁ÷Á¿×ÜºÍ*/
	uint cycle_time = 0;/* Ñ­»·ÖÜÆÚµÄÊ±¼ä*/
	init_register_s52();
	init_max7219();
	quit_led();
	buzzer();
	send_reset_to_phone('9'); //ÏÂÎ»»ú¸´Î»
	while(1)
	{	 
		/*ÏµÍ³¿ªÊ¼¼ì²âÏµÍ³Ä£Ê½£¬µ±OK¼üÔÚËùÓĞ¼üÒÔ¼°ÊÖ»ú¿ØÖÆÖ®Ç°°´ÏÂÊ±Îª×Ô¶¯Ä£Ê½,Ã¿¸ö·½Ïò¼ÆÊ±20s
		 *						·ñÔòÍ¨¹ıÆäËû·½Ê½£¨ÆäËû°´¼ü»òÕßÊÖ»ú¶Ë£©ÉèÖÃºóÔÙ´Î°´ÏÂÏµÍ³Æô¶¯
		 *
		 *¿ªÊ¼¼ì²â°´¼ü»òÊÖ»úÔ¶³ÌÉèÖÃ
		 */
		key_num = scankey();
		if((key_num==OK) || (Phone_OK_Finish==1))//°´¼ü¶ËÖ±½Ó¿ØÖÆ£¬ÏµÍ³ÕıÊ½Æô¶¯ 
		{
			Phone_OK_Finish = 0;
			init_led();
			init_Numeric_Display();
			clearFlag();
			TR0 = 1;	

			cycle_time = Sou_Nor_Green_Time + Sou_Nor_Red_Time + Sou_Nor_Yellow_Time + West_East_Yellow_Time;		
			send_cycle_time_to_phone('8',cycle_time);//·¢ËÍÑ­»·Ê±¼ä		  
			while(1)
			{
				
				/*
				 * ÊÖ»ú¶ËÔ¶³Ì¿ØÖÆÄ£¿é
				 */
				phone_remote_control();
				/*
				 *½ô¼±Çé¿öÄ£¿é
				 */
				if(enter_stop_flag == 1)//½ô¼±Çé¿ö±ê¼Ç
				{	
					TR0 = 0;//½ûÍ£ºó£¬¹Ø±Õ¶¨Ê±Æ÷
					buzzer();			  
					all_led_red();
					while(enter_stop_flag)
					{
						key_num = scankey();
						phone_remote_control();
						if((OK==key_num) || (Phone_OK_Finish == 1) || (quit_stop_flag==1))
						{
							enter_stop_flag = 0;
							Phone_OK_Finish = 0;		 
							quit_stop_flag = 0;
							TR0 = 1;//ÔÙ´ÎÆô¶¯¶¨Ê±Æ÷
						}
					}
				}
				/*
				 * ´³ºìµÆ¼ì²âÄ£¿é
				 */
				 run_red_light_result = run_red_light();
				 if(run_red_light_result != 0)
				 {
					buzzer();
					//½«´³ºìµÆ¾ßÌå·½Î»Êı¾İÍ¨¹ı´®¿Ú×ªÀ¶ÑÀ´«ËÍµ½ÊÖ»úÖÕ¶Ë
					//Êı¾İCRC·â×°
					send_run_red_light_to_phone('4',run_red_light_result);
					run_red_light_result = 0;
				 }
				/*
				 * ³µÁ¾¼ì²âÄ£¿é
				 */
				//µÃµ½Ã¿¸ö·½ÏòÉÏµÄ³µÁ÷Á¿´¢´æµ½car_numÊı×éÖĞ
				car_flow_detection(car_num);
				//¼ÆËãÄÏ±±¡¢¶«Î÷·½ÏòÉÏµÄ³µÁ¾ÊıËãÊıÆ½¾ùÖµ£¬Ä£ÄâÈ¡³öÕûÊıÖµ
				sou_nor_nums = (car_num[1]+car_num[3])/cycle_nums;
				eas_wes_nums = (car_num[0]+car_num[2])/cycle_nums;
				//¼ÆËãÑ­»·ÖÜÆÚµÄÊ±¼äºÍ
				cycle_time = Sou_Nor_Green_Time + Sou_Nor_Red_Time + Sou_Nor_Yellow_Time + West_East_Yellow_Time;
				//¼ÆËã³µÁ÷Á¿
				//??????????
				//½«Á½¸ö·½ÏòÉÏµÄ³µÁ÷Á¿Êı¾İÍ¨¹ı´®¿Ú×ªÀ¶ÑÀ´«ËÍµ½ÊÖ»úÖÕ¶Ë
				//Êı¾İCRC·â×°
				//sou_nor_nums,eas_wes_nums,cycle_nums£¬cycle_time
				
				if(car_flow_detection_flag == 1)
				{ 
					//Ò»¸öÑ­»·ÖÜÆÚ·¢ËÍÒ»´Î³µÁ÷Á¿×´Ì¬
					send_sou_nor_nums_to_phone('5',sou_nor_nums);
					delay(100);
					send_eas_wes_nums_to_phone('6',eas_wes_nums); 
					delay(100);
					send_cycle_nums_to_phone('7',cycle_nums);  	
					delay(100);
					send_cycle_time_to_phone('8',cycle_time);//·¢ËÍÑ­»·Ê±¼ä	
					//Ä£Äâ¸Ä±äÄÏ±±¶«Î÷·½ÏòÉÏµÄºìÂÌµÆÊ±¼ä
					if(Sou_Nor_Green_Time <= small_time)
					{
					if(sou_nor_nums > 10)//10Á¾
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
					//ÆäËûµÄÕı³£
					
					//Ä£Äâ¸Ä±ä¶«Î÷¶«Î÷·½ÏòÉÏµÄºìÂÌµÆÊ±
					if(Sou_Nor_Red_Time <= small_time)
					{
					if(eas_wes_nums > 10)//10Á¾
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
					
					car_flow_detection_flag = 0;//×Ô¶¯Ñ­»·Ä£Ê½±ê¼Ç¼ì²â´¦Àí½áÊø£¬ÖÃª0	
				}
				if((sou_nor_nums>999) || (eas_wes_nums>999))//ÖØÖÃ³µÁ÷Á¿Êı¾İ£¬²¢½«Ñ­»·´ÎÊıÉèÖÃÎª0
				{
					car_num[0] = 0;
					car_num[1] = 0;
					car_num[2] = 0;
					car_num[3] = 0;
					cycle_nums  = 0;
				}
			}
		}
		/*
		 *	Ê±¼äÊÖ¶¯ÉèÖÃÄ£¿é
		 *  1¡¢ÏÂÎ»»ú°´¼üÄ£¿é
		 *  2¡¢ÊÖ»ú¶ËÃüÁîÉèÖÃÄ£¿é
		 */
		//OK¼üÃ»ÓĞ°´ÏÂ£¬´ËÊ±ÊÇÓÃ»§×Ô¼ºÉèÖÃ¸÷·½ÏòµÄºì»ÆÂÌµÆÊ±¼ä,ĞŞ¸ÄµÄÊÇ¶ÔÓ¦·½ÏòÉÏµÄÂÌµÆÊ±¼ä
		//1¡¢ÏÂÎ»»ú°´¼üÄ£¿é
		if(key_num == SET_SOU_NOR_TIME)//ÉèÖÃÄÏ±±·½ÏòÂÌµÆÊ±¼ä
		{
			wr_max7219(Addr_Digit2,0x00);	 
			wr_max7219(Addr_Digit3,0x00);	
			set_time(1);//ÄÏ±±	
		}
		if(key_num == SET_WEST_EAST_TIME)
		{
			wr_max7219(Addr_Digit0,0x00);	 
			wr_max7219(Addr_Digit1,0x00);				
			set_time(2);
		}
		//2¡¢ÊÖ»ú¶ËÃüÁîÉèÖÃÄ£¿é
		phone_remote_control();
	}
	return 0;
}

/*
 * ÊÖ»ú¶Ë
 */
void phone_remote_control()	
{
	uint i;
	switch(uartState)
	{
		case UART_IDLE://´®¿ÚÎ´ÓĞÊı¾İ×´Ì¬
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
			if ((timeout>0x8000) || (recieve_Buf[(pointer-1)]=='F') || (pointer>7) || (pointer >= (recieve_Buf[1])-'0'))//[0]±íÊ¾Êı¾İ³¤¶È
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
			//½«ÊÖ»ú¶Ë½ÓÊÕµ½µÄ×Ö·û´®×ª»»Îª16½øÖÆ
			uartState = UART_IDLE; 

			for(i=0; i<pointer;i++)
			{
				sendByte(recieve_Buf[i]);
				delay(1);
			}

	  		g_cCommand = recieve_Buf[2] ; //È¡³ö¹¦ÄÜÃüÁî×Ö
//					i =	g_cReceBuf[0]-2;
//					CRCdata = CRC16(g_cReceBuf,i);//½øĞĞCRCÑ­»·Ğ£Ñé

			if((timeout<0x8000) && (recieve_Buf[0]=='$')/* && (CRCdata==(u16)(g_cReceBuf[i]<<8)+g_cReceBuf[i+1])*/ )
			{
				switch(g_cCommand)
				{
					
					case '0'://ÊÖ»úÈ·ÈÏÉèÖÃÍê±Ï
						Phone_OK_Finish = 1; 
						buzzer();	  
						break;
					case '1'://ÉèÖÃÊ±¼ä
					{
						if(recieve_Buf[3] == '1')//ÉèÖÃÄÏ±±ÂÌµÆÊ±¼ä
						{						
							Sou_Nor_Green_Time = (recieve_Buf[4] - '0')*10 + (recieve_Buf[5] - '0');//×Ö·ûµÄASCII
							Sou_Nor_Green_Time_Temp =  Sou_Nor_Green_Time;
							wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
							wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
						}
						if(recieve_Buf[3] == '2')//ÉèÖÃ¶«Î÷ÂÌµÆÊ±¼ä
						{							 
							Sou_Nor_Red_Time = (recieve_Buf[4] - '0')*10 + (recieve_Buf[5] - '0');
							Sou_Nor_Red_Time_Temp =  Sou_Nor_Red_Time;
							wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
							wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
						}
						buzzer();	
						break;
					}
					case '2'://½ûÖ¹Í¨ĞĞ
						enter_stop_flag = 1;
						buzzer();
						break;
					case '3'://¿ªÆôÍ¨ĞĞ
						quit_stop_flag = 1;
						buzzer();
						break;
					default:
						break;
				}
			}
			break;
		}
	} 
}
/*
 *³õÊ¼»¯52µ¥Æ¬»úµÄ¼Ä´æÆ÷£¬Íâ²¿ÖĞ¶Ï£¬¶¨Ê±Æ÷£¬ÒÔ¼°ÏàÒÀÓÅÏÈ¼¶µÈ
 */
void init_register_s52()
{	
	//CPU²éÑ¯ÖĞ¶ÏµÄË³Ğò£¨INT0->T0->INT1->TI/RI->T2£©
	IE = 0X97;//´ò¿ªÏàÓ¦ÖĞ¶Ï((EA(×Ü¿ª¹Ø)-1))--(²»ÓÃ-0)--(T2-0)--(´®ĞĞ¿Ú-1)
						 //--(T1-0)--(INT1-1)--(T0-1)--(INT0-1))
	TMOD = 0X21;//T1·½Ê½2£¬T0·½Ê½1ÉèÖÃ£¨GATE-C/T-M1-M0--GATE-C/T-M1-M0£©
	IT0 = 1;//INT0ÏÂ½µÑØÓĞĞ§
	IT1 = 1;//ÏÂ½µÑØÓĞĞ§
	TL1 = 0XFD;
	TH0 = 0X4C;//¶¨Ê±Æ÷
	TL0 = 0X00;
	//TR0 = 1;//Æô¶¯¶¨Ê±Æ÷
	
	TH1 = 0XFD;//²¨ÌØÂÊ9600,SMOD²»¼Ó±¶
	TL1 = 0XFD;
	TR1 = 1;//Æô¶¯T1
	SCON = 0X50;//´®¿Ú·½Ê½£¬·½Ê½1£¬ÔÊĞí½ÓÊÕ

	PS = 1;	//´®ĞĞ¿ÚÓÅÏÈ¼¶
	PX0 = 1; 
	PX1 = 1;//INT1ÖĞ¶ÏÓÅÏÈ¼¶
	//IP = 0X21;//(²»ÓÃ-²»ÓÃ-T2-´®ĞĞ¿Ú-T1-INT1-T0-INT0)
	Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;
	Sou_Nor_Green_Time_Temp = Sou_Nor_Green_Time;
}

/*
 *³õÊ¼»¯ÊıÂë¹ÜÏÔÊ¾Êı×Ö,ÒÔÄÏ±±Îª»ù×¼ÏÔÊ¾£¬Ò²¾ÍÊÇÏÔÊ¾µÄÊÇµ±Ç°ÄÏ±±Í¨ĞĞÊ±ÂÌµÆÊ±¼ä
 */
void init_Numeric_Display()
{
	wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
	wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
	wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
	wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
}












































/*
 *½«ËùÓĞÓÃIO¿ÚÀ©Õ¹µÄ°´¼üÈ«²¿Çå³ı£¬ÖÃÎª¸ßµçÆ½
 */
//void remove_key()
//{
//	West_East_Add_Button = 1;
//	Sou_Nor_Add_Button = 1;
//	SUB_TIME = 1;
//	OK = 1;
//}
/*
 *ÊÖ¶¯¿ØÖÆÄ£Ê½ÏÂ£ºÉèÖÃÄÏ±±ÂÌµÆÊ±¼ä
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
 *ÊÖ¶¯¿ØÖÆÄ£Ê½ÏÂ£ºÉèÖÃ¶«Î÷ÂÌµÆÊ±¼ä
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
