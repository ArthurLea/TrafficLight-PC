#include "config.h"
#include "key.h"
#include "max7219.h"
extern volatile uchar Sou_Nor_Red_Time;
extern volatile uchar Sou_Nor_Green_Time;
extern volatile uchar Sou_Nor_Red_Time_Temp;
extern volatile uchar Sou_Nor_Green_Time_Temp;
uchar scankey()
{
	uchar num = 16;
	uchar temp = 0;
	uchar row = 0;
	uchar column = 0;
	uchar row_scan = 0XFE;//��������ɨ�裬����ɨ���ֵΪ0XFE
	uchar row_scan_temp = 0;
	P1 = 0XF0;
	temp = P1;//�����ֶ�����ʱms��������������us��
	if((temp&0XF0) != 0XF0)
	{	
		//for(i=0;i<4;row++)//�����
		while(row_scan != 0XEF)
		{
			P1 = row_scan;
			temp = P1;
			if((temp&0XF0) != 0XF0)
			{
				buzzer();//��ʱ������
				P1 = temp;
				temp = (temp&0XF0);//���ε���λ����������λ
			
				switch(temp)//�õ��к�
				{
					case 0XE0:
						column = 0;
						break;
					case 0XD0:
						column = 1;
						break;
					case 0XB0:
						column = 2;
						break;
					case 0X70:
						column = 3;
						break;	
				}

				row = (row_scan & 0X0F);
				switch(row)//�õ��к�
				{
					case 0X0E:
						row = 0;
						break;
					case 0X0D:
						row = 1;
						break;
					case 0X0B:
						row = 2;
						break;
					case 0X07:
						row = 3;
						break;	
				}
			   	num = column + row*4;
				break;
			}
			else
			{
				row_scan_temp = row_scan;
				row_scan = (row_scan<<1)|(row_scan_temp>>(8-1));//8==sizeof(uchar)
			}
		}
	}
	return num;
}
void set_time(uint flag)
{
	uchar result = 5;//ֱ��ȷ�ϣ�Ĭ�Ϸ���20��
	uchar current_key = 16;
	uint pull_key_nums = 0;
	while(1)
	{
		current_key = scankey();
		if(current_key == OK)
		{
			wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
			wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
			wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Red_Time/10]);	 
			wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Red_Time%10]);
			break;
		}
		if((0<=current_key) && (current_key<=9))
		{
			pull_key_nums++;
			switch(pull_key_nums)
			{
				case 1:
					result = current_key;
					break;
				case 2:
					result = result*10 + current_key;
				 	break;
				default://pull_key_nums>2;ʱ�����ó���100�����99��
					result = current_key;
					pull_key_nums = 1;
					break;
			}
		}
		else if(current_key == DEL)
		{
			result = 0;
			pull_key_nums = 0;	
		}
		if(flag == 1)//�ϱ�
		{
			if(result != 0)
			{
				Sou_Nor_Green_Time = result;
				Sou_Nor_Green_Time_Temp = result;
				wr_max7219(Addr_Digit0,max7219_7led_code[Sou_Nor_Green_Time_Temp/10]);	 
				wr_max7219(Addr_Digit1,max7219_7led_code[Sou_Nor_Green_Time_Temp%10]);
			}
		}
		if(flag == 2)//����
		{	 
			if(result != 0)
			{
				Sou_Nor_Red_Time = result;
				Sou_Nor_Red_Time_Temp = Sou_Nor_Red_Time;
				wr_max7219(Addr_Digit2,max7219_7led_code[Sou_Nor_Red_Time_Temp/10]);	 
				wr_max7219(Addr_Digit3,max7219_7led_code[Sou_Nor_Red_Time_Temp%10]);
			}				
		}
		current_key = 16;
	}
}