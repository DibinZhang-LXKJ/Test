#ifndef _HALL_H
#define _HALL_H

#include "gd32l23x.h"

void hall_adc_init(void);
void hall_adc_on(void);
void hall_adc_off(void);
uint16_t hall_adc_get(void);

#endif /* _HALL_H */
