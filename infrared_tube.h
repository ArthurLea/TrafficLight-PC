#ifndef _INFRARED_TUBE_H_
#define _INFRARED_TUBE_H_

	  #include "config.h"
	  //Υ�淽����
	  #define S_TO_N 		0x01	/*��-����*/
	  #define S_TO_W		0x02    /*��-����*/
	  #define N_TO_S		0x03    /*��-����*/
	  #define N_TO_E		0x04    /*��-����*/
	  #define E_TO_W		0x05    /*��-����*/
	  #define E_TO_S		0x06    /*��-����*/
	  #define W_TO_E		0x07    /*��-����*/
	  #define W_TO_N		0x08	/*��-����*/

	  sbit INFRARED_W	= P2^4;//W_N
	  sbit INFRARED_N	= P2^5;//E_N
	  sbit INFRARED_E	= P2^6;//E_S
	  sbit INFRARED_S	= P2^7;//W_S	  
	  
	  extern uint run_red_light();
	  extern void car_flow_detection(uint num[]); 
#endif