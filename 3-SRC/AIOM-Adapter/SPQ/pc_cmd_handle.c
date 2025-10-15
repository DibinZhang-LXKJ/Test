/********************************************************************
 * 
 * pc端对应的串口报文处理函数列表
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

//设置适配器地址
void on_set_adapter_address()
{
	uint32_t u32_Temp;

	if(GET_KEY_AT && g_uart_package1.data_len == 2)//keyAddress通过查询获取
	{
		tempUartUint1 = UINT16_HL(g_uart_package1.data[0], g_uart_package1.data[1]);
		if(tempUartUint1 >= MIN_ADAPTER_ADDRESS && tempUartUint1 <= MAX_ADAPTER_ADDRESS)
		{
			//备份设备ID
			for (g_temp_uint = 0; g_temp_uint < 16; g_temp_uint++)
			{
				uartData1[g_temp_uint] = READ_DRESS_BYTE(LOCAL_ID_OF_FLASH_SAVE + g_temp_uint);
			}
			//擦除地址
			fmc_erase_pages(LOCAL_ADDR_OF_FLASH_SAVE);
			//写地址
			fmc_write_word(LOCAL_ADDR_OF_FLASH_SAVE, (uint32_t)tempUartUint1);
			//写ID
			fmc_contiune_write(LOCAL_ID_OF_FLASH_SAVE, uartData1, 16);
//			for (g_temp_uint = 0; g_temp_uint < 16; g_temp_uint += 4)       //问题：不满足4byte时，写入清况
//			{
//				u32_Temp = uartData1[g_temp_uint + 3] << 24 
//						| uartData1[g_temp_uint + 2] << 16 
//						| uartData1[g_temp_uint + 1] << 8 
//						| uartData1[g_temp_uint];
//				fmc_write_word(LOCAL_ID_OF_FLASH_SAVE + g_temp_uint, u32_Temp);
//			}
			//回应ACK
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
	}
}

//获取\设置适配器ID
void on_kiss_adapter_id()
{
	uint32_t u32_Temp;

	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//如果为16则为设置判断数据长度，如果为0则为获取命令命令
		if(g_uart_package1.data_len == 0)//读取适配器ID并返回 
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
		else if(g_uart_package1.data_len == 16)//写如适配器ID
		{
			//备份设备地址
			tempUartUint1 = LOCAL_ADDRESS;

			fmc_erase_pages(LOCAL_ADDR_OF_FLASH_SAVE);
			//写设备地址
			fmc_write_word(LOCAL_ADDR_OF_FLASH_SAVE, (uint32_t)tempUartUint1);
			//写设备ID
			fmc_contiune_write(LOCAL_ID_OF_FLASH_SAVE, g_uart_package1.data, 16);
	
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
		else
			send_nack_from_package(PC_UART, g_uart_package1, g_buf);
	}
}

//复位适配器地址和ID
void on_reset_adapter_id()
{
	if(GET_KEY_AT)//keyAddress通过查询获取
	{
		//擦除
		fmc_erase_pages(LOCAL_ADDR_OF_FLASH_SAVE);
		send_ack_from_package(PC_UART, g_uart_package1, g_buf);
	}
}


//在线更新命令
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
			//更新开始
			if(g_temp_uchar == 0)
			{
				#ifdef DEBUG_TEST
				print_log("SPQ Update Start\n");
				#endif
				//复位
				g_update_step = UPDATE_DATA;
				g_update_index = 4;				//前4位用于存放数据包长度
				g_current_package_index = 0;
				send_ack_from_package(PC_UART, g_uart_package1, g_buf);
			}
			else
			{
				#ifdef DEBUG_TEST
				print_log("SPQ Update end\n");
                #endif
				//更新结束
				if(g_update_step == UPDATE_END)//更新数据接收完成
				{
					send_ack_from_package(PC_UART, g_uart_package1, g_buf);
					g_update_step = UPDATE_OFF;
					g_update_index = 0xFFFF;
					delay_ms(1); 
                    mcu_software_reset();   //复位
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
/************************  注意32位内存对齐  ****************************/
//接收更新主程序
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
		//获取当前包号
		g_temp_uchar = GET_PACKAGE_INDEX(g_uart_package1);

		//验证数据，g_update_step为当前步骤，g_current_package_index为待接收包索引
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

				//计算需要擦除多少页，并擦除Flash区域
				erase_page_count = (g_uart_package1.data_len + FMC_PAGE_SIZE - 1) / FMC_PAGE_SIZE;
				for (i = 0; i < erase_page_count; i++)
				{
					fmc_erase_pages(UPDATE_CODE_START + i * FMC_PAGE_SIZE);
				}

				//保存数据前关中断
				enter_critical_section();			//进入临界区

				//处理数据包，最长时245个字节，需要考虑数据包长度不足4字节的填充情况导致的升级包错误
				for (i = 0; i < g_uart_package1.data_len;)			
				{
					SetWatchDog;
					u32_Temp = 0;		//每次处理数据前清零
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
					else			//剩余数据不足4字节
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
			
			//接收完最后一包后，将数据长度写入数据段头部，并改变当前步骤为待接收中断向量表
			if(IS_END_PACKAGE(g_uart_package1))
			{
				//write_flash(UPDATE_CODE_START, g_update_index>>8);
				//write_flash((UPDATE_CODE_START+1), g_update_index);
				fmc_write_word(UPDATE_CODE_START, (uint32_t)g_update_index);
				g_update_step = UPDATE_INT_VECTOR;
			}
			exit_critical_section();			//退出临界区
			send_ack_from_package(PC_UART, g_uart_package1, g_buf);
		}
	}
}

//接收中断向量表
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
		 
			//接收完成后保存至向量表数据段，并设置当前步骤为接收结束
			enter_critical_section();			//进入临界区

			write_update_flag();		//写入待更新标志位		//更新标志位
			exit_critical_section();			//退出临界区

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

//设置钥匙地址
void on_set_key_address()
{
	//如果钥匙在位且地址一致，则转发到钥匙
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

//设置钥匙ID
void on_set_key_id()
{
	//如果钥匙在位且地址一致，则转发到钥匙
	if(GET_KEY_AT && g_uart_package1.data_len == 16)
	{
		g_temp_uint = get_package_data(&g_uart_package1, g_buf);
        memcpy(g_key_id, &g_uart_package1.data[0], 16);
		send_to_key(g_buf, g_temp_uint); 
		
	}
}

//查询钥匙状态
void on_get_key_state()
{
	//检测报文是否是发送给自己
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//设置报文数据，然后转发到PC
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

//同步结束命令
void on_sync_end()
{	
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_KEY)
	{ 
		//如果接收地址是本座的钥匙，则置标志位，并回应ACK
		g_key_sync = FALSE;
		g_query_key_timeout = 0;
		send_ack_from_package(PC_UART, g_uart_package1, g_buf);
	}
}

//查询设备信息
void on_get_device_info()
{
	g_temp_uint = get_package_direction(&g_uart_package1);
	if(g_temp_uint == DIR_LOCAL)
	{
		//设置报文数据，然后转发到PC
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

//其它命令
void on_other_command()
{
	tempUartUint1 = g_uart_package1.head.dest_address;
	if(tempUartUint1 == g_key_address || tempUartUint1 == LOCAL_ADDRESS)
	{
		//当数据包目的地址为充电座地址(查询使用)或钥匙地址(直接转发)时发送到钥匙
		//当63口为高时，延时一段时间后再发（使用Timer_B）
		if(CHECK_CHARGER)
		{
			EnableChongDian;
			disable_uart_rx_interrupt(KEY_USART);		//关闭串口0中断，即key_uart接收中断
			send_delay_from_uart1 = 50;
		}
		else if(send_delay_from_uart1 == 0xFFFF)//如果当前无延时转发任务，则立刻转发到钥匙
		{
			g_temp_uint = get_package_data(&g_uart_package1, g_buf);
			//IE1 &= ~URXIE0;
			disable_uart_rx_interrupt(KEY_USART);		//关闭串口0中断，即key_uart接收中断
			send_to_key(g_buf, g_temp_uint); 
			enable_uart_rx_interrupt(KEY_USART);		    //开启串口0中断，即key_uart接收中断

			
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
				case CMD_SYN_LOCK:			//同步
					g_key_sync = TRUE;
					break;
				default:
					break;
			}
		}
		g_query_key_timeout = QUERY_KEY_TIMEOUT;
	}
}

//设置适配器模式（调试用）
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

//空动作
void do_nothing()
{

}
