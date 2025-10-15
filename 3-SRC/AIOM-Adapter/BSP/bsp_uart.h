#ifndef _BSP_UART_H
#define _BSP_UART_H

#include "gd32l23x.h"

typedef enum{
    PC_UART,
    KEY_UART,
    
    SUM_UART
} uart_type_enum;

#define PC_GPIO_CLK    RCU_GPIOB
#define PC_GPIO_PORT   GPIOB
#define PC_TX_PIN      GPIO_PIN_6
#define PC_RX_PIN      GPIO_PIN_7
#define PC_AF          GPIO_AF_7
#define PC_USART       USART0  
#define PC_USART_CLK   RCU_USART0
#define PC_USART_IRQ   USART0_IRQn
#define PC_IRQHandler  USART0_IRQHandler

#define KEY_GPIO_CLK   RCU_GPIOB
#define KEY_GPIO_PORT  GPIOB
#define KEY_TX_PIN     GPIO_PIN_3
#define KEY_RX_PIN     GPIO_PIN_4
#define KEY_AF         GPIO_AF_8
#define KEY_USART      UART4
#define KEY_USART_CLK  RCU_UART4
#define KEY_USART_IRQ  UART4_IRQn
#define KEY_IRQHandler UART4_IRQHandler

#define enable_uart_rx_interrupt(uart_id)  // USART_REG_VAL(uart_id, USART_INT_RBNE) |= BIT(USART_BIT_POS(USART_INT_RBNE))
#define disable_uart_rx_interrupt(uart_id) // USART_REG_VAL(uart_id, USART_INT_RBNE) &= ~BIT(USART_BIT_POS(USART_INT_RBNE))


typedef void (*uart_callback_t)(uint8_t);

void bsp_uart_init(uart_type_enum uart_id, uint32_t baudrate, uint8_t nvic_priority);
void uart_register_callback(uart_type_enum uart_id, uart_callback_t usart_Cbs);

uint8_t bsp_get_uart_rx_io_status(uart_type_enum uart_id);
void bsp_usart_send_byte(uart_type_enum uart_id, uint8_t data);

#endif
