#ifndef _MAX7219_H_
#define _MAX7219_H_

	//寄存器地址宏定义
	#define Addr_No_Op 0x00 //不工作寄存器地址
	#define Addr_Digit0 0x01
	#define Addr_Digit1 0x02
	#define Addr_Digit2 0x03
	#define Addr_Digit3 0x04
	#define Addr_Digit4 0x05
	#define Addr_Digit5 0x06
	#define Addr_Digit6 0x07
	#define Addr_Digit7 0x08

	#define Addr_Decode_Mode 0x09 //译码模式寄存器地址
	#define Addr_Intensity 0x0a //亮度控制寄存器地址（max7221）
	#define Addr_Scan_Limit 0x0b //扫描控制寄存器地址
	#define Addr_Shutdowm 0x0c //掉电模式寄存器地址
	#define Addr_Display_Test 0x0f //显示测试寄存器
	
	#define ShutdownMode    0x00        // 低功耗方式 
	#define NormalOperation 0x01        // 正常操作方式   
	#define ScanDigit       0x07        // 扫描位数设置，显示8位数码管   
	#define DecodeDigit     0x0f        // 译码设置，8位均为BCD码   
	#define IntensityGrade  0x00        // 亮度级别设置  
		   
	sbit Max7219_Din = P2^0;
	sbit Max7219_Load = P2^1;
	sbit Max7219_Clk = P2^2;

	extern unsigned char code max7219_7led_code[];

	extern void wr_max7219(unsigned char addr,unsigned char dat);
	extern void init_max7219();

#endif