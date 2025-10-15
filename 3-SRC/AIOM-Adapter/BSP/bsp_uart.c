#include "bsp_uart.h"

static const rcu_periph_enum USART_GPIO_CLK[SUM_UART] = {PC_GPIO_CLK, KEY_GPIO_CLK};
static const uint32_t USART_GPIO_PORT[SUM_UART] = {PC_GPIO_PORT, KEY_GPIO_PORT};
static const uint32_t USART_TX_PIN[SUM_UART] = {PC_TX_PIN, KEY_TX_PIN};
static const uint32_t USART_RX_PIN[SUM_UART] = {PC_RX_PIN, KEY_RX_PIN};
static const uint32_t USART_AF[SUM_UART] = {PC_AF, KEY_AF};

static const uint32_t USART_ID[SUM_UART] = {PC_USART, KEY_USART};
static const rcu_periph_enum USART_CLK[SUM_UART] = {PC_USART_CLK, KEY_USART_CLK};
static const IRQn_Type USART_IRQ[SUM_UART] = {PC_USART_IRQ, KEY_USART_IRQ};



uart_callback_t usart_handlers[SUM_UART] = {0};

void bsp_uart_init(uart_type_enum uart_id, uint32_t baudrate, uint8_t nvic_priority)
{   
    /* USART interrupt configuration */
    nvic_irq_enable(USART_IRQ[uart_id], nvic_priority);

    /* enable COM GPIO clock */
    rcu_periph_clock_enable(USART_GPIO_CLK[uart_id]);
    /* enable USART clock */
    rcu_periph_clock_enable(USART_CLK[uart_id]); 
   
    /* connect port to USART_TX */
    gpio_af_set(USART_GPIO_PORT[uart_id], USART_AF[uart_id], USART_TX_PIN[uart_id]);
    gpio_af_set(USART_GPIO_PORT[uart_id], USART_AF[uart_id], USART_RX_PIN[uart_id]);
    
    gpio_mode_set(USART_GPIO_PORT[uart_id], GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART_TX_PIN[uart_id]);
    gpio_output_options_set(USART_GPIO_PORT[uart_id], GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, USART_TX_PIN[uart_id]);
    gpio_mode_set(USART_GPIO_PORT[uart_id], GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART_RX_PIN[uart_id]);
    gpio_output_options_set(USART_GPIO_PORT[uart_id], GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, USART_RX_PIN[uart_id]);


    /* USART configure */
    usart_deinit(USART_ID[uart_id]);
    usart_word_length_set(USART_ID[uart_id], USART_WL_8BIT);
    usart_stop_bit_set(USART_ID[uart_id], USART_STB_1BIT);
    usart_parity_config(USART_ID[uart_id], USART_PM_NONE);
    usart_baudrate_set(USART_ID[uart_id], baudrate);
    usart_receive_config(USART_ID[uart_id], USART_RECEIVE_ENABLE);
    usart_transmit_config(USART_ID[uart_id], USART_TRANSMIT_ENABLE);

    usart_enable(USART_ID[uart_id]);
    usart_interrupt_enable(USART_ID[uart_id], USART_INT_RBNE);
    usart_interrupt_disable(USART_ID[uart_id], USART_INT_TBE);
}

void bsp_usart_send_byte(uart_type_enum uart_id, uint8_t data)
{
    usart_data_transmit(USART_ID[uart_id], data);
    while (usart_flag_get(USART_ID[uart_id], USART_FLAG_TBE) == RESET){
    }
}

uint8_t bsp_get_uart_rx_io_status(uart_type_enum uart_id)
{
    return gpio_input_bit_get(USART_GPIO_PORT[uart_id], USART_RX_PIN[uart_id]);
}

void uart_register_callback(uart_type_enum uart_id, uart_callback_t usart_Cbs)
{
    if (usart_handlers[uart_id] == 0) 
    { 
        usart_handlers[uart_id] = usart_Cbs; 
    }
}

void PC_IRQHandler(void)
{
    if (usart_interrupt_flag_get(PC_USART, USART_INT_FLAG_RBNE)) 
    {
        uint8_t data = usart_data_receive(PC_USART);
        if (usart_handlers[PC_UART]) { usart_handlers[PC_UART](data); }
        usart_interrupt_flag_clear(PC_USART, USART_INT_FLAG_RBNE);
    }
}

void KEY_IRQHandler(void)
{
    if (usart_interrupt_flag_get(KEY_USART, USART_INT_FLAG_RBNE)) 
    {
        uint8_t data = usart_data_receive(KEY_USART);
        if (usart_handlers[KEY_UART]) { usart_handlers[KEY_UART](data); }
        usart_interrupt_flag_clear(KEY_USART, USART_INT_FLAG_RBNE);
    }
}

