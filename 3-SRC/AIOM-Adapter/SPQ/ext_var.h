/********************************************************************
 * 
 * �ⲿ���ñ���ͷ�ļ����μ�var.h
 * 
 ********************************************************************/

#ifndef _MY_EXT_VAR_H_
#define _MY_EXT_VAR_H_
#include "p268.h"


/***************��ʱ����****************/
extern uint g_temp_uint ;
extern uchar g_temp_uchar ;
extern uchar* g_temp_uchar_ptr ;
extern char g_temp_char ;
/***************************************/

/***************Կ��״̬����****************/
extern uchar g_key_state_data[21];

//Կ�׵�ַ
extern uint g_key_address ;
//Կ�׳��״̬
extern uint g_key_charge_state ;
//Կ��ID
extern uchar* g_key_id ;
/*******************************************/

extern uint g_recv_task_flag ;
extern uint g_blink ;

//��ʱ���̶�
extern uint g_timerA_tick ;
extern uint g_timerB_tick ;

//��ѯԿ��״̬��ʱʱ��(��)
extern INT g_query_key_timeout ;
extern INT g_query_charge_timeout ;

extern Package g_package;

extern uchar g_buf[256] ;

extern uint g_port1_error ;

//����������ݳ���
const extern uint MaxDataLen ;

//Կ��״̬��־
//��һλ��ʾԿ���Ƿ���λ
//�ڶ�λ��ʾԿ���Ƿ�ͨ�Ź�
extern uchar g_key_flags ;

//����������ͨ�ű�־
//��һλ��ʾuart0�Ƿ�����ͨ��
//�ڶ�λ��ʾuart1�Ƿ�����ͨ��
extern uint g_uart_data_flags ;

/////////////////////////////////////////
extern uint g_check_key_flag ;
extern uint g_checking_key_state ;
extern uint g_on_new_state ;

extern uint g_p35_high_count ;
extern uint g_recv_key_state ;

extern uint gc_check_full_count ;
extern uint gc_query_key_count ;

//Կ���Ƿ�����ͬ��
extern uint g_key_sync ;

extern TimeSpan g_last_checking_time;
extern TimeSpan g_last_time;

extern TimeSpan g_now_time;
//extern TimeSpan g_short_extern TimeSpan;
//extern TimeSpan g_last_query;
extern TimeSpan g_recv_key_time;

//////////���߸���//////////
//���߸��±�־
extern uint g_update_step ;
//����������ƫ����
extern uint g_update_index ;
//���ո��°����
extern uint g_current_package_index ;
//�������ݳ���
extern uint g_update_len ;
//��дָ��
extern char* write_ptr;
extern char* read_ptr;
///////////////////////////

extern uint g_disable_charging_time ;
extern uint send_delay_from_uart1 ;
//���Կ���Ƿ���λʧ�ܴ���
extern uint g_comm_port_error ;

//����UART���ݽ��ձ���
DECLARE_UART_DATA(0);
DECLARE_UART_DATA(1);

extern uint g_mode	;

#endif
