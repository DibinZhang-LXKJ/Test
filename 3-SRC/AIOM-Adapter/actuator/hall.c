#include "hall.h"
#include "delay.h"

void vref_init(void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);
    vref_deinit();
    vref_high_impedance_mode_disable();
    vref_enable();
    while(vref_status_get() != SET){}
}

void hall_adc_init(void)
{
    vref_init();
	/* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);

	/* reset ADC */
    //adc_deinit();
    /* ADC contineous function enable */
    //adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1U);
    /* ADC routine channel config */
    //adc_routine_channel_config(0U, ADC_CHANNEL_11, ADC_SAMPLETIME_55POINT5);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_NONE);
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, ENABLE);
    /* ADC regular channel differential mode configure 差分配置*/
    //adc_channel_differential_mode_config(ADC_DIFFERENTIAL_MODE_CHANNEL_11, ENABLE);
    /* ADC DMA function enable */
    //adc_dma_mode_enable();

    /* enable ADC interface */
    adc_enable();
	delay_ms(10);
    //delay_1ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
    /* ADC software trigger enable */
    //adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);
}

uint16_t hall_adc_get(void)
{
	/* ADC routine channel config */
    adc_routine_channel_config(0U, ADC_CHANNEL_11, ADC_SAMPLETIME_7POINT5);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);

    /* wait the end of conversion flag */
    while(!adc_flag_get(ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear(ADC_FLAG_EOC);
    /* return routine channel sample value */
    return (adc_routine_data_read());
}


