#ifndef _INFRARED_TUBE_H_
#define _INFRARED_TUBE_H_

	  #include "config.h"
	  //违规方向定义
	  #define S_TO_N 		'1'	/*南-》北*/
	  #define S_TO_W		'2' /*南-》西*/
	  #define N_TO_S		'3' /*北-》南*/
	  #define N_TO_E		'4' /*北-》东*/
	  #define E_TO_W		'5' /*东-》西*/
	  #define E_TO_S		'6' /*东-》南*/
	  #define W_TO_E		'7' /*西-》南*/
	  #define W_TO_N		'8'	/*西-》北*/

	  sbit INFRARED_W	= P2^4;//W_N
	  sbit INFRARED_N	= P2^5;//E_N
	  sbit INFRARED_E	= P2^6;//E_S
	  sbit INFRARED_S	= P2^7;//W_S	  
	  
	  extern uint run_red_light();
	  extern void car_flow_detection(uint num[]); 
#endif