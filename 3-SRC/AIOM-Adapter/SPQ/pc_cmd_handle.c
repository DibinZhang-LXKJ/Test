/********************************************************************
 * 
 * pc�˶�Ӧ�Ĵ��ڱ��Ĵ������б�
 * 
 ********************************************************************/

#include "ext_var.h"
#include "cmd_handler.h"
#include <string.h>
#include "spq_function.h"
#include "iap_config.h"
#include "port_config.h"
#include "bsp_cpu.h"
#include "bsp_flash.h"
#include "bsp_uart.h"
#include "bsp_log.h"

#include "delay.h"
#include "actuator.h"
#include "iap.h"

//������������ַ
void on_set_adapter_address()
{
	uint32_t u32_Temp;

	if(GET_KEY_AT && g_uart_package1.data_len == 2)//keyAddressͨ����ѯ��ȡ
	{
		tempUartUint1 = UINT16_HL(g_uart_package1.data[0], g_uart_package1.data[1]);
		if(tempUartUint1 >= MIN_ADAPTER_ADDRESS && tempUartUint1 <= MAX_ADAPTER_ADDRESS)
		{
			//�����豸ID
			for (g_temp_uint = 0; g_temp_uint < 16; g_temp_uint++)
			{
				uartData1[g_temp_uint] = READ_DRESS_BYTE(LOCAL_ID_OF_FLASH_SAVE + g_temp_uint);
			}
			//������ַ
			fmc_erase_pages(LOCAL_ADDR_OF_FLASH_SAVE);
			//д��ַ
			fmc_write_word(LOCAL_ADDR_OF_FLASH_SAVE, (uint32_t)tempUartUint1);
			//дID
			fmc_contiune_write(LOCAL_ID_OF_FLASH_SAVE, uartData1, 16);
//			for (g_temp_uint = 0; g_temp_uint < 16; g_temp_uint += 4)       //���⣺������4byteʱ��д�����
//			{
//				u32_Temp = uartData1[g_temp_uint + 3] << 24 
//						| uartData1[g_temp_uint + 2] << 16 
//						| uartData1[g_temp_uint + 1] << 8 
//						| uartData1[g_temp_uint];
//				fmc_write_word(LOCAL_ID_OF_FLASH_SAVE + g_temp_uint, u32_Temp);
//			}
			//��ӦACK
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
	}
}

//��ȡ\����������ID
void on_kiss_adapter_id()
{
	uint32_t u32_Temp;

	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//���Ϊ16��Ϊ�����ж����ݳ��ȣ����Ϊ0��Ϊ��ȡ��������
		if(g_uart_package1.data_len == 0)//��ȡ������ID������ 
		{
			for (g_temp_uint = 0; g_temp_uint < 16; g_temp_uint++)
			{
				uartData1[g_temp_uint] = READ_DRESS_BYTE(LOCAL_ID_OF_FLASH_SAVE + g_temp_uint);
			}
			g_package.head.control = 0x60;
			g_package.head.command = CMD_RW_ADAPTER_ID;
			g_package.head.src_address = g_uart_package1.head.dest_address;
			g_package.head.dest_address = g_uart_package1.head.src_address;
			g_package.data = uartData1;
			g_package.data_len = 16;
			g_temp_uint = get_package_data(&g_package, g_buf);
			send_to_pc(g_buf,g_temp_uint); 
		}
		else if(g_uart_package1.data_len == 16)//д��������ID
		{
			//�����豸��ַ
			tempUartUint1 = LOCAL_ADDRESS;

			fmc_erase_pages(LOCAL_ADDR_OF_FLASH_SAVE);
			//д�豸��ַ
			fmc_write_word(LOCAL_ADDR_OF_FLASH_SAVE, (uint32_t)tempUartUint1);
			//д�豸ID
			fmc_contiune_write(LOCAL_ID_OF_FLASH_SAVE, g_uart_package1.data, 16);
	
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
		else
			send_nack_from_package(PC_UART, g_uart_package1, g_buf);
	}
}

//��λ��������ַ��ID
void on_reset_adapter_id()
{
	if(GET_KEY_AT)//keyAddressͨ����ѯ��ȡ
	{
		//����
		fmc_erase_pages(LOCAL_ADDR_OF_FLASH_SAVE);
		send_ack_from_package(PC_UART, g_uart_package1, g_buf);
	}
}


//���߸�������
void on_update_command()
{
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//
		if(g_uart_package1.data_len != 1 ||((g_temp_uchar = g_uart_package1.data[0]) != 0 && g_temp_uchar != 0xFF))
		{
			send_nack_from_package(PC_UART, g_uart_package1, g_buf);
		}
		else
		{
			//���¿�ʼ
			if(g_temp_uchar == 0)
			{
				#ifdef DEBUG_TEST
				print_log("SPQ Update Start\n");
				#endif
				//��λ
				g_update_step = UPDATE_DATA;
				g_update_index = 4;				//ǰ4λ���ڴ�����ݰ�����
				g_current_package_index = 0;
				send_ack_from_package(PC_UART, g_uart_package1, g_buf);
			}
			else
			{
				#ifdef DEBUG_TEST
				print_log("SPQ Update end\n");
                #endif
				//���½���
				if(g_update_step == UPDATE_END)//�������ݽ������
				{
					send_ack_from_package(PC_UART, g_uart_package1, g_buf);
					g_update_step = UPDATE_OFF;
					g_update_index = 0xFFFF;
					delay_ms(1); 
                    mcu_software_reset();   //��λ
				}
				else
				{
					g_update_step = UPDATE_OFF;
					send_nack_from_package(PC_UART, g_uart_package1, g_buf);
				}
			}
		}
	}
}
/************************  ע��32λ�ڴ����  ****************************/
//���ո���������
void on_update_program()
{
    uint16_t i;
	uint32_t u32_Temp;
	uint8_t data_32_index;
	uint8_t erase_page_count;
	static uint8_t data_buff[4];
	static uint8_t data_len = 0;

	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//��ȡ��ǰ����
		g_temp_uchar = GET_PACKAGE_INDEX(g_uart_package1);

		//��֤���ݣ�g_update_stepΪ��ǰ���裬g_current_package_indexΪ�����հ�����
		if(g_update_step != UPDATE_DATA || g_update_index == 0xFFFF || g_current_package_index < g_temp_char)
		{
			g_update_step = UPDATE_OFF;
			g_current_package_index=0;
			send_nack_from_package(PC_UART, g_uart_package1, g_buf);
		}
		else
		{
			if(g_current_package_index == g_temp_uchar)
			{
				g_current_package_index++;
				if (g_current_package_index == 32)
				{
					g_current_package_index = 0;
				}

				//������Ҫ��������ҳ��������Flash����
				erase_page_count = (g_uart_package1.data_len + FMC_PAGE_SIZE - 1) / FMC_PAGE_SIZE;
				for (i = 0; i < erase_page_count; i++)
				{
					fmc_erase_pages(UPDATE_CODE_START + i * FMC_PAGE_SIZE);
				}

				//��������ǰ���ж�
				enter_critical_section();			//�����ٽ���

				//�������ݰ����ʱ245���ֽڣ���Ҫ�������ݰ����Ȳ���4�ֽڵ����������µ�����������
				for (i = 0; i < g_uart_package1.data_len;)			
				{
					SetWatchDog;
					u32_Temp = 0;		//ÿ�δ�������ǰ����
					if (data_len)
					{
						for (data_32_index = 0; data_32_index < 4; data_32_index++)
						{
							if (data_len)
							{
								u32_Temp += data_buff[data_32_index] << (8 * data_32_index);
								data_len--;
							}
							else
							{
								u32_Temp += g_uart_package1.data[i] << (8 * data_32_index);
								i++;
							}
						}
						fmc_write_word(UPDATE_CODE_START + g_update_index, u32_Temp);
						g_update_index += 4;
					}
					else if ((i + 4 <= g_uart_package1.data_len) || IS_END_PACKAGE(g_uart_package1))
					{
						u32_Temp = g_uart_package1.data[i + 3] << 24 
							| g_uart_package1.data[i + 2] << 16 
							| g_uart_package1.data[i + 1] << 8 
							| g_uart_package1.data[i];
						fmc_write_word(UPDATE_CODE_START + g_update_index, u32_Temp);
						g_update_index += 4;
						i += 4;
					}
					else			//ʣ�����ݲ���4�ֽ�
					{
						data_len = 0;
						while (i < g_uart_package1.data_len)
						{
							data_buff[data_len] = g_uart_package1.data[i];
							i++;
							data_len++;
						}
					}
				}/* for end */
				data_len = 0;
			}/*if end */
			
			//���������һ���󣬽����ݳ���д�����ݶ�ͷ�������ı䵱ǰ����Ϊ�������ж�������
			if(IS_END_PACKAGE(g_uart_package1))
			{
				//write_flash(UPDATE_CODE_START, g_update_index>>8);
				//write_flash((UPDATE_CODE_START+1), g_update_index);
				fmc_write_word(UPDATE_CODE_START, (uint32_t)g_update_index);
				g_update_step = UPDATE_INT_VECTOR;
			}
			exit_critical_section();			//�˳��ٽ���
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
	}
}

//�����ж�������
void on_update_interrupt_table()
{
	uint32_t u32_Temp;

	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		if(g_update_step == UPDATE_INT_VECTOR)
		{
			if(IS_FIRST_PACKAGE(g_uart_package1))
				g_update_index = 0;
		 
			//������ɺ󱣴������������ݶΣ������õ�ǰ����Ϊ���ս���
			enter_critical_section();			//�����ٽ���

			write_update_flag();		//д������±�־λ		//���±�־λ
			exit_critical_section();			//�˳��ٽ���

			if(IS_END_PACKAGE(g_uart_package1))
				g_update_step = UPDATE_END;
			
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
		else
		{
			g_update_step = UPDATE_OFF;
			send_nack_from_package(PC_UART, g_uart_package1, g_buf);
		} 
	}
	else 
		on_other_command();
}

//����Կ�׵�ַ
void on_set_key_address()
{
	//���Կ����λ�ҵ�ַһ�£���ת����Կ��
	if(GET_KEY_AT && g_uart_package1.data_len == 2)
	{
		g_temp_uint = UINT16_HL(g_uart_package1.data[0], g_uart_package1.data[1]);
		if(g_temp_uint >= MIN_KEY_ADDRESS && g_temp_uint <= MAX_KEY_ADDRESS )
		{
			g_key_address = g_temp_uint;
			g_temp_uint = get_package_data(&g_uart_package1, g_buf);
			send_to_key(g_buf, g_temp_uint);  
		}
	}
}

//����Կ��ID
void on_set_key_id()
{
	//���Կ����λ�ҵ�ַһ�£���ת����Կ��
	if(GET_KEY_AT && g_uart_package1.data_len == 16)
	{
		g_temp_uint = get_package_data(&g_uart_package1, g_buf);
        memcpy(g_key_id, &g_uart_package1.data[0], 16);
		send_to_key(g_buf, g_temp_uint); 
		
	}
}

//��ѯԿ��״̬
void on_get_key_state()
{
	//��ⱨ���Ƿ��Ƿ��͸��Լ�
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//���ñ������ݣ�Ȼ��ת����PC
		SET_SINGLE_PACKAGE(g_package)
		g_package.head.command = CMD_QUERY_KEY_STATE;
		g_package.head.src_address = g_uart_package1.head.dest_address;
		g_package.head.dest_address = g_uart_package1.head.src_address;
		g_key_state_data[0] = (GET_KEY_AT == TRUE ? KEY_ON : KEY_OFF);
		FLOP_UINT(g_key_state_data,g_key_address,1);
		FLOP_UINT(g_key_state_data,g_key_charge_state,3);
		if(g_key_address == DEFAULT_KEY_ADDRESS)g_key_state_data[0] = KEY_OFF;
		g_package.data = g_key_state_data;
		g_package.data_len = sizeof(g_key_state_data);
		g_temp_uint = get_package_data(&g_package,g_buf);
		send_to_pc(g_buf,g_temp_uint); 
	}
}

//ͬ����������
void on_sync_end()
{	
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_KEY)
	{ 
		//������յ�ַ�Ǳ�����Կ�ף����ñ�־λ������ӦACK
		g_key_sync = FALSE;
		g_query_key_timeout = 0;
		send_ack_from_package(PC_UART, g_uart_package1, g_buf);
	}
}

//��ѯ�豸��Ϣ
void on_get_device_info()
{
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//���ñ������ݣ�Ȼ��ת����PC
		SET_SINGLE_PACKAGE(g_package)
		g_package.head.command = CMD_QUERY_DEVICE_INFO;	
		g_package.head.src_address = g_uart_package1.head.dest_address;
		g_package.head.dest_address = g_uart_package1.head.src_address;
		uartData1[0] = DEVICE_ADAPTER;
		uartData1[1] = VER_MAJOR;
		uartData1[2] = VER_MINOR;
		uartData1[3] = VER_REV;
		g_package.data = uartData1;
		g_package.data_len = 4;
		g_temp_uint = get_package_data(&g_package,g_buf);
		send_to_pc(g_buf,g_temp_uint); 
	}
	 else 
		on_other_command();
}

//��������
void on_other_command()
{
	tempUartUint1 = g_uart_package1.head.dest_address;
	if(tempUartUint1 == g_key_address || tempUartUint1 == LOCAL_ADDRESS)
	{
		//�����ݰ�Ŀ�ĵ�ַΪ�������ַ(��ѯʹ��)��Կ�׵�ַ(ֱ��ת��)ʱ���͵�Կ��
		//��63��Ϊ��ʱ����ʱһ��ʱ����ٷ���ʹ��Timer_B��
		if(CHECK_CHARGER)
		{
			EnableChongDian;
			disable_uart_rx_interrupt(KEY_USART);		//�رմ���0�жϣ���key_uart�����ж�
			send_delay_from_uart1 = 50;
		}
		else if(send_delay_from_uart1 == 0xFFFF)//�����ǰ����ʱת������������ת����Կ��
		{
			g_temp_uint = get_package_data(&g_uart_package1, g_buf);
			//IE1 &= ~URXIE0;
			disable_uart_rx_interrupt(KEY_USART);		//�رմ���0�жϣ���key_uart�����ж�
			send_to_key(g_buf, g_temp_uint); 
			enable_uart_rx_interrupt(KEY_USART);		    //��������0�жϣ���key_uart�����ж�

			
			switch(g_uart_package1.head.command)
			{
				case CMD_SEND_TASK:
				{
					if((g_uart_package1.head.control & 0x20) != 0)
					g_recv_task_flag = TRUE;
					break;
				}
				case CMD_DELETE_TASK : 
				{
					g_blink = FALSE; 
					break;
				}
				case CMD_SYN_LOCK:			//ͬ��
					g_key_sync = TRUE;
					break;
				default:
					break;
			}
		}
		g_query_key_timeout = QUERY_KEY_TIMEOUT;
	}
}

//����������ģʽ�������ã�
void on_set_mode()
{
	g_temp_uint = get_package_direction(&g_uart_package1);
	if (g_temp_uint == DIR_LOCAL && g_uart_package1.data_len == 1) {
		g_mode = g_uart_package1.data[0];
		reg_handler();
	}
}

void on_opera_actuator()
{
	g_temp_uint = get_package_direction(&g_uart_package1);
	if (g_temp_uint == DIR_LOCAL) {
        if (g_uart_package1.data[0] == 0x55) {
			actuator_unlock();
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
        } else if (g_uart_package1.data[0] == 0xAA) {
			actuator_lock();
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
        } else {
			send_nack_from_package(PC_UART, g_uart_package1, g_buf);
        }
	} else {
		on_other_command();
	}
}

//�ն���
void do_nothing()
{

}
