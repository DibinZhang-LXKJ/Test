#ifndef KEY_EXISTENCE__
#define KEY_EXISTENCE__

#include "gd32l23x.h"

typedef enum {
    INIT    =  0x00,
    NO_KEY  = 0x01,
    HAS_KEY = 0x02,
    NO_RIGHT = 0xE1,
}key_status_t;

key_status_t key_status_poll(void);
void key_status_init(void);
void key_test(void);
#endif /* ifndef KEY_EXISTENCE__ */
