#ifndef _BSP_CPU_H
#define _BSP_CPU_H

#include "gd32l23x.h"

void enter_low_power(void);
void exit_low_power(void);

void mcu_software_reset(void);
void clear_reset_flag(void);

void enter_critical_section(void);
void exit_critical_section(void);

#endif
