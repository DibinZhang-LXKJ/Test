#include "bsp_port.h"

static const rcu_periph_enum PORT_CLK[SUM_PORT] = { LED1_G_CLK, LED1_R_CLK, LED2_G_CLK, 
                                                    LED2_R_CLK, LED3_G_CLK, LED3_R_CLK, 
                                                    WATCHDOG_CLK, FMQ_CLK, CHARGER_CLK,
                                                  };
static const uint32_t PORT_GPIO[SUM_PORT] = {LED1_G_PORT, LED1_R_PORT, LED2_G_PORT, LED2_R_PORT, LED3_G_PORT, LED3_R_PORT, WATCHDOG_PORT, FMQ_PORT, CHARGER_PORT};
static const uint32_t PORT_PIN[SUM_PORT] = {LED1_G_PIN, LED1_R_PIN, LED2_G_PIN, LED2_R_PIN, LED3_G_PIN, LED3_R_PIN, WATCHDOG_PIN, FMQ_PIN, CHARGER_PIN};


static const rcu_periph_enum EXTI_GPIO_CLK[SUM_EXTI] = {CHECK_CLK};
static const uint32_t EXTI_GPIO[SUM_EXTI] = {CHECK_PORT};
static const uint32_t EXTI_PIN[SUM_EXTI] = {CHECK_PIN};
static const exti_line_enum EXTI_LINE[SUM_EXTI] = {CHECK_EXTI_LINE};
static const uint8_t EXTI_PORT_SOURCE[SUM_EXTI] = {CHECK_EXTI_PORT_SOURCE};
static const uint8_t EXTI_PIN_SOURCE[SUM_EXTI] = {CHECK_EXTI_PIN_SOURCE};
static const IRQn_Type EXTI_IRQn[SUM_EXTI] = {CHECK_EXTI_IRQn};

exti_callback_t exti_handlers[SUM_EXTI] = {0};

void bsp_port_init(port_type_enum port_id)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(PORT_CLK[port_id]);
    /* configure GPIO pin */
    gpio_mode_set(PORT_GPIO[port_id], GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, PORT_PIN[port_id]);
    gpio_output_options_set(PORT_GPIO[port_id], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PORT_PIN[port_id]);

    /* reset GPIO pin */
    gpio_bit_reset(PORT_GPIO[port_id], PORT_PIN[port_id]);
}

void bsp_port_set(port_type_enum port_id, uint8_t value)
{
    if (value)
    {
        gpio_bit_set(PORT_GPIO[port_id], PORT_PIN[port_id]);
    }
    else
    {
        gpio_bit_reset(PORT_GPIO[port_id], PORT_PIN[port_id]);
    }
}

void bsp_port_toggle(port_type_enum port_id)
{
    gpio_bit_toggle(PORT_GPIO[port_id], PORT_PIN[port_id]);
}


uint8_t bsp_get_port_value(port_type_enum port_id)
{
    return gpio_input_bit_get(PORT_GPIO[port_id], PORT_PIN[port_id]);
}

//短路检测（一个，中断输入）
void bsp_exti_init(exti_type_enum exti_id, uint8_t nvic_priority)
{
    rcu_periph_clock_enable(EXTI_GPIO_CLK[exti_id]);
    rcu_periph_clock_enable(RCU_SYSCFG);                //EXTI需要SYSCFG时钟

    /* configure button pin as input */
    gpio_mode_set(EXTI_GPIO[exti_id], GPIO_MODE_INPUT, GPIO_PUPD_NONE, EXTI_PIN[exti_id]);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(EXTI_IRQn[exti_id], nvic_priority);
    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(EXTI_PORT_SOURCE[exti_id], EXTI_PIN_SOURCE[exti_id]);
    /* configure key EXTI line */
    exti_init(EXTI_LINE[exti_id], EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_LINE[exti_id]);
}

void exti_register_callback(exti_type_enum exti_id, exti_callback_t exti_Cbs)
{
    if (exti_handlers[exti_id] == 0)
    {
        exti_handlers[exti_id] = exti_Cbs;
    }
}

void CHECK_EXTI_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_LINE[CHECK])) {
        if (exti_handlers[CHECK] != 0)
        {
            exti_handlers[CHECK]();
        }
        exti_interrupt_flag_clear(EXTI_LINE[CHECK]);
    }
}

