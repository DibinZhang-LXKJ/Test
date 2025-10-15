#ifndef ACTUATOR_HEAD
#define ACTUATOR_HEAD
void actuator_init(void);
void actuator_unlock(void);
void actuator_lock(void);
void actuator_poll(void);
//extern g_actuator_status = BOOT;
#endif /* ifndef ACTUATOR_HEAD */
