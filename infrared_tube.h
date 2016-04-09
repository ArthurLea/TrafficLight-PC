#ifndef _INFRARED_TUBE_H_
#define _INFRARED_TUBE_H_

	  #include "config.h"
	  //Υ�淽����
	  #define S_TO_N 		'1'	/*��-����*/
	  #define S_TO_W		'2' /*��-����*/
	  #define N_TO_S		'3' /*��-����*/
	  #define N_TO_E		'4' /*��-����*/
	  #define E_TO_W		'5' /*��-����*/
	  #define E_TO_S		'6' /*��-����*/
	  #define W_TO_E		'7' /*��-����*/
	  #define W_TO_N		'8'	/*��-����*/

	  sbit INFRARED_W	= P2^4;//W_N
	  sbit INFRARED_N	= P2^5;//E_N
	  sbit INFRARED_E	= P2^6;//E_S
	  sbit INFRARED_S	= P2^7;//W_S	  
	  
	  extern uint run_red_light();
	  extern void car_flow_detection(uint num[]); 
#endif