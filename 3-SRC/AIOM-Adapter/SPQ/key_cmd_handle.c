/********************************************************************
 * 
 * Կ�׶˶�Ӧ�Ĵ��ڱ��Ĵ������б�
 * 
 ********************************************************************/

#include "ext_var.h"
#include "cmd_handler.h"
//#include "functions.h"
#include <string.h>
#include "spq_function.h"
#include "actuator.h"

//��ѯԿ�׳��״̬�����Ӧ������
void on_key_response_state()
{
	g_recv_task_flag = FALSE; 
	
	if (g_uart_package0.data_len == 21)
	{
		g_temp_uint = UINT16_HL(g_uart_package0.data[3], g_uart_package0.data[4]);
		if(g_temp_uint != 0) 
		{
			g_recv_key_state = TRUE;
			g_key_charge_state = (g_temp_uint>405?KEY_CHARGE_FULL:KEY_CHARGING);
		}
		g_recv_key_time = g_now_time;
		memcpy(g_key_id,&g_uart_package0.data[5],16);
		
		#ifdef LOGGING
			send_to_pc((uchar*)&g_temp_uint,2);
		#endif
	}
}

//���������������
void on_key_response_package()
{
	SET_KEY_TALK(TRUE);
	g_temp_uint = get_package_data(&g_uart_package0, g_buf);
	send_to_pc(g_buf, g_temp_uint);
	g_query_key_timeout = QUERY_KEY_TIMEOUT;

	//�����ͳɹ����̵���˸
	if(g_recv_task_flag && g_uart_package0.head.command == CMD_ACK)
	{
		actuator_unlock();
		g_blink = TRUE;
		g_recv_task_flag = FALSE;
	}
}

//��ȡ����cmd���͵�����ĺ�����ַ
cmd_handler get_key_handler(int cmd)
{
	switch(cmd)
	{
		case CMD_QUERY_KEY_STATE:
			return &on_key_response_state;
		default:
			return &on_key_response_package;
	}
}
