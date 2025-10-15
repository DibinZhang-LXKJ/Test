/** Bootloader应用程序 **/

#include "boot_main.h"
#include "bsp_port.h"
#include "port_config.h"
#include "bsp_timer.h"

#define DEBUG_TEST 
static void bsp_config(void);
static void led_state_timer_handler(void);

volatile uint32_t time;

int main(void)
{
    uint8_t data = 1;

//    enter_critical_section();
//    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00010000);
    #ifdef DEBUG_TEST
        segger_rtt_init("\nDemo start !\n");  // 调用这句进行初始化以及打印开机信息【Demo start！】
        print_log("This is bootloader\n");
    #endif
    bsp_config();
    exit_critical_section();

    //检查更新
    if (check_update() == 1)
    {
        live_update();
    }
    SetWatchDog;
    LED1ONGr;
    LED2ONGr;
    LED3ONGr;
    clear_update_flag();
    mcu_software_reset();
    while(1)
    {
        SetWatchDog;
//        if (time == 100)
//        {
//            time = 0;
//            if (data)
//            {
//                data = 0;
//                LED1ONGr;
//                LED2ONGr;
//                LED3ONGr;
//                print_log("This is bootloader\n");
//            }
//            else
//            {
//                data = 1;
//                LED1OFF;
//                LED2OFF;
//                LED3OFF;
                mcu_software_reset();
//            }
//        }
    }

}

static void bsp_config(void)
{
    clear_reset_flag();
    
    //端口初始化
    bsp_port_init(LED1_G);
    bsp_port_init(LED1_R);
    bsp_port_init(LED2_G);
    bsp_port_init(LED2_R);
    bsp_port_init(LED3_G);
    bsp_port_init(LED3_R);
    bsp_port_init(WATCHDOG);
    
    
    //定时器中断 
    bsp_timer_init(LED_TIMER, 10, 1);
    timer_register_callback(LED_TIMER, led_state_timer_handler);//handle_led_state);

    SetWatchDog;
}

static void led_state_timer_handler(void)
{
    time++;
}

