#ifndef _SPQ_FUNCTION_H
#define _SPQ_FUNCTION_H

#include "gd32l23x.h"
#include "bsp_uart.h"
#include "port_config.h"
#include "p268.h"

//适配器地址范围
#define DEFAULT_ADAPTER_ADDRESS 0xFFFF	//默认的适配器地址
#define	MIN_ADAPTER_ADDRESS	0x01		//最小适配器地址
#define	MAX_ADAPTER_ADDRESS	0xFF		//最大适配器地址
//钥匙地址范围
#define DEFAULT_KEY_ADDRESS 0xFFFF		//默认的钥匙地址
#define	MIN_KEY_ADDRESS	0x100			//最小钥匙地址
#define	MAX_KEY_ADDRESS	0xFFFE			//最大钥匙地址

//适配器地址即ID存储位置
#define LOCAL_ADDR_OF_FLASH_SAVE   0xFD00UL
#define LOCAL_ID_OF_FLASH_SAVE     0xFD04UL
#define READ_DRESS_BYTE(addr)     (*(volatile uint8_t *)(0x08000000 + addr))
#define UINT16_HL(HByte,LByte)    ((HByte<<8) + LByte)
#define LOCAL_ADDRESS             UINT16_HL(READ_DRESS_BYTE(0xFD01), READ_DRESS_BYTE(0xFD00))	 //本地适配器地址

//适配器在线更新
//#define PROGRAM_CODE_START  0x5000	//充电座主程序起始地址
//#define UPDATE_CODE_START   0x11000	//更新数据存储区地址
//#define INTERUPT_CODE_START 0x10000	//中断向量存储区地址

void init_args(void);            //初始化参数
void reg_handler(void);          //注册pc命令处理函数

int get_package_data(Package* p, uchar* buf);
uint get_package_direction(Package* p);

//回复应答
void send_ack_from_package(uart_type_enum uart_id, Package pSource, uint8_t *buf);
void send_ack(uart_type_enum uart_id, uint16_t src, uint16_t dest, uint8_t *buf);
void send_nack_from_package(uart_type_enum uart_id, Package pSource, uint8_t *buf);
void send_nack(uart_type_enum uart_id, uint16_t src, uint16_t dest, uint8_t *buf);

void live_update(void);
void handle_key_data(uint8_t data);
void handle_pc_data(uint8_t data);
void handle_key_state(void);
void handle_led_state(void);
void handle_duanlu(void);

void OPEN_FMQ(void);
#endif
