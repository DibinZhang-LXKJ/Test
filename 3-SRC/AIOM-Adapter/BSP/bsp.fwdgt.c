#include "bsp_fwdgt.h"

static void fmc_flags_clear(void)
{
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);             //��1��д����δ�ر�ʱ��FMC���в�������
    fmc_flag_clear(FMC_FLAG_PGAERR);            //��λ����Ϊд�����
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
    
    //���ѡ���ֽ��п��Ź��Ƿ�����ΪӲ������ģʽ
    user_option_byte = ob_user_get();
    //�����������Ź�Ӳ��������ѡ���ֽڣ�ʹ���ϵ�ʱ���Զ������������Ź�
//    if ((user_option_byte & 0x01) == 1)
//    {//�򿪶������Ź�Ӳ��ʹ�ܹ���, ��OPTION_BYTES_ADDRESS���ĵ�0λ
        //�����0λΪ0�����ϵ���Զ��򿪶������Ź�
    uint8_t OB_FWDGT_BIT = mode;
    uint8_t OB_DEEPSLEEP_BIT = (user_option_byte & 0x02) >> 1;
    uint8_t OB_STDBY_BIT = (user_option_byte & 0x04) >> 2;
    uint8_t OB_BOR_TH_BITS = user_option_byte >> 5;

//        /* unlock the flash program/erase controller */
//        fmc_unlock();
//        ob_unlock();

//        fmc_flags_clear();

//        //�ú��������˶�FMC�Ĳ�������
//        if (FMC_READY == ob_user_write(OB_FWDGT_BIT, OB_DEEPSLEEP_BIT, OB_STDBY_BIT, OB_BOR_TH_BITS))
//        {//ѡ���ֽ����óɹ�
//            #ifdef DEBUG_TEST
//            print_log("Option byte set success\n");
//            #endif
//            NVIC_SystemReset();
//        }
//        else
//        {//ѡ���ֽ�����ʧ��
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

