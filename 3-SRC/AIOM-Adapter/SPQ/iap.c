#include "iap.h"
#include "iap_config.h"
#include "bsp_cpu.h"
#include "bsp_flash.h"
#include "port_config.h"

//#define BOOTLOADER_START_ADDR 0x08010000
typedef void (*IAP_Entry)(void);
IAP_Entry entry_bootloader;             //���ܴ����ɾֲ�����


//�Ȳ�����д�룬ͬʱ��У��д���Ƿ�ɹ�
//�ɹ���0��ʧ�ܣ�1
static uint8_t write_to_flash(uint32_t addr, uint8_t* data, uint32_t len);

void write_update_flag(void)
{
    fmc_erase_pages(INTERUPT_CODE_START);
    fmc_write_word(INTERUPT_CODE_START, (uint32_t)0x88777788);		//���±�־λ
}

void clear_update_flag(void)
{
    fmc_erase_pages(INTERUPT_CODE_START);
}

//����Ƿ���Ҫ���³���
uint8_t check_update(void)
{
    uint32_t* update_are_ptr = (uint32_t*)INTERUPT_CODE_START;
    
    if(*update_are_ptr == 0x88777788) //�����±�־λ
    {
        return 1; //��Ҫ����
    }   
    return 0;
}


void jump_bootloader(void)
{
    enter_critical_section();
    if ((*((volatile uint32_t*)BOOTLOADER_START_ADDR) & 0x2FFE0000) == 0x20000000) //���ջ��ָ���Ƿ���ȷ
    {
        entry_bootloader = (IAP_Entry)(*(volatile uint32_t*)(BOOTLOADER_START_ADDR + 4)); //��ȡ��λ������ڵ�ַ
        
        //����MSPָ��ָ���µ�������ַ
        __set_MSP(*(volatile uint32_t*)BOOTLOADER_START_ADDR);
        
        //��ת����λ����
        entry_bootloader();
    }
    exit_critical_section();
}


//�Զ����³��������
/*
	�����������PROGRAM_CODE_START��ʼ��
	���µ����ݴ����UPDATE_CODE_START�У�
	���ݸ�ʽΪ
	
	-----------------
	���ݳ���N��2�ֽڣ�
	���ݶΣ�N�ֽڣ�
	-----------------
	
	������Ҫ�ǰѴ�UPDATE_CODE_START + 4 ��ʼ������д�뵽 PROGRAM_CODE_START ��ַ��
*/
void live_update(void)
{ 
    uint32_t g_update_len;						    //�������ݳ���
    //��дָ��
    //char* write_ptr;
    char* read_ptr;

    //g_temp_uint = 0;
   // g_temp_uchar = 0;
   // write_ptr = (char*)UPDATE_CODE_START; 
    read_ptr = (char*)UPDATE_CODE_START;

    //write_to_flash(UPDATE_CODE_START, (uint8_t *)&g_update_len, 4); //��������
    //g_update_len = 0;
    g_update_len = *((uint32_t *)read_ptr);         //��ȡ�������ݳ���

    SetWatchDog;
    if (1 == write_to_flash(PROGRAM_CODE_START, (uint8_t *)read_ptr, g_update_len)) 
    {
        //д��ʧ��
        mcu_software_reset();
    } 
}


//addr: 0 ~ 0x40000; len��д������ݴ�С����λbyte
//���أ�1��д��ʧ��     0��д��ɹ�
static uint8_t write_to_flash(uint32_t addr, uint8_t* data, uint32_t len)
{
    //falsh����д�빦��
    uint32_t i;
    uint32_t write_word= 0;
    uint32_t* read_word = (uint32_t*)addr;
    uint32_t erase_count = 0;
    uint32_t surplus_len = 0;
    
    //������������
    surplus_len = (addr & (~(FMC_PAGE_SIZE - 1))) + FMC_PAGE_SIZE - addr;
    if ( surplus_len > len )
    {
        erase_count = 1;
    }
    else 
    {
        erase_count = (len - surplus_len + FMC_PAGE_SIZE - 1) / FMC_PAGE_SIZE + 1;
    }
    
    enter_critical_section();
    //����flash
    for (i = 0; i < erase_count; i++)
    {
        fmc_erase_pages(addr + i * FMC_PAGE_SIZE);
        if (fmc_erase_pages_check(addr + i * FMC_PAGE_SIZE))
        {
            exit_critical_section();
            return 1;
        }
        SetWatchDog;
    }
    
    
    //д������
    for (i = 0; i < len; i += 4)
    {
        write_word = *(uint32_t*)(data + i);
        fmc_write_word(addr + i, write_word);
        read_word = (uint32_t *)(addr + i);
        //���ݶԱ�
        if (*read_word != write_word)
        {
            exit_critical_section();
            return 1;
        }
        SetWatchDog;
    }
    exit_critical_section();
    return 0;
}

