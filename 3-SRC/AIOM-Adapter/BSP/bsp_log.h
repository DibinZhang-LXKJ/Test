#ifndef _BSP_LOG_H
#define _BSP_LOG_H

#define DEBUG_TEST

#ifdef DEBUG_TEST
	#define  print_log_t(fmt, args...) 	print_log(fmt, ##args)
#else
	#define  print_log_t(fmt, args...) 	(void)0
#endif

void segger_rtt_init(char * str);
void print_log(const char * sFormat, ...);

#endif
