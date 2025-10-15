/********************************************************************
 * 
 * 外部引用变量头文件，参见var.h
 * 
 ********************************************************************/

#ifndef _MY_EXT_VAR_H_
#define _MY_EXT_VAR_H_
#include "p268.h"


/***************临时变量****************/
extern uint g_temp_uint ;
extern uchar g_temp_uchar ;
extern uchar* g_temp_uchar_ptr ;
extern char g_temp_char ;
/***************************************/

/***************钥匙状态缓存****************/
extern uchar g_key_state_data[21];

//钥匙地址
extern uint g_key_address ;
//钥匙充电状态
extern uint g_key_charge_state ;
//钥匙ID
extern uchar* g_key_id ;
/*******************************************/

extern uint g_recv_task_flag ;
extern uint g_blink ;

//计时器刻度
extern uint g_timerA_tick ;
extern uint g_timerB_tick ;

//查询钥匙状态超时时间(秒)
extern INT g_query_key_timeout ;
extern INT g_query_charge_timeout ;

extern Package g_package;

extern uchar g_buf[256] ;

extern uint g_port1_error ;

//报文最大数据长度
const extern uint MaxDataLen ;

//钥匙状态标志
//第一位表示钥匙是否在位
//第二位表示钥匙是否通信过
extern uchar g_key_flags ;

//串口是正在通信标志
//第一位表示uart0是否正在通信
//第二位表示uart1是否正在通信
extern uint g_uart_data_flags ;

/////////////////////////////////////////
extern uint g_check_key_flag ;
extern uint g_checking_key_state ;
extern uint g_on_new_state ;

extern uint g_p35_high_count ;
extern uint g_recv_key_state ;

extern uint gc_check_full_count ;
extern uint gc_query_key_count ;

//钥匙是否正在同步
extern uint g_key_sync ;

extern TimeSpan g_last_checking_time;
extern TimeSpan g_last_time;

extern TimeSpan g_now_time;
//extern TimeSpan g_short_extern TimeSpan;
//extern TimeSpan g_last_query;
extern TimeSpan g_recv_key_time;

//////////在线更新//////////
//在线更新标志
extern uint g_update_step ;
//更新数据区偏移量
extern uint g_update_index ;
//接收更新包序号
extern uint g_current_package_index ;
//更新数据长度
extern uint g_update_len ;
//读写指针
extern char* write_ptr;
extern char* read_ptr;
///////////////////////////

extern uint g_disable_charging_time ;
extern uint send_delay_from_uart1 ;
//检测钥匙是否在位失败次数
extern uint g_comm_port_error ;

//定义UART数据接收变量
DECLARE_UART_DATA(0);
DECLARE_UART_DATA(1);

extern uint g_mode	;

#endif
