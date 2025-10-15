#include "app_main.h"

static void key_rx_handler(uint8_t rx_data);
static void pc_rx_handler(uint8_t rx_data);
void key_state_timer_handler(void);
void led_state_timer_handler(void);
void duanlu_exti_handler(void);
static void bsp_config(void);
static void switch_system_clock_to_36m_hxtal(void);

volatile Queue512 pcBuf;
volatile Queue512 keyBuf;

int main(void)
{
    uint8_t data;
//    switch_system_clock_to_36m_hxtal();
    //confiure FWDGT counter clock: 32KHz(IRC32K) / 64 = 0.5 KHz */
    fwdgt_config(6 * 500, FWDGT_PSC_DIV64);
    fwdgt_enable();
  //  fwdgt_choose_start_mode(SOFTWARE);
    bsp_port_init(WATCHDOG);
    SetWatchDog;
    
    #ifdef DEBUG_TEST
    segger_rtt_init("\nDemo start !\n");  // 调用这句进行初始化以及打印开机信息【Demo start！】
    if(RESET != rcu_flag_get(RCU_FLAG_SWRST)) {
        print_log("\nSoftware reset generated\n");
    }
    print_log("This is APP\n");
    #endif
  
    enter_critical_section();
    //检查更新
    #ifdef UPDATE_CODE
    if (check_update() == TRUE)
    {
        //跳转到固件升级程序
        jump_bootloader();
    }
    #endif
    init_args();        //初始化时间参数
    //外设初始化
    reg_handler();
    QueueEmpty(pcBuf);
    QueueEmpty(keyBuf);
    bsp_config();
    send_buff_clear();
    exit_critical_section();
    actuator_init();
    key_status_init();
    SetWatchDog;
    while (1) {
        actuator_poll();
        send_buff_start();
        if (count_10ms % 100 == 0) {
            //actuator_unlock();
            //print_log_t("\n");
        }
        if ((QueueDataLen(pcBuf) > 0) || (QueueDataLen(keyBuf) > 0)) {
            SetWatchDog;
            if (QueueDataLen(pcBuf) > 0) {
                //处理PC端数据
                QueueDataOut(pcBuf, &data);
                handle_pc_data(data);
            }
            if (QueueDataLen(keyBuf) > 0) {
                //处理钥匙端数据
                QueueDataOut(keyBuf, &data);
                handle_key_data(data);
            }
        } else {
            enter_low_power();       //睡眠模式，触发任何中断都会唤醒芯片
        }
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
    bsp_port_init(FMQ);
    bsp_port_init(CHARGER);
    
    DisnableChongDian;
    SetWatchDog;
    
    bsp_uart_init(KEY_UART, 57600, 0);
    uart_register_callback(KEY_UART, key_rx_handler);       //注册中断回调处理函数
    bsp_uart_init(PC_UART, 57600, 0);
    uart_register_callback(PC_UART, pc_rx_handler);

    SetWatchDog;
    LED1ONRed;
    LED3ONGr;
    delay_ms(100);
    SetWatchDog;
    
    //短路检查中断初始化
    bsp_exti_init(CHECK, 0);
    exti_register_callback(CHECK, duanlu_exti_handler);
    
    //定时器中断 
    bsp_timer_init(LED_TIMER, 10, 1);
    timer_register_callback(LED_TIMER, led_state_timer_handler);//handle_led_state);
    bsp_timer_init(STATE_TIMER, 10, 1);        //10ms
    timer_register_callback(STATE_TIMER, key_state_timer_handler);//handle_key_state);
    SetWatchDog;
}

//钥匙端数据接收处理
static void key_rx_handler(uint8_t rx_data)
{
    exit_low_power();
    SetWatchDog;
    QueueDataIn(keyBuf, &rx_data, 1);
}

//PC端数据接收处理
static void pc_rx_handler(uint8_t rx_data)
{
    exit_low_power();
    SetWatchDog;
    QueueDataIn(pcBuf, &rx_data, 1);
}

//钥匙端状态更新
void key_state_timer_handler(void)
{
    exit_low_power();
    count_10ms++;
    SetWatchDog;
    handle_key_state();
}

//LED状态更新
void led_state_timer_handler(void)
{
    exit_low_power();
    SetWatchDog;
    handle_led_state();
}

//短路检测
void duanlu_exti_handler(void)
{
    exit_low_power();
    SetWatchDog;
    handle_duanlu();
}

#define RCU_MODIFY_DE_2(__delay)  do{                                     \
                                      volatile uint32_t i,reg;            \
                                      if(0 != __delay){                   \
                                          /* Insert a software delay */   \
                                          for(i=0; i<__delay; i++){       \
                                          }                               \
                                          reg = RCU_CFG0;                 \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV2;     \
                                          /* AHB = SYSCLK/2 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0; i<__delay; i++){      \
                                          }                              \
                                          reg = RCU_CFG0;                \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV4;     \
                                          /* AHB = SYSCLK/4 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0; i<__delay; i++){      \
                                          }                              \
                                      }                                  \
                                  }while(0)
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0; i<time*10; i++){
    }
}

static void switch_system_clock_to_36m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;

    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    /* select IRC16M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC16M);
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    _soft_delay_(10);
    rcu_deinit();
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while(1) {
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT(1);

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* PLL = HXTAL / 2 * 6 = 24 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_6);
    RCU_CFG1 &= ~RCU_CFG1_PREDV;
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL6);
    RCU_CFG1 |= RCU_PLL_PREDV2; 
    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLL;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLL)) {
    }
}

