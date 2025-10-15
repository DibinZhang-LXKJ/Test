#ifndef _MY_FUNCTIONS_H_
#define _MY_FUNCTIONS_H_

#include "p268.h"

//DECLARE_SEND_UART_METHOD(0)							//声明串口0发送函数
//DECLARE_SEND_UART_METHOD(1)							//声明串口1发送函数


uint get_sum(uchar* data,uint startIndex,uint len);	//校验和计算
int get_package_data(Package* p,uchar* buf);		//填充报文数据
//void write_flash(uint FlashDress,uint date);		//写Flash
//uchar READ_DRESS(uint FlashDress);					//读Flash
//void Flash_Crease(UNIT FLASHDress);					//擦除Flash
//void INIT_UART();									//初始化串口
//void INIT_SYS();									//初始化系统
void Delayms(UNIT delaytimems);						//软延时
void init_args();									//初始化系统变量
void delay_second(int second);						//软延时（秒）
void delay_ms(uint ms);								//软延时（毫秒）
BOOL update_program();								//自动升级程序
void reg_handler();									//注册串口报文处理函数（PC端）
uint get_package_direction(Package*);				//获取报文的接收方（自己或钥匙）

#endif
