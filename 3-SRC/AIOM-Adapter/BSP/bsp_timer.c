#include "bsp_timer.h"


static const rcu_periph_enum TIMER_CLK[SUM_TIMER] = {STATE_TIMER_CLK, LED_TIMER_CLK};
static const uint32_t TIMER_ID[SUM_TIMER] = {STATE_TIMER_ID, LED_TIMER_ID};
static const IRQn_Type TIMER_IRQn[SUM_TIMER] = {STATE_TIMER_IRQn, LED_TIMER_IRQn};

timer_callback_t timer_handlers[SUM_TIMER] = {0};

//time范围：1 ~ 6553，单位：ms
void bsp_timer_init(timer_type_enum timer_id, uint32_t time, uint8_t nvic_priority)       
{
    timer_parameter_struct timer_initpara;
	uint64_t sysClock;
    
    sysClock = rcu_clock_freq_get(CK_SYS);
    
    nvic_irq_enable(TIMER_IRQn[timer_id], nvic_priority);
		
    /* enable the peripherals clock */
    rcu_periph_clock_enable(TIMER_CLK[timer_id]);
    
    timer_deinit(TIMER_ID[timer_id]);

    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /*  configuration */
    timer_initpara.prescaler        = sysClock / 10000 - 1;//6400-1;
    timer_initpara.alignedmode      = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period           = time * 10 - 1;
    timer_initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(TIMER_ID[timer_id], &timer_initpara);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER_ID[timer_id]);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER_ID[timer_id], TIMER_INT_FLAG_UP);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(TIMER_ID[timer_id], TIMER_INT_UP);


    /* enable a TIMER */
    timer_enable(TIMER_ID[timer_id]);
}

void timer_register_callback(timer_type_enum timer_id, timer_callback_t timer_Cbs)
{
    if (timer_handlers[timer_id] == 0)
    {
        timer_handlers[timer_id] = timer_Cbs;
    }
}

void STATE_TIMER_IRQHandler(void)
{
    uint32_t state_timer_flag = timer_interrupt_flag_get(TIMER_ID[STATE_TIMER], TIMER_INT_FLAG_UP);
    if (state_timer_flag)
    {
        if (timer_handlers[STATE_TIMER] != 0)
        {
            timer_handlers[STATE_TIMER]();
        }
        timer_interrupt_flag_clear(TIMER_ID[STATE_TIMER], TIMER_INT_FLAG_UP);
    }
}

void LED_TIMER_IRQHandler(void)
{
    uint32_t led_timer_flag = timer_interrupt_flag_get(TIMER_ID[LED_TIMER], TIMER_INT_FLAG_UP);
    if (led_timer_flag)
    {
        if (timer_handlers[LED_TIMER] != 0)
        {
            timer_handlers[LED_TIMER]();
        }
        timer_interrupt_flag_clear(TIMER_ID[LED_TIMER], TIMER_INT_FLAG_UP);
    }
}
