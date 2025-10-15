#ifndef _BSP_PORT_H
#define _BSP_PORT_H

#include "gd32l23x.h"

typedef enum {
    LED1_G = 0,
    LED1_R,
    LED2_G,
    LED2_R,
    LED3_G,
    LED3_R,
    WATCHDOG,
    FMQ,
    CHARGER,
    SUM_PORT
} port_type_enum;

typedef enum {
    CHECK,
    SUM_EXTI
}exti_type_enum;

#define LED1_G_CLK        RCU_GPIOA
#define LED1_G_PORT       GPIOA
#define LED1_G_PIN        GPIO_PIN_8
#define LED1_R_CLK        RCU_GPIOA
#define LED1_R_PORT       GPIOA
#define LED1_R_PIN        GPIO_PIN_9

#define LED2_G_CLK        RCU_GPIOC
#define LED2_G_PORT       GPIOC
#define LED2_G_PIN        GPIO_PIN_8
#define LED2_R_CLK        RCU_GPIOC
#define LED2_R_PORT       GPIOC
#define LED2_R_PIN        GPIO_PIN_9

#define LED3_G_CLK        RCU_GPIOC
#define LED3_G_PORT       GPIOC
#define LED3_G_PIN        GPIO_PIN_6
#define LED3_R_CLK        RCU_GPIOC
#define LED3_R_PORT       GPIOC
#define LED3_R_PIN        GPIO_PIN_7

#define WATCHDOG_CLK    RCU_GPIOB
#define WATCHDOG_PORT   GPIOB
#define WATCHDOG_PIN    GPIO_PIN_12

#define FMQ_CLK       RCU_GPIOB
#define FMQ_PORT      GPIOB
#define FMQ_PIN       GPIO_PIN_11

#define CHARGER_CLK   RCU_GPIOA
#define CHARGER_PORT  GPIOA
#define CHARGER_PIN   GPIO_PIN_1

//短路检测引脚，外部中断输入
#define CHECK_CLK                   RCU_GPIOB
#define CHECK_PORT                  GPIOB
#define CHECK_PIN                   GPIO_PIN_10
#define CHECK_EXTI_LINE             EXTI_10
#define CHECK_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOB
#define CHECK_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN10
#define CHECK_EXTI_IRQn             EXTI10_15_IRQn
#define CHECK_EXTI_IRQHandler       EXTI10_15_IRQHandler

typedef void (*exti_callback_t)(void);

void bsp_port_init(port_type_enum port_id);
void bsp_exti_init(exti_type_enum exti_id, uint8_t nvic_priority);
void exti_register_callback(exti_type_enum exti_id, exti_callback_t exti_Cbs);
void bsp_port_set(port_type_enum port_id, uint8_t value);
void bsp_port_toggle(port_type_enum port_id);
uint8_t bsp_get_port_value(port_type_enum port_id);
#endif
