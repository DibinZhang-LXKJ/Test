#include "motor.h"
#include "delay.h"

#define bv(x) (0x01<<x)

#define MOTOR_DIN1_CLK  RCU_GPIOC
#define MOTOR_DIN1_PORT GPIOC
#define MOTOR_DIN1_PIN  GPIO_PIN_2

#define MOTOR_DIN2_CLK  RCU_GPIOC
#define MOTOR_DIN2_PORT GPIOC
#define MOTOR_DIN2_PIN  GPIO_PIN_3

#define MOTOR_DIN1_ON        gpio_bit_reset(MOTOR_DIN1_PORT, MOTOR_DIN1_PIN)
#define MOTOR_DIN1_OFF       gpio_bit_set(MOTOR_DIN1_PORT, MOTOR_DIN1_PIN)

#define MOTOR_DIN2_ON        gpio_bit_reset(MOTOR_DIN2_PORT, MOTOR_DIN2_PIN)
#define MOTOR_DIN2_OFF       gpio_bit_set(MOTOR_DIN2_PORT, MOTOR_DIN2_PIN)

void motor_init(void)
{
	rcu_periph_clock_enable(MOTOR_DIN1_CLK);

    gpio_mode_set(MOTOR_DIN1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MOTOR_DIN1_PIN);
    gpio_output_options_set(MOTOR_DIN1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, MOTOR_DIN1_PIN);

	gpio_mode_set(MOTOR_DIN2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MOTOR_DIN2_PIN);
    gpio_output_options_set(MOTOR_DIN2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, MOTOR_DIN2_PIN);

    motor_off();
} 

void motor_off(void)
{
	MOTOR_DIN1_OFF;
	MOTOR_DIN2_OFF;
}

void motor_break(void)
{
    MOTOR_DIN1_ON;
	MOTOR_DIN2_ON;
}

void motor_left_on_pwm(uint16_t time_ms)
{
	MOTOR_DIN2_OFF;
	for (uint16_t i = 0; i < time_ms; i++)
	{
		MOTOR_DIN1_ON;
		delay_us(700);
		MOTOR_DIN1_OFF;
		delay_us(300);
	}
//	motor_off();
//	delay_ms(5);
}

void motor_right_on_pwm(uint16_t time_ms)
{
	MOTOR_DIN1_OFF;
	for (uint16_t i = 0; i < time_ms; i++)
	{
		MOTOR_DIN2_ON;
		delay_us(700);
		MOTOR_DIN2_OFF;
		delay_us(300);
	}
	motor_off();
	delay_ms(5);
}

void motor_left_on(void)
{
	MOTOR_DIN1_ON;
	MOTOR_DIN2_OFF;
}

void motor_right_on(void)
{
	MOTOR_DIN1_OFF;
	MOTOR_DIN2_ON;
}
