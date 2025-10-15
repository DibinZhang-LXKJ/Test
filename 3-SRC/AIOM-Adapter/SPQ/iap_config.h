#ifndef _IAP_CONFIG_H_
#define _IAP_CONFIG_H_

#include "gd32l23x.h"

//����������������
#define PROGRAM_CODE_START  0x08000000	//�������������ʼ��ַ

//������Bootloader������
#define BOOTLOADER_START_ADDR   0x08010000	//Bootloader��������ʼ��ַ
#define BOOTLOADER_END_ADDR     0x08014000	//Bootloader������������ַ

//�������������ݴ洢��
#define UPDATE_CODE_START       0x08014000	//�������ݴ洢����ַ
#define INTERUPT_CODE_START     0x08020000	//���±�־�洢����ַ

#endif /* _IAP_VALUE_H_ */
