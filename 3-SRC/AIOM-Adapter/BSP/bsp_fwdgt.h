#ifndef _BSP_FWDGT_H
#define _BSP_FWDGT_H


#include "gd32l23x.h"

typedef enum{
    HARDWARE = 0,
    SOFTWARE = 1
}FWDGT_START_MODE;

void fwdgt_choose_start_mode(FWDGT_START_MODE mode);
void feed_fwdgt(void);

#endif
