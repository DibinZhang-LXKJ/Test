#ifndef _PORT_CONFIG_H
#define _PORT_CONFIG_H


#include "gd32l23x.h"
#include "bsp_port.h"
#include "bsp_uart.h"
#include "bsp_fwdgt.h"


//#define UPDATE_CODE


#ifndef NULL
	#define NULL 0
#endif
//#define	UCHAR	unsigned char
//#define	UNIT	uint16_t 
#define	uchar	unsigned char
#define	uint	uint16_t
#define	TRUE    1
#define	FALSE   0
typedef	uint8_t     BOOL;
typedef	int16_t 	INT;


#define  EnableChongDian      bsp_port_set(CHARGER, 0)
#define  DisnableChongDian    bsp_port_set(CHARGER, 1)


#define  CHECK_KEY_UART 	bsp_get_uart_rx_io_status(KEY_UART)
#define  CHECK_CHARGER 		bsp_get_port_value(CHARGER)
//  (P##IONAME##IN & (1<<IONUMBER))


#define  SET_FMQ  			bsp_port_toggle(FMQ)//SET_XOR(1,4)  


#define  LED1OFF        	bsp_port_set(LED1_G, 0);bsp_port_set(LED1_R, 0)
#define  LED1ONGr      	 	bsp_port_set(LED1_G, 1);bsp_port_set(LED1_R, 0)
#define  LED1ONRed     	 	bsp_port_set(LED1_G, 0);bsp_port_set(LED1_R, 1)
#define  LED1ONYe       	bsp_port_set(LED1_G, 1);bsp_port_set(LED1_R, 1)


#define  LED2OFF        	bsp_port_set(LED2_G, 0);bsp_port_set(LED2_R, 0)
#define  LED2ONGr       	bsp_port_set(LED2_G, 1);bsp_port_set(LED2_R, 0)
#define  LED2ONRed      	bsp_port_set(LED2_G, 0);bsp_port_set(LED2_R, 1)
#define  LED2ONYe       	bsp_port_set(LED2_G, 1);bsp_port_set(LED2_R, 1)

#define  LED3OFF        	bsp_port_set(LED3_G, 0);bsp_port_set(LED3_R, 0)
#define  LED3ONGr       	bsp_port_set(LED3_G, 1);bsp_port_set(LED3_R, 0)
#define  LED3ONRed      	bsp_port_set(LED3_G, 0);bsp_port_set(LED3_R, 1)
#define  LED3ONYe       	bsp_port_set(LED3_G, 1);bsp_port_set(LED3_R, 1)

void send_buff_clear(void);
void send_buff_start(void);
void uart_send(uart_type_enum uart_id, uint8_t* data, uint32_t len);
void send_to_pc(uint8_t* data, uint32_t len);
void send_to_key(uint8_t* data, uint32_t len);


//直接连续写入，不擦除，不校验
void fmc_contiune_write(uint32_t address, uint8_t *data, uint32_t len);

void watch_delay(void);


#define  SetWatchDog   		bsp_port_toggle(WATCHDOG); watch_delay();feed_fwdgt();//P6OUT^=0x04;

#endif
