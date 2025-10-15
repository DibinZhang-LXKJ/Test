#include "delay.h"
#include "port_config.h"

volatile uint32_t count_10ms;

// void delay_ms(uint16_t ms)
// {
//   while(ms--){
//     SetWatchDog;
//    // __delay_cycles(1000*CPU_F/1000000 - 5);
//   }
// }

void delay_ms(uint32_t times)
{
    volatile uint32_t i, t;
    
    for (i = 0; i < times; i++)
    {
        for (t = 0; t < 15000; t++)
        {
            __NOP();
        }
       // SetWatchDog;
    }
}

void delay_s(uint16_t s)
{
    while (s--) {
        SetWatchDog;
        delay_ms(1000);
    }
}

void delay_10ms(uint16_t tms)
{
  uint16_t pre = count_10ms;
  while ((pre + tms) > count_10ms){}
}



void delay_2_5us(void) 
{
    // 大约 160 - 10（补偿循环开销）≈ 150
    for (volatile int i = 0; i < 30; i++) {
        __NOP();
    }
}


// 精确延时函数（基于24MHz系统时钟）
void delay_us(uint32_t us) 
{
    us = us * 6;  // 24MHz下1us≈6个周期（实测调整）
    while(us--) {
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
    }
}
