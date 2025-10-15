#ifndef _IAP_CONFIG_H_
#define _IAP_CONFIG_H_

#include "gd32l23x.h"

//适配器功能运行区
#define PROGRAM_CODE_START  0x08000000	//充电座主程序起始地址

//适配器Bootloader功能区
#define BOOTLOADER_START_ADDR   0x08010000	//Bootloader功能区起始地址
#define BOOTLOADER_END_ADDR     0x08014000	//Bootloader功能区结束地址

//适配器更新数据存储区
#define UPDATE_CODE_START       0x08014000	//更新数据存储区地址
#define INTERUPT_CODE_START     0x08020000	//更新标志存储区地址

#endif /* _IAP_VALUE_H_ */
