/********************************************************************
 * 
 * ϵͳ��������ͷ�ļ�
 * 
 ********************************************************************/
#ifndef _MY_VAR_H_
#define _MY_VAR_H_


#include "p268.h"


/***************��ʱ����****************/
uint g_temp_uint = 0;
uchar g_temp_uchar = 0;
uchar* g_temp_uchar_ptr = NULL;
char g_temp_char = 0;
/***************************************/



/***************Կ��״̬����****************/
uchar g_key_state_data[21] = 
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF
};


//Կ�׵�ַ
uint g_key_address = 0xFFFF;
//Կ�׳��״̬
uint g_key_charge_state = 0;
//Կ��ID
uchar* g_key_id = &g_key_state_data[5];

/*******************************************/



uint g_recv_task_flag = FALSE;		//�Ƿ�ս��յ������־
uint g_blink = TRUE;				//������LED��˸��־


uint g_timerA_tick = 0;				//��ʱ��A�̶�
uint g_timerB_tick = 0;				//��ʱ��B�̶�



INT g_query_key_timeout = 0;		//��ѯԿ��״̬��ʱʱ��(��)
//INT g_query_charge_timeout = 0;		//��ѯԿ�׳��״̬��ʱʱ��(��)

Package g_package;					//ȫ�ֱ��Ķ��󣨹��ã�

uchar g_buf[256] = {0};				//ȫ�ֱ������ݻ�����

uint g_port1_error = 0;				//�˿ڼ����������

const uint MaxDataLen = 256;		//����������ݳ���

uint g_mode	= MODE_NORMAL;			//������ģʽ

//Կ��״̬��־
//��һλ��ʾԿ���Ƿ���λ
//�ڶ�λ��ʾԿ���Ƿ�ͨ�Ź�
uchar g_key_flags = 0;


//����������ͨ�ű�־
//��һλ��ʾuart0�Ƿ�����ͨ��
//�ڶ�λ��ʾuart1�Ƿ�����ͨ��
uint g_uart_data_flags = 0;



uint g_check_key_flag = KEY_FLAG_TAKEN;		//��ǰԿ��״̬������λ����硢�ճ������ѳ�����
uint g_checking_key_state = FALSE;			//�Ѳ�ʹ��
uint g_on_new_state = TRUE;					//ָʾ��ǰ�Ƿ����һ���µ�״̬

 
uint g_p35_high_count = 0;					//��Կ����λʱ��⵽35�ڵĴ������
uint g_recv_key_state = FALSE;				//ָʾ�Ƿ���յ�Կ��״̬����


uint gc_check_full_count = 0;				//��Կ����λʱ���յ�Կ�׳����籨�ĵĴ���
uint gc_query_key_count = 0;				//��Կ�׷��Ͳ�ѯ���ļ���

uint g_key_sync = FALSE;					//ָʾԿ���Ƿ�����ͬ��


TimeSpan g_last_checking_time;				//��¼��һ�μ��Կ��ʱ��
TimeSpan g_last_time;						//��¼��һ�ζ���ʱ�䣨�Ѳ�ʹ�ã�

TimeSpan g_now_time;						//ϵͳ��ǰʱ��
TimeSpan g_recv_key_time;					//��¼��һ���յ�Կ��״̬ʱ��

//����UART���ݽ��ձ���
DEFINE_UART_DATA(0);
DEFINE_UART_DATA(1);



//////////���߸���//////////

uint g_update_step = UPDATE_OFF;			//��ǰ����״̬
uint g_update_index = 0xFFFF;				  //����������ƫ����
uint g_current_package_index = 0;			//���ո��°����


///////////////////////////


uint g_disable_charging_time = DISABLE_CHARGING_TIMEOUT;	//�س��ʱ��

uint send_delay_from_uart1 = 0xFFFF;	//������ʱ���ͱ�־λ
uint g_comm_port_error = 0;				//������ͨ��״̬ʱ���Կ���Ƿ���λʧ�ܴ���

cmd_handler g_pc_handler[256] = {NULL};	//���ڱ��Ĵ�������ÿ��������Ӧ��ֵ��֮��ȵ�������
//cmd_handler	g_key_handler[128] = {NULL};	//�����ڴ����ƣ��ѷ���
cmd_handler g_default_pc_handler = NULL;	//��δ���ô��ڴ�����ʱʹ�õ�Ĭ�ϴ���ʽ
//cmd_handler g_default_key_handler = NULL;
#endif
