#include "config.h"
#include "infrared_tube.h"

//����Ʊ�־								  
extern volatile uchar s_n_stop_flag;//�ϱ���Ʊ�־
extern volatile uchar e_w_stop_flag;//������Ʊ�־

extern volatile uchar car_flow_detection_flag; 
/*
	��ĳ������Ϊ���ʱ��ֻ��������ʻ���ǺϷ��ģ�������������ֱ�о�ΪΥ�棩
	�磺���ϱ�Ϊ���ʱ����->�������߱�->��������ĺ����ڶ�ʱ�����ʱ�ڱ�Ϊ�͵�ƽ��0��Ϊ�Ϸ�������Υ��
	    ������Ϊ���ʱ����->�ϣ����߶�->��������ĺ����ڶ�ʱ�����ʱ�ڱ�Ϊ�͵�ƽ��0��Ϊ�Ϸ�������Υ��

	������no
		
	����ֵ��uint����ǰ�ĸ��������д���Ƴ�����
*/
uint run_red_light()
{
	if(s_n_stop_flag == 1)//�ϱ�����Ϊ��ƻ��߻Ƶ�ʱ������ϱ������Υ�泵��
	{
		if(INFRARED_S == 0)//��
		{
			delay(100);//��ʱ100ms����ʾ�������ʱ���е�ģ������ʱ�䣬Ȼ�󵽴���һ��·��
			if(INFRARED_N == 0)
				return S_TO_N;
			if(INFRARED_W == 0)
				return S_TO_W;

		}
		if(INFRARED_N == 0)//��
		{
			delay(100);//��ʱ100ms
			if(INFRARED_S == 0)
				return N_TO_S;
			if(INFRARED_E == 0)
				return N_TO_E;
		}
	}	
	if(e_w_stop_flag == 1)//��������Ϊ��ƻ��߻Ƶ�ʱ����ⶫ�������Υ�泵��
	{
		if(INFRARED_E == 0)//��
		{
			delay(100);//��ʱ100ms
			if(INFRARED_W == 0)
                return E_TO_W;

			if(INFRARED_S == 0)
				return E_TO_S;
		}
		if(INFRARED_W == 0)//��
		{
			delay(100);//��ʱ100ms
			if(INFRARED_E == 0)
				return W_TO_E;
			if(INFRARED_N == 0)
				return W_TO_N;
		}
	}

	return 0;//�Ϸ�
}
/*
	���ϼ�⳵���������������������ݣ�Ȼ���ϱ��������������ɼ������ݷֱ�����ܺ�Ȼ�����2��
	�ֱ�Ȼ��ֵ��car_num[2]���飬

	���ۼ��㷨��
	��λʱ����ͨ��ĳ·�εĳ�����������=��λʱ��*����/������+������
	ģ����㷨��
	��λʱ���ڣ�һ�����̵�ѭ�����ڣ���ͨ����ͨʮ��·��ĳ�����ϵĵĳ�������
		�ϱ������� = (num[0] + (INFRARED_S+INFRARED_N)%2+(INFRARED_S+INFRARED_N)/2)/cycle_nums;
		���������� = (num[1] + (INFRARED_E+INFRARED_W)%2+(INFRARED_E+INFRARED_W)/2)/cycle_nums;

	������
		num[] uchar: �洢�ϱ����������ϵĳ�����
			[0]��ʾ�ϱ�
			[1]��ʾ����
	����ֵ��void
*/
void car_flow_detection(uint num[])
{	 
	//��¼�ϱ����������ϵĳ�����
	uchar nums_from_sou = 0;
	uchar nums_from_nor = 0;
	uchar nums_from_eas = 0;
	uchar nums_from_wes = 0;
	/*
		����ʲô����£������������
		��-������
		��-������
		��-������
		��-���ϣ�
	*/
	//s_n_stop_flag == 0
	//��ʾ�ϱ�����ͨ�У���ʱ��-��������-����---���Ϸ����г�
	//						��-���ϣ���-����---�򱱷����г�
	if(s_n_stop_flag == 0)
	{
		if(INFRARED_S == 0)
		{
			delay(100);//��ʱ100ms����ʾ�������ʱ���е�ģ������ʱ�䣬Ȼ�󵽴���һ��·��
			/* ��--��������--���� */
			if((INFRARED_N==0) || (INFRARED_W==0))
				nums_from_sou++;	
		}
		if(INFRARED_N == 0)
		{
			delay(100);
			/* ��-���ϣ���-���� */
			if((INFRARED_S==0) || (INFRARED_E==0))
				nums_from_nor++;	
		}
	}
	//e_w_stop_flag == 0
	//��ʾ��������ͨ�У���ʱ��-��������-����---�򶫷����г�
	//						��-��������-����---���������г�
	
	if(e_w_stop_flag == 0)
	{
		if(INFRARED_E == 0)
		{
			delay(100);
			/* ��-��������-���� */
			if((INFRARED_W==0) || (INFRARED_S==0))
				nums_from_eas++;	
		}
		if(INFRARED_W == 0)
		{
			delay(10);//��ʱ10ms������ô�����״̬���䣬��ʾ�г�
			/* ��-��������-���� */
			if((INFRARED_E==0) || (INFRARED_N==0))
				nums_from_wes++;	
		}
	}
	if(INFRARED_S == 0)
	{
		delay(100);
		/* ��-���� */
		if(INFRARED_E == 0)
		{
			nums_from_sou++;
			nums_from_eas++;
		}
	}
	
	if(INFRARED_E == 0)
	{
		delay(100);
		/* ��-���� */
		if(INFRARED_N == 0)
		{
			nums_from_eas++;
			nums_from_nor++;
		}
	}
	if(INFRARED_N == 0)
	{
		delay(100);
		/* ��-���� */
		if(INFRARED_W == 0)
		{
			nums_from_nor++;
			nums_from_wes++;
		}
	}
	
	if(INFRARED_W == 0)
	{
		delay(100);
		/* ��-���� */
		if(INFRARED_S == 0)
		{
			nums_from_wes++;
			nums_from_sou++;
		}
	}
	//���ĸ������ϵ����ݴ���
	if((TR0==1) && (car_flow_detection_flag==0))//��ʾ���ڴ���һ��ѭ��ģʽ��,��û�н��г��������ж�
	{
		//�����ϱ�������Ŀ�洢
		num[0] +=  nums_from_eas;
		num[1] +=  nums_from_sou;
		num[2] +=  nums_from_wes;
		num[3] +=  nums_from_nor;
	}
}