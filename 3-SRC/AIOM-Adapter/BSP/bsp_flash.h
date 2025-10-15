#ifndef _BSP_FLASH_H
#define _BSP_FLASH_H

#include "gd32l23x.h"


/*           根据GD32L233芯片的Flash大小，修改下面这些宏定义           */
//Flash size:  256K   |  128K  |  64K   |  32K   
//Page  size: 0x1000  | 0x800  | 0x400  | 0x400  
//Page  num :   64    |   64   |  64    |  32
#define FMC_PAGE_NUM                 ((uint16_t)64U)
#define FMC_PAGE_SIZE                ((uint16_t)0x1000U)  

#define FMC_WRITE_START_ADDR         ((uint32_t)0x08000000U)
#define FMC_WRITE_END_ADDR           (FMC_WRITE_START_ADDR + (FMC_PAGE_SIZE * FMC_PAGE_NUM))

#define FMC_ADDR_CHECK(addr)         ((addr >= FMC_WRITE_START_ADDR) && (addr < FMC_WRITE_END_ADDR))


void fmc_erase_pages(uint32_t erase_addr);
uint8_t fmc_erase_pages_check(uint32_t erase_addr);
void fmc_write_word(uint32_t address, uint32_t data);
#endif
