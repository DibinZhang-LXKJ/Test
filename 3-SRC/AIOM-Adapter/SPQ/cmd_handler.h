/********************************************************************
 * 
 * 串口命令报文处理函数声明
 * 
 ********************************************************************/
 
#ifndef _MY_CMD_HANDLER_H_
#define _MY_CMD_HANDLER_H_

/////钥匙//////
//查询钥匙状态处理函数
void on_key_response_state();
//其它报文处理函数
void on_key_response_package();
//获取处理cmd命令的函数地址
cmd_handler get_key_handler(int cmd);

/////PC///////
//设置适配器地址
void on_set_adapter_address();
//获取\设置适配器ID
void on_kiss_adapter_id();
//复位适配器地址和ID
void on_reset_adapter_id();
//在线更新命令
void on_update_command();
//接收更新主程序
void on_update_program();
//接收中断向量表
void on_update_interrupt_table();
//设置钥匙ID
void on_set_key_id();
//查询钥匙状态
void on_get_key_state();
//同步结束命令
void on_sync_end();
//查询设备信息
void on_get_device_info();
//操作适配器闭锁机构
void on_opera_actuator();
//其它命令
void on_other_command();
//设置钥匙地址
void on_set_key_address();
//设置适配器模式
void on_set_mode();
//空动作
void do_nothing();

#endif
