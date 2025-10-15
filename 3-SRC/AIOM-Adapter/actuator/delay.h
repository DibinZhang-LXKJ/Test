#ifndef DELAY__HH
#define DELAY__HH

#include "gd32l23x.h"

//#define CPU_F 4000000ul

#define delay_second(x) delay_s(x)
#define Delayms(x) delay_ms(x)
#define Delayms_ms(x) delay_ms(x)

//#define delay_us(x) __delay_cycles(x*CPU_F/1000000)

void delay_ms(uint32_t ms);


void delay_s(uint16_t s);

void delay_10ms(uint16_t ms);

void delay_2_5us(void);
void delay_us(uint32_t us);

extern volatile uint32_t count_10ms;

#endif