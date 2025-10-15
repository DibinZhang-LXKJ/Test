#include "bsp_flash.h"


static void fmc_flags_clear(void)
{
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);             //置1，写保护未关闭时对FMC进行擦除操作
    fmc_flag_clear(FMC_FLAG_PGAERR);            //置位，则为写入错误
    fmc_flag_clear(FMC_FLAG_PGERR);
}

void fmc_erase_pages(uint32_t erase_addr )
{ 
    if ((erase_addr & FMC_WRITE_START_ADDR) == 0)
    {
        erase_addr += FMC_WRITE_START_ADDR;
    }
    if (FMC_ADDR_CHECK(erase_addr) == 0)
    {
        return;
    }
    //擦除对齐
    erase_addr = erase_addr & (~(FMC_PAGE_SIZE - 1));

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flags_clear();

    /* erase the flash pages */
    fmc_page_erase(erase_addr);
    fmc_flags_clear();

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

uint8_t fmc_erase_pages_check(uint32_t erase_addr)
{
    uint32_t i;
    uint32_t *ptrd;

    if ((erase_addr & FMC_WRITE_START_ADDR) == 0)
    {
        erase_addr += FMC_WRITE_START_ADDR;
    }
    if (FMC_ADDR_CHECK(erase_addr) == 0)
    {
        return 1;
    }
    //擦除对齐
    erase_addr = erase_addr & (~(FMC_PAGE_SIZE - 1));
    ptrd = (uint32_t *)(erase_addr);
    /* check flash whether has been erased */
    for(i = 0; i < FMC_PAGE_SIZE / 4; i++) {
        if(0xFFFFFFFF != (*ptrd)) {
            return 1;
            break;
        } else {
            ptrd++;
        }
    }
    return 0;
}


void fmc_write_word(uint32_t address, uint32_t data)
{
    if ((address & FMC_WRITE_START_ADDR) == 0)
    {
        address += FMC_WRITE_START_ADDR;
    }
    
    if (FMC_ADDR_CHECK(address) == 0)
    {
        return;
    }
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flags_clear();

    /* program flash */
    fmc_word_program(address, data);
    fmc_flags_clear();

    /* lock the main FMC after the program operation */
    fmc_lock();
}

