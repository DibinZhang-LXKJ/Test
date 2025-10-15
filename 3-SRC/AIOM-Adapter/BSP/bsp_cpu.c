#include "bsp_cpu.h"

void enter_low_power(void)
{
    pmu_to_sleepmode(PMU_LDNP_LOWDRIVE, WFI_CMD);
}

void exit_low_power(void)
{
    //pmu_to_sleepmode(PMU_LDNP_NORMALDRIVE, WFI_CMD);
    /* clear lowpower mode and low drive bits */
    PMU_CTL0 &= ~((uint32_t)(PMU_CTL0_LPMOD | PMU_CTL0_LDNP));
}

//软件复位
void mcu_software_reset(void)
{
    /* set FAULTMASK */
    __set_PRIMASK(1);
    NVIC_SystemReset();
}

//清除复位标志
void clear_reset_flag(void)
{
    /* clear all reset flags */
    rcu_all_reset_flag_clear();
}

uint32_t primask = 0;
//进入临界区
void enter_critical_section(void)
{
    // 关闭所有中断（返回当前中断状态用于恢复）
    
    primask = __get_PRIMASK();
    __disable_irq();
}

//    __set_PRIMASK(1);

//退出临界区
void exit_critical_section(void)
{
    __set_PRIMASK(primask);
}

