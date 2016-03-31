#include "config.h"
#include "infrared_tube.h"

//闯红灯标志								  
extern volatile uchar s_n_stop_flag;//南北红灯标志
extern volatile uchar e_w_stop_flag;//东西红灯标志

extern volatile uchar car_flow_detection_flag; 
/*
	当某个方向为红灯时，只有向右行驶才是合法的，其他方向（向左，直行均为违规）
	如：当南北为红灯时，南->东（或者北->西）方向的红外在短时间的延时内变为低电平（0）为合法，其余违规
	    当东西为红灯时，西->南（或者东->北）方向的红外在短时间的延时内变为低电平（0）为合法，其余违规

	参数：no
		
	返回值：uint，当前哪个方向上有闯红灯车流量
*/
uint run_red_light()
{
	if(s_n_stop_flag == 1)//南北方向为红灯或者黄灯时，检测南北方向的违规车辆
	{
		if(INFRARED_S == 0)//南
		{
			delay(100);//延时100ms，表示车在这段时间中的模拟运行时间，然后到达下一个路口
			if(INFRARED_N == 0)
				return S_TO_N;
			if(INFRARED_W == 0)
				return S_TO_W;

		}
		if(INFRARED_N == 0)//北
		{
			delay(100);//延时100ms
			if(INFRARED_S == 0)
				return N_TO_S;
			if(INFRARED_E == 0)
				return N_TO_E;
		}
	}	
	if(e_w_stop_flag == 1)//东西方向为红灯或者黄灯时，检测东西方向的违规车辆
	{
		if(INFRARED_E == 0)//东
		{
			delay(100);//延时100ms
			if(INFRARED_W == 0)
                return E_TO_W;

			if(INFRARED_S == 0)
				return E_TO_S;
		}
		if(INFRARED_W == 0)//西
		{
			delay(100);//延时100ms
			if(INFRARED_E == 0)
				return W_TO_E;
			if(INFRARED_N == 0)
				return W_TO_N;
		}
	}

	return 0;//合法
}
/*
	不断检测车流量传感器传过来的数据，然后南北、东西传感器采集的数据分别计算总和然后除以2，
	分别然后赋值给car_num[2]数组，

	理论计算法：
	单位时间内通过某路段的车辆：车流量=单位时间*车速/（车距+车身长）
	模拟计算法：
	单位时间内（一个红绿灯循环周期）内通过交通十字路口某方向上的的车辆数：
		南北车流量 = (num[0] + (INFRARED_S+INFRARED_N)%2+(INFRARED_S+INFRARED_N)/2)/cycle_nums;
		东西车流量 = (num[1] + (INFRARED_E+INFRARED_W)%2+(INFRARED_E+INFRARED_W)/2)/cycle_nums;

	参数：
		num[] uchar: 存储南北东西方向上的车流量
			[0]表示南北
			[1]表示东西
	返回值：void
*/
void car_flow_detection(uint num[])
{	 
	//记录南北东西方向上的车辆数
	uchar nums_from_sou = 0;
	uchar nums_from_nor = 0;
	uchar nums_from_eas = 0;
	uchar nums_from_wes = 0;
	/*
		无论什么情况下，向右是允许的
		南-》东，
		东-》北，
		北-》西，
		西-》南，
	*/
	//s_n_stop_flag == 0
	//表示南北可以通行，此时南-》北，南-》西---向南方向有车
	//						北-》南，北-》东---向北方向有车
	if(s_n_stop_flag == 0)
	{
		if(INFRARED_S == 0)
		{
			delay(100);//延时100ms，表示车在这段时间中的模拟运行时间，然后到达下一个路口
			/* 南--》北、南--》西 */
			if((INFRARED_N==0) || (INFRARED_W==0))
				nums_from_sou++;	
		}
		if(INFRARED_N == 0)
		{
			delay(100);
			/* 北-》南，北-》东 */
			if((INFRARED_S==0) || (INFRARED_E==0))
				nums_from_nor++;	
		}
	}
	//e_w_stop_flag == 0
	//表示东西可以通行，此时东-》西，东-》南---向东方向有车
	//						西-》东，西-》北---向西方向有车
	
	if(e_w_stop_flag == 0)
	{
		if(INFRARED_E == 0)
		{
			delay(100);
			/* 东-》西，东-》南 */
			if((INFRARED_W==0) || (INFRARED_S==0))
				nums_from_eas++;	
		}
		if(INFRARED_W == 0)
		{
			delay(10);//延时10ms，如果该传感器状态不变，表示有车
			/* 西-》东，西-》北 */
			if((INFRARED_E==0) || (INFRARED_N==0))
				nums_from_wes++;	
		}
	}
	if(INFRARED_S == 0)
	{
		delay(100);
		/* 南-》东 */
		if(INFRARED_E == 0)
		{
			nums_from_sou++;
			nums_from_eas++;
		}
	}
	
	if(INFRARED_E == 0)
	{
		delay(100);
		/* 东-》北 */
		if(INFRARED_N == 0)
		{
			nums_from_eas++;
			nums_from_nor++;
		}
	}
	if(INFRARED_N == 0)
	{
		delay(100);
		/* 北-》西 */
		if(INFRARED_W == 0)
		{
			nums_from_nor++;
			nums_from_wes++;
		}
	}
	
	if(INFRARED_W == 0)
	{
		delay(100);
		/* 西-》南 */
		if(INFRARED_S == 0)
		{
			nums_from_wes++;
			nums_from_sou++;
		}
	}
	//将四个方向上的数据储存
	if((TR0==1) && (car_flow_detection_flag==0))//表示现在处于一个循环模式下,并没有进行车流量的判断
	{
		//东西南北车辆数目存储
		num[0] +=  nums_from_eas;
		num[1] +=  nums_from_sou;
		num[2] +=  nums_from_wes;
		num[3] +=  nums_from_nor;
	}
}