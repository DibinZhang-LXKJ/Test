#include "actuator.h"
//#include "debug.h"
#include "delay.h"
#include "hall.h"
#include "motor.h"
#include "spq_function.h"

#include "bsp_log.h"

#define ACTUATOR_NULL_EVENT  0x00
#define ACTUATOR_UNLOCK_EVENT 0x04
#define ACTUATOR_LOCK_EVENT 0x08

typedef enum {
    STATE_UNKNOWN = 0x00,
    STATE_LOCKED,
    STATE_UNLOCKED,
    STATE_RUNNING
}actuator_state_t;

uint8_t g_actuator_action;

static actuator_state_t actuator_state_anlysis(uint16_t buf[], uint8_t size);

void actuator_init(void) 
{
    hall_adc_init();
    motor_init();
    g_actuator_action = ACTUATOR_NULL_EVENT;
}

void get_actuator_hall_value(uint16_t *hall_buf, uint8_t size)
{
    for (int i = 0; i < size; i++) {
        hall_buf[i] = hall_adc_get();
        print_log_t("%04X ", hall_buf[i]);
    }
    print_log_t("%04X \n");
}

#define PERIOD_TIME        100         //电机单次转动周期，单位ms
#define TRY_COUNT          20          //开锁或闭锁操作时，电机可尝试的周期数，单位次
#define SAMPLING_COUNT     16          //电机转动一周期后，霍尔传感器采样次数
//09CD 09CD 09CC 09CC 09CB 09CB 09CB 09CD 09CC 09CE 09CD 09CB 09CC 09CC 09CD 09CB       unlock
//0A59 0A59 0A58 0A57 0A59 0A59 0A5B 0A59 0A59 0A5B 0A57 0A57 0A59 0A56 0A57 0A57       lock     +- 5

static void actuator_unlock_process_test(void)
{
    uint16_t hall_value_buff[SAMPLING_COUNT] = {0};
    uint8_t try_count = TRY_COUNT;
    actuator_state_t current_state = STATE_UNKNOWN;

    while (current_state != STATE_UNLOCKED) {
        get_actuator_hall_value(hall_value_buff, SAMPLING_COUNT);
        current_state = actuator_state_anlysis(hall_value_buff, SAMPLING_COUNT);
        motor_right_on_pwm(PERIOD_TIME);
        delay_ms(2);
        try_count--;
        if (try_count == 0) {
            OPEN_FMQ();
            break;
        }
    }
}

static void actuator_lock_process_test(void)
{
    uint16_t hall_value_buff[SAMPLING_COUNT] = {0};
    uint8_t try_count = TRY_COUNT;
    actuator_state_t current_state = STATE_UNKNOWN;

    while (current_state != STATE_LOCKED) {
        get_actuator_hall_value(hall_value_buff, SAMPLING_COUNT);
        current_state = actuator_state_anlysis(hall_value_buff, SAMPLING_COUNT);       
        motor_right_on_pwm(PERIOD_TIME);
        delay_ms(2);
        try_count--;
        if (try_count == 0) {
            OPEN_FMQ();
            break;
        }
    }
}

void actuator_poll(void)
{
    if (g_actuator_action == ACTUATOR_UNLOCK_EVENT){
        //actuator_unlock_process();
        actuator_unlock_process_test();
        g_actuator_action = ACTUATOR_NULL_EVENT;
    } else if (g_actuator_action == ACTUATOR_LOCK_EVENT) {
        //actuator_lock_process();
        actuator_lock_process_test();
        g_actuator_action = ACTUATOR_NULL_EVENT;
    } else {
        g_actuator_action = ACTUATOR_NULL_EVENT;
    }
}

void actuator_unlock(void) {
    g_actuator_action = ACTUATOR_UNLOCK_EVENT;
}

void actuator_lock(void) {
    g_actuator_action = ACTUATOR_LOCK_EVENT;
}

#define UNLOCKED_HIGH_BYTE(data)      (((data & 0xFF00) <= 0x0900) ? 1 : 0)
#define UNLOCKED_LOW_BYTE(data)       (((data & 0x00FF) <= 0x00F0) ? 1 : 0)

#define LOCKED_HIGH_BYTE(data)        (((data & 0xFF00) >= 0x0A00) ? 1 : 0)
#define LOCKED_LOW_BYTE(data)         (((data & 0x00FF) >= 0x002A) ? 1 : 0)
static actuator_state_t actuator_state_anlysis(uint16_t buf[], uint8_t size)
{
    actuator_state_t state = STATE_UNKNOWN;
    uint8_t unlocked_count = 0;
    uint8_t locked_count = 0;

    for (uint8_t i = 0; i < size; i++) {
        if (UNLOCKED_HIGH_BYTE(buf[i])) {
            if (UNLOCKED_LOW_BYTE(buf[i])) {
                unlocked_count++;
            }
        } else if (LOCKED_LOW_BYTE(buf[i])) {
            if (LOCKED_HIGH_BYTE(buf[i])) {
                locked_count++;
            }
        }
    }

    if (unlocked_count >= (size - 2)) {
        return STATE_UNLOCKED;
    } else if (locked_count >= (size - 2)) {
        return STATE_LOCKED;
    }
    return STATE_RUNNING;
}
