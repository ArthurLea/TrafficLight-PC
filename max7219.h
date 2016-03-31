#ifndef _MAX7219_H_
#define _MAX7219_H_

	//�Ĵ�����ַ�궨��
	#define Addr_No_Op 0x00 //�������Ĵ�����ַ
	#define Addr_Digit0 0x01
	#define Addr_Digit1 0x02
	#define Addr_Digit2 0x03
	#define Addr_Digit3 0x04
	#define Addr_Digit4 0x05
	#define Addr_Digit5 0x06
	#define Addr_Digit6 0x07
	#define Addr_Digit7 0x08

	#define Addr_Decode_Mode 0x09 //����ģʽ�Ĵ�����ַ
	#define Addr_Intensity 0x0a //���ȿ��ƼĴ�����ַ��max7221��
	#define Addr_Scan_Limit 0x0b //ɨ����ƼĴ�����ַ
	#define Addr_Shutdowm 0x0c //����ģʽ�Ĵ�����ַ
	#define Addr_Display_Test 0x0f //��ʾ���ԼĴ���
	
	#define ShutdownMode    0x00        // �͹��ķ�ʽ 
	#define NormalOperation 0x01        // ����������ʽ   
	#define ScanDigit       0x07        // ɨ��λ�����ã���ʾ8λ�����   
	#define DecodeDigit     0x0f        // �������ã�8λ��ΪBCD��   
	#define IntensityGrade  0x00        // ���ȼ�������  
		   
	sbit Max7219_Din = P2^0;
	sbit Max7219_Load = P2^1;
	sbit Max7219_Clk = P2^2;

	extern unsigned char code max7219_7led_code[];

	extern void wr_max7219(unsigned char addr,unsigned char dat);
	extern void init_max7219();

#endif