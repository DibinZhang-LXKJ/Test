/********************************************************************
 * 
 * 系统变量声明头文件
 * 
 ********************************************************************/
#ifndef _MY_VAR_H_
#define _MY_VAR_H_


#include "p268.h"


/***************临时变量****************/
uint g_temp_uint = 0;
uchar g_temp_uchar = 0;
uchar* g_temp_uchar_ptr = NULL;
char g_temp_char = 0;
/***************************************/



/***************钥匙状态缓存****************/
uchar g_key_state_data[21] = 
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF
};


//钥匙地址
uint g_key_address = 0xFFFF;
//钥匙充电状态
uint g_key_charge_state = 0;
//钥匙ID
uchar* g_key_id = &g_key_state_data[5];

/*******************************************/



uint g_recv_task_flag = FALSE;		//是否刚接收到任务标志
uint g_blink = TRUE;				//适配器LED闪烁标志


uint g_timerA_tick = 0;				//计时器A刻度
uint g_timerB_tick = 0;				//计时器B刻度



INT g_query_key_timeout = 0;		//查询钥匙状态超时时间(秒)
//INT g_query_charge_timeout = 0;		//查询钥匙充电状态超时时间(秒)

Package g_package;					//全局报文对象（公用）

uchar g_buf[256] = {0};				//全局报文数据缓冲区

uint g_port1_error = 0;				//端口检测错误计数器

const uint MaxDataLen = 256;		//报文最大数据长度

uint g_mode	= MODE_NORMAL;			//适配器模式

//钥匙状态标志
//第一位表示钥匙是否在位
//第二位表示钥匙是否通信过
uchar g_key_flags = 0;


//串口是正在通信标志
//第一位表示uart0是否正在通信
//第二位表示uart1是否正在通信
uint g_uart_data_flags = 0;



uint g_check_key_flag = KEY_FLAG_TAKEN;		//当前钥匙状态（不在位、充电、刚充满、已充满）
uint g_checking_key_state = FALSE;			//已不使用
uint g_on_new_state = TRUE;					//指示当前是否进入一个新的状态

 
uint g_p35_high_count = 0;					//当钥匙在位时检测到35口的错误次数
uint g_recv_key_state = FALSE;				//指示是否接收到钥匙状态报文


uint gc_check_full_count = 0;				//当钥匙在位时接收到钥匙充满电报文的次数
uint gc_query_key_count = 0;				//向钥匙发送查询报文计数

uint g_key_sync = FALSE;					//指示钥匙是否正在同步


TimeSpan g_last_checking_time;				//记录上一次检测钥匙时间
TimeSpan g_last_time;						//记录上一次动作时间（已不使用）

TimeSpan g_now_time;						//系统当前时间
TimeSpan g_recv_key_time;					//记录上一次收到钥匙状态时间

//定义UART数据接收变量
DEFINE_UART_DATA(0);
DEFINE_UART_DATA(1);



//////////在线更新//////////

uint g_update_step = UPDATE_OFF;			//当前更新状态
uint g_update_index = 0xFFFF;				  //更新数据区偏移量
uint g_current_package_index = 0;			//接收更新包序号


///////////////////////////


uint g_disable_charging_time = DISABLE_CHARGING_TIMEOUT;	//关充电时间

uint send_delay_from_uart1 = 0xFFFF;	//串口延时发送标志位
uint g_comm_port_error = 0;				//当处于通信状态时检测钥匙是否在位失败次数

cmd_handler g_pc_handler[256] = {NULL};	//串口报文处理函数，每个索引对应于值与之相等的命令码
//cmd_handler	g_key_handler[128] = {NULL};	//由于内存限制，已废弃
cmd_handler g_default_pc_handler = NULL;	//当未设置串口处理函数时使用的默认处理方式
//cmd_handler g_default_key_handler = NULL;
#endif
