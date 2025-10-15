#include "iap.h"
#include "iap_config.h"
#include "bsp_cpu.h"
#include "bsp_flash.h"
#include "port_config.h"

//#define BOOTLOADER_START_ADDR 0x08010000
typedef void (*IAP_Entry)(void);
IAP_Entry entry_bootloader;             //不能创建成局部变量


//先擦除后写入，同时会校验写入是否成功
//成功：0，失败：1
static uint8_t write_to_flash(uint32_t addr, uint8_t* data, uint32_t len);

void write_update_flag(void)
{
    fmc_erase_pages(INTERUPT_CODE_START);
    fmc_write_word(INTERUPT_CODE_START, (uint32_t)0x88777788);		//更新标志位
}

void clear_update_flag(void)
{
    fmc_erase_pages(INTERUPT_CODE_START);
}

//检测是否需要更新程序
uint8_t check_update(void)
{
    uint32_t* update_are_ptr = (uint32_t*)INTERUPT_CODE_START;
    
    if(*update_are_ptr == 0x88777788) //检查更新标志位
    {
        return 1; //需要更新
    }   
    return 0;
}


void jump_bootloader(void)
{
    enter_critical_section();
    if ((*((volatile uint32_t*)BOOTLOADER_START_ADDR) & 0x2FFE0000) == 0x20000000) //检查栈顶指针是否正确
    {
        entry_bootloader = (IAP_Entry)(*(volatile uint32_t*)(BOOTLOADER_START_ADDR + 4)); //获取复位函数入口地址
        
        //更改MSP指针指向新的向量地址
        __set_MSP(*(volatile uint32_t*)BOOTLOADER_START_ADDR);
        
        //跳转到复位函数
        entry_bootloader();
    }
    exit_critical_section();
}


//自动更新充电座程序
/*
	适配器程序从PROGRAM_CODE_START开始，
	更新的数据存放在UPDATE_CODE_START中，
	数据格式为
	
	-----------------
	数据长度N（2字节）
	数据段（N字节）
	-----------------
	
	更新主要是把从UPDATE_CODE_START + 4 开始的数据写入到 PROGRAM_CODE_START 地址中
*/
void live_update(void)
{ 
    uint32_t g_update_len;						    //更新数据长度
    //读写指针
    //char* write_ptr;
    char* read_ptr;

    //g_temp_uint = 0;
   // g_temp_uchar = 0;
   // write_ptr = (char*)UPDATE_CODE_START; 
    read_ptr = (char*)UPDATE_CODE_START;

    //write_to_flash(UPDATE_CODE_START, (uint8_t *)&g_update_len, 4); //擦除程序
    //g_update_len = 0;
    g_update_len = *((uint32_t *)read_ptr);         //获取更新数据长度

    SetWatchDog;
    if (1 == write_to_flash(PROGRAM_CODE_START, (uint8_t *)read_ptr, g_update_len)) 
    {
        //写入失败
        mcu_software_reset();
    } 
}


//addr: 0 ~ 0x40000; len：写入的数据大小，单位byte
//返回：1：写入失败     0：写入成功
static uint8_t write_to_flash(uint32_t addr, uint8_t* data, uint32_t len)
{
    //falsh连续写入功能
    uint32_t i;
    uint32_t write_word= 0;
    uint32_t* read_word = (uint32_t*)addr;
    uint32_t erase_count = 0;
    uint32_t surplus_len = 0;
    
    //擦除块数计算
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
    //擦除flash
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
    
    
    //写入数据
    for (i = 0; i < len; i += 4)
    {
        write_word = *(uint32_t*)(data + i);
        fmc_write_word(addr + i, write_word);
        read_word = (uint32_t *)(addr + i);
        //数据对比
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

