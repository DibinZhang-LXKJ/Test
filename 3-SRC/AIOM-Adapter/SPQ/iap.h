#ifndef _IAP_H_
#define _IAP_H_


#include "gd32l23x.h"

void write_update_flag(void);      //写更新标志
void clear_update_flag(void);      //清除更新标志
uint8_t check_update(void);        //查看是否更新
void live_update(void); //__attribute__((section("LIVE_UPDATE_CODE")));
void jump_bootloader(void);

#endif /* _IAP_H */

