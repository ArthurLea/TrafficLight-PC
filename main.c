#include <reg52.h>
#include "config.h"
#include "max7219.h"
#include "led.h"
#include "key.h"
#include "uart.h"
#include "infrared_tube.h"

//������Ҫ����
volatile uchar uartState = UART_IDLE;
volatile uint timeout;
volatile uint pointer;

volatile uchar g_cCommand; 
volatile uchar send_Buf[16] = {0};//ͨ������ת�������͸��ֻ��Ļ�����
volatile uchar recieve_Buf[16] = {0};//���ݽ��յĻ�����
/*�Զ�ģʽ,Ĭ�ϸ������Ϊ20s,ͬʱ��ʼ���ϱ�����Ϊ׼���ϱ�Ϊ�̣�����Ϊ��
 *�ֶ�ģʽ,�������Լ������ϱ����������ϵ��̵�ʱ��
 *ע�⣺�ϱ������ϵ��̵�ʱ��Ͷ��������ϵĺ��ʱ����ͬ
 *      ���������ϵ��̵�ʱ����ϱ������ϵĺ��ʱ����ͬ
 */
//���̻Ƶ�ʱ��ȫ���ױ����
volatile uchar Sou_Nor_Red_Time = 5;
volatile uchar Sou_Nor_Green_Time = 5;
volatile uchar Sou_Nor_Yellow_Time = 2;
volatile uchar West_East_Yellow_Time = 2;

//���̵�ʱʱ�䱸��
volatile uchar Sou_Nor_Red_Time_Temp;
volatile uchar Sou_Nor_Green_Time_Temp;

uchar code big_time = 10;//��ʱ��
uchar code small_time = 5;//��ʱ��

volatile uchar Phone_OK_Finish = 0;//�ֻ�������ϱ��
//�ⲿ�������
extern uchar enter_stop_flag;//��ֹͨ�б��
volatile uchar quit_stop_flag = 0;
extern uchar car_flow_detection_flag;
//���������
volatile uint car_num[4] = {0,0,0,0};//0-�������ϵĳ�����
										//1-�Ϸ���
										//2-������
										//3-������
extern volatile uchar cycle_nums;//from interrupt.c file   

//��������
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
	uchar key_num = 17;/*��ǰ����ֵ*/
	uint run_red_light_result = 0;/*�����̵ƵĽ��*/
	uint sou_nor_nums = 0;/* �ϱ������ϵĳ������ܺ�*/
	uint eas_wes_nums = 0;/* ���������ϵĳ������ܺ�*/
	uint cycle_time = 0;/* ѭ�����ڵ�ʱ��*/
	init_register_s52();
	init_max7219();
	quit_led();
	buzzer();
	send_reset_to_phone('9'); //��λ����λ
	while(1)
	{	 
		/*ϵͳ��ʼ���ϵͳģʽ����OK�������м��Լ��ֻ�����֮ǰ����ʱΪ�Զ�ģʽ,ÿ�������ʱ20s
		 *						����ͨ��������ʽ���������������ֻ��ˣ����ú��ٴΰ���ϵͳ����
		 *
		 *��ʼ��ⰴ�����ֻ�Զ������
		 */
		key_num = scankey();
		if((key_num==OK) || (Phone_OK_Finish==1))//������ֱ�ӿ��ƣ�ϵͳ��ʽ���� 
		{
			Phone_OK_Finish = 0;
			init_led();
			init_Numeric_Display();
			clearFlag();
			TR0 = 1;	

			cycle_time = Sou_Nor_Green_Time + Sou_Nor_Red_Time + Sou_Nor_Yellow_Time + West_East_Yellow_Time;		
			send_cycle_time_to_phone('8',cycle_time);//����ѭ��ʱ��		  
			while(1)
			{
				
				/*
				 * �ֻ���Զ�̿���ģ��
				 */
				phone_remote_control();
				/*
				 *�������ģ��
				 */
				if(enter_stop_flag == 1)//����������
				{	
					TR0 = 0;//��ͣ�󣬹رն�ʱ��
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
							TR0 = 1;//�ٴ�������ʱ��
						}
					}
				}
				/*
				 * ����Ƽ��ģ��
				 */
				 run_red_light_result = run_red_light();
				 if(run_red_light_result != 0)
				 {
					buzzer();
					//������ƾ��巽λ����ͨ������ת�������͵��ֻ��ն�
					//����CRC��װ
					send_run_red_light_to_phone('4',run_red_light_result);
					run_red_light_result = 0;
				 }
				/*
				 * �������ģ��
				 */
				//�õ�ÿ�������ϵĳ��������浽car_num������
				car_flow_detection(car_num);
				//�����ϱ������������ϵĳ���������ƽ��ֵ��ģ��ȡ������ֵ
				sou_nor_nums = (car_num[1]+car_num[3])/cycle_nums;
				eas_wes_nums = (car_num[0]+car_num[2])/cycle_nums;
				//����ѭ�����ڵ�ʱ���
				cycle_time = Sou_Nor_Green_Time + Sou_Nor_Red_Time + Sou_Nor_Yellow_Time + West_East_Yellow_Time;
				//���㳵����
				//??????????
				//�����������ϵĳ���������ͨ������ת�������͵��ֻ��ն�
				//����CRC��װ
				//sou_nor_nums,eas_wes_nums,cycle_nums��cycle_time
				
				if(car_flow_detection_flag == 1)
				{ 
					//һ��ѭ�����ڷ���һ�γ�����״̬
					send_sou_nor_nums_to_phone('5',sou_nor_nums);
					delay(100);
					send_eas_wes_nums_to_phone('6',eas_wes_nums); 
					delay(100);
					send_cycle_nums_to_phone('7',cycle_nums);  	
					delay(100);
					send_cycle_time_to_phone('8',cycle_time);//����ѭ��ʱ��	
					//ģ��ı��ϱ����������ϵĺ��̵�ʱ��
					if(Sou_Nor_Green_Time <= small_time)
					{
					if(sou_nor_nums > 10)//10��
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
					//����������
					
					//ģ��ı䶫�����������ϵĺ��̵�ʱ
					if(Sou_Nor_Red_Time <= small_time)
					{
					if(eas_wes_nums > 10)//10��
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
					
					car_flow_detection_flag = 0;//�Զ�ѭ��ģʽ��Ǽ�⴦��������ê0	
				}
				if((sou_nor_nums>999) || (eas_wes_nums>999))//���ó��������ݣ�����ѭ����������Ϊ0
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
		 *	ʱ���ֶ�����ģ��
		 *  1����λ������ģ��
		 *  2���ֻ�����������ģ��
		 */
		//OK��û�а��£���ʱ���û��Լ����ø�����ĺ���̵�ʱ��,�޸ĵ��Ƕ�Ӧ�����ϵ��̵�ʱ��
		//1����λ������ģ��
		if(key_num == SET_SOU_NOR_TIME)//�����ϱ������̵�ʱ��
		{
			wr_max7219(Addr_Digit2,0x00);	 
			wr_max7219(Addr_Digit3,0x00);	
			set_time(1);//�ϱ�	
		}
		if(key_num == SET_WEST_EAST_TIME)
		{
			wr_max7219(Addr_Digit0,0x00);	 
			wr_max7219(Addr_Digit1,0x00);				
			set_time(2);
		}
		//2���ֻ�����������ģ��
		phone_remote_control();
	}
	return 0;
}

/*
 * �ֻ���
 */
void phone_remote_control()	
{
	uint i;
	switch(uartState)
	{
		case UART_IDLE://����δ������״̬
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
			if ((timeout>0x8000) || (recieve_Buf[(pointer-1)]=='F') || (pointer>7) || (pointer >= (recieve_Buf[1])-'0'))//[0]��ʾ���ݳ���
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
			//���ֻ��˽��յ����ַ���ת��Ϊ16����
			uartState = UART_IDLE; 

			for(i=0; i<pointer;i++)
			{
				sendByte(recieve_Buf[i]);
				delay(1);
			}

	  		g_cCommand = recieve_Buf[2] ; //ȡ������������
//					i =	g_cReceBuf[0]-2;
//					CRCdata = CRC16(g_cReceBuf,i);//����CRCѭ��У��

			if((timeout<0x8000) && (recieve_Buf[0]=='$')/* && (CRCdata==(u16)(g_cReceBuf[i]<<8)+g_cReceBuf[i+1])*/ )
			{
				switch(g_cCommand)
				{
					
					case '0'://�ֻ�ȷ���������
						Phone_OK_Finish = 1; 
						buzzer();	  
						break;
					case '1'://����ʱ��
					{
						if(recieve_Buf[3] == '1')//�����ϱ��̵�ʱ��
						{						
							Sou_Nor_Green_Time = (recieve_Buf[4] - '0')*10 + (recieve_Buf[5] - '0');//�ַ���ASCII
							Sou_Nor_Green_Time_Temp =  Sou_Nor_Green_Time;
							wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
							wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
						}
						if(recieve_Buf[3] == '2')//���ö����̵�ʱ��
						{							 
							Sou_Nor_Red_Time = (recieve_Buf[4] - '0')*10 + (recieve_Buf[5] - '0');
							Sou_Nor_Red_Time_Temp =  Sou_Nor_Red_Time;
							wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
							wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
						}
						buzzer();	
						break;
					}
					case '2'://��ֹͨ��
						enter_stop_flag = 1;
						buzzer();
						break;
					case '3'://����ͨ��
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
 *��ʼ��52��Ƭ���ļĴ������ⲿ�жϣ���ʱ�����Լ��������ȼ���
 */
void init_register_s52()
{	
	//CPU��ѯ�жϵ�˳��INT0->T0->INT1->TI/RI->T2��
	IE = 0X97;//����Ӧ�ж�((EA(�ܿ���)-1))--(����-0)--(T2-0)--(���п�-1)
						 //--(T1-0)--(INT1-1)--(T0-1)--(INT0-1))
	TMOD = 0X21;//T1��ʽ2��T0��ʽ1���ã�GATE-C/T-M1-M0--GATE-C/T-M1-M0��
	IT0 = 1;//INT0�½�����Ч
	IT1 = 1;//�½�����Ч
	TL1 = 0XFD;
	TH0 = 0X4C;//��ʱ��
	TL0 = 0X00;
	//TR0 = 1;//������ʱ��
	
	TH1 = 0XFD;//������9600,SMOD���ӱ�
	TL1 = 0XFD;
	TR1 = 1;//����T1
	SCON = 0X50;//���ڷ�ʽ����ʽ1���������

	PS = 1;	//���п����ȼ�
	PX0 = 1; 
	PX1 = 1;//INT1�ж����ȼ�
	//IP = 0X21;//(����-����-T2-���п�-T1-INT1-T0-INT0)
	Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;
	Sou_Nor_Green_Time_Temp = Sou_Nor_Green_Time;
}

/*
 *��ʼ���������ʾ����,���ϱ�Ϊ��׼��ʾ��Ҳ������ʾ���ǵ�ǰ�ϱ�ͨ��ʱ�̵�ʱ��
 */
void init_Numeric_Display()
{
	wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
	wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
	wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
	wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
}












































/*
 *��������IO����չ�İ���ȫ���������Ϊ�ߵ�ƽ
 */
//void remove_key()
//{
//	West_East_Add_Button = 1;
//	Sou_Nor_Add_Button = 1;
//	SUB_TIME = 1;
//	OK = 1;
//}
/*
 *�ֶ�����ģʽ�£������ϱ��̵�ʱ��
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
 *�ֶ�����ģʽ�£����ö����̵�ʱ��
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
