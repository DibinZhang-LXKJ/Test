#include "bsp_fwdgt.h"

static void fmc_flags_clear(void)
{
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);             //置1，写保护未关闭时对FMC进行擦除操作
    fmc_flag_clear(FMC_FLAG_PGAERR);            //置位，则为写入错误
    fmc_flag_clear(FMC_FLAG_PGERR);
}


void fwdgt_choose_start_mode(FWDGT_START_MODE mode)
{
    uint8_t user_option_byte;
    uint8_t op_byte[4];
    uint8_t i;
    
    for(i = 0U; i < 4U; i++) {
        op_byte[i] = OP_BYTE(i);
    }
    
    //检查选项字节中看门狗是否设置为硬件启动模式
    user_option_byte = ob_user_get();
    //开启独立看门狗硬件启动的选项字节，使其上电时会自动启动独立看门狗
//    if ((user_option_byte & 0x01) == 1)
//    {//打开独立看门狗硬件使能功能, 在OPTION_BYTES_ADDRESS处的第0位
        //如果第0位为0，则上电后自动打开独立看门狗
    uint8_t OB_FWDGT_BIT = mode;
    uint8_t OB_DEEPSLEEP_BIT = (user_option_byte & 0x02) >> 1;
    uint8_t OB_STDBY_BIT = (user_option_byte & 0x04) >> 2;
    uint8_t OB_BOR_TH_BITS = user_option_byte >> 5;

//        /* unlock the flash program/erase controller */
//        fmc_unlock();
//        ob_unlock();

//        fmc_flags_clear();

//        //该函数包含了对FMC的擦除操作
//        if (FMC_READY == ob_user_write(OB_FWDGT_BIT, OB_DEEPSLEEP_BIT, OB_STDBY_BIT, OB_BOR_TH_BITS))
//        {//选项字节设置成功
//            #ifdef DEBUG_TEST
//            print_log("Option byte set success\n");
//            #endif
//            NVIC_SystemReset();
//        }
//        else
//        {//选项字节设置失败
//            #ifdef DEBUG_TEST
//            print_log("Option byte set failed\n");
//            #endif
//        }
//    }
}

void feed_fwdgt(void)
{
    /* reload FWDGT counter */
    fwdgt_counter_reload();
}

