#ifndef _MOTOR_H
#define _MOTOR_H

#include "gd32l23x.h"
			
void motor_init(void);
void motor_left_on(void);
void motor_right_on(void);
void motor_left_on_pwm(uint16_t time_ms);
void motor_right_on_pwm(uint16_t time_ms);

void motor_off(void);
void motor_break(void);

#endif /* _MOTOR_H */
