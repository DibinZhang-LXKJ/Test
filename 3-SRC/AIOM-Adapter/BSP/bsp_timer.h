#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

#include "gd32l23x.h"


typedef enum{
    STATE_TIMER,
    LED_TIMER,
    SUM_TIMER
}timer_type_enum;



#define STATE_TIMER_ID              TIMER5
#define STATE_TIMER_CLK             RCU_TIMER5
#define STATE_TIMER_IRQn            TIMER5_IRQn
#define STATE_TIMER_IRQHandler      TIMER5_IRQHandler

#define LED_TIMER_ID                TIMER6
#define LED_TIMER_CLK               RCU_TIMER6
#define LED_TIMER_IRQn              TIMER6_IRQn
#define LED_TIMER_IRQHandler        TIMER6_IRQHandler


typedef void (*timer_callback_t)(void);

//time范围：1 ~ 6553，单位：ms
void bsp_timer_init(timer_type_enum timer_id, uint32_t time, uint8_t nvic_priority);

void timer_register_callback(timer_type_enum timer_id, timer_callback_t timer_Cbs);

#endif

