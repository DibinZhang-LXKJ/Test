#include "key_test.h"
#include "delay.h"
#include "port_config.h"
#include "bsp_log.h"

#define DETECT_LED_CLK   RCU_GPIOA
#define DETECT_LED_PORT  GPIOA
#define DETECT_LED_PIN   GPIO_PIN_12

#define DETECT_IR_CLK   RCU_GPIOA
#define DETECT_IR_PORT  GPIOA
#define DETECT_IR_PIN   GPIO_PIN_11

#define DETECT_LED_ON        gpio_bit_reset(DETECT_LED_PORT, DETECT_LED_PIN)
#define DETECT_LED_OFF       gpio_bit_set(DETECT_LED_PORT, DETECT_LED_PIN)

#define DETECT_KEY          gpio_input_bit_get(DETECT_IR_PORT, DETECT_IR_PIN) // zero for having key

typedef enum {
    START = 0x00,
    SUN   = 0x01,
    IR    = 0x02,
    COM   = 0x03
}key_machine_t;

key_status_t key_status_poll()
{
    unsigned char temp;
    static key_machine_t mac = START;

    switch (mac) {
    case START:
        DETECT_LED_OFF;
        mac = SUN;
        break;
    case SUN:
        if (DETECT_KEY) {   //没阳光，进入IR测试
            DETECT_LED_ON;  //打开IR（红外LED）
            mac = IR;
            print_log_t("NO_SUN ");
        } else {   //低电平，有阳光
            DETECT_LED_OFF;
            mac = START;
            print_log_t("SUN ");
        }
        break;
    case IR:
        if (DETECT_KEY) {  //钥匙遮挡，进入COM测试
            DETECT_LED_OFF;
            mac = COM;
            print_log_t("KEY ");
        } else {            //无钥匙遮挡，进入START
            DETECT_LED_OFF;
            mac = START;
            print_log_t("NO_KEY ");
        }
        break;
    case COM:
        //detect key inject
        EnableChongDian;
        delay_2_5us();

        temp = CHECK_KEY_UART;
        if (!temp) {        //钥匙接触到位，正常通信
            DisnableChongDian;
            mac = START;
            print_log_t("KEY_SUCCESS ");
            return HAS_KEY;				
        } else {            //钥匙接触不良，无法通信
            DisnableChongDian;
            mac = START;
            print_log_t("KEY_ERR ");
            return NO_RIGHT;				
        }
        break;
    }
    
    return NO_KEY;
}

void key_status_init(void) 
{
    rcu_periph_clock_enable(DETECT_LED_CLK);

    gpio_mode_set(DETECT_LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, DETECT_LED_PIN);
    gpio_output_options_set(DETECT_LED_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, DETECT_LED_PIN);

    gpio_mode_set(DETECT_IR_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, DETECT_IR_PIN);

    DETECT_LED_OFF;
    //DETECT_LED_ON;
}


void key_test(void)
{
    DETECT_LED_ON;
    if (DETECT_KEY) {
        print_log_t("no_ir ");
    } else {
        print_log_t("ir ");
    }
    print_log_t("\n");
}

