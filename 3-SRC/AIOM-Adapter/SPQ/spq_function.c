#include "spq_function.h"
#include <string.h>
#include "iap_config.h"
#include "var.h"
#include "Queque.h"
#include "delay.h"
#include "cmd_handler.h"
#include "bsp_log.h"
#include "bsp_cpu.h"
#include "actuator.h"
#include "key_test.h"

static uint get_sum(uchar* data, uint startIndex, uint len);
static void delay_2_5us_optimized(void);
static uint get_current_key_state();
static void query_key_charging_state(void);
static uint get_current_charging_state(void);
extern volatile Queue512 keyBuf;

//����ACK������ָ�����ڣ��ӱ��Ļ�ȡԴ��ַ��Ŀ�ĵ�ַ��
void send_ack_from_package(uart_type_enum uart_id, Package pSource, uint8_t *buf)
{
    g_package.head.command = 0x00;
    g_package.head.control = 0x60;
    g_package.head.src_address = pSource.head.dest_address;
    g_package.head.dest_address = pSource.head.src_address;
    g_package.data_len = 0;
    g_package.data = NULL;
    g_temp_uint = get_package_data(&g_package, buf);
	uart_send(uart_id, buf, g_temp_uint);
}

//����ACK������ָ������
void send_ack(uart_type_enum uart_id, uint16_t src, uint16_t dest, uint8_t *buf)
{
    g_package.head.command = 0x00;
    g_package.head.control = 0x60;
    g_package.head.src_address = src;
    g_package.head.dest_address = dest;
    g_package.data_len = 0;
    g_package.data = NULL;
    g_temp_uint = get_package_data(&g_package, buf);
	uart_send(uart_id, buf, g_temp_uint);
}

//����ACK������ָ�����ڣ��ӱ��Ļ�ȡԴ��ַ��Ŀ�ĵ�ַ��
void send_nack_from_package(uart_type_enum uart_id, Package pSource, uint8_t *buf)
{
    g_package.head.command = 0xFF;
    g_package.head.control = 0x60;
    g_package.head.src_address = pSource.head.dest_address;
    g_package.head.dest_address = pSource.head.src_address;
    g_package.data_len = 0;
    g_package.data = NULL;
    g_temp_uint = get_package_data(&g_package, buf);
	uart_send(uart_id, buf, g_temp_uint);
}

void send_nack(uart_type_enum uart_id, uint16_t src, uint16_t dest, uint8_t *buf)
{
    g_package.head.command = 0xFF;
    g_package.head.control = 0x60;
    g_package.head.src_address = src;
    g_package.head.dest_address = dest;
    g_package.data_len = 0;
    g_package.data = NULL;
    g_temp_uint = get_package_data(&g_package, buf);
	uart_send(uart_id, buf, g_temp_uint);
}

//��ʼ��ϵͳȫ�ֱ���
void init_args(void)
{
	//��ʼ��ʱ��
	g_now_time.hour = 0;
	g_now_time.minute = 0;
	g_now_time.second = 0;
	g_now_time.million_second = 0;
  
	g_last_time = g_now_time;
}

//ע�ᴮ�ڱ��Ĵ�������PC�ˣ�
void reg_handler(void)
{
	//�������ԭ������
	for(g_temp_uint = 0;g_temp_uint < sizeof(g_pc_handler)/sizeof(g_pc_handler[0]);  g_temp_uint++)
	{
        g_pc_handler[g_temp_uint] = NULL;
    }
	//���ݵ�ǰ������ģʽ���趨��Ӧ�Ĵ�����
	if(g_mode == MODE_NORMAL)
	{
	    g_pc_handler[CMD_SET_ADAPTER_ADDRESS] = on_set_adapter_address;
		g_pc_handler[CMD_RW_ADAPTER_ID] = on_kiss_adapter_id;
		g_pc_handler[CMD_RESET_ADAPTER] = on_reset_adapter_id;
		g_pc_handler[CMD_UPDATE_COMMAND] = on_update_command;
		g_pc_handler[CMD_UPDATE_PROGRAM] = on_update_program;
		g_pc_handler[CMD_RECV_INT_TABLE] = on_update_interrupt_table;
		g_pc_handler[CMD_SET_KEY_ID] = on_set_key_id;
		g_pc_handler[CMD_SET_KEY_ADDRESS] = on_set_key_address;
		g_pc_handler[CMD_QUERY_KEY_STATE] = on_get_key_state;
		g_pc_handler[CMD_SEND_DATA_COMPLETED] = on_sync_end;
		g_pc_handler[CMD_QUERY_DEVICE_INFO] = on_get_device_info;
        g_pc_handler[CMD_OPERA_ACTUATOR] = on_opera_actuator;
        g_pc_handler[CMD_SET_MODE] = on_set_mode;
		g_default_pc_handler = on_other_command;
	}
	else if(g_mode == MODE_ECHO || g_mode == MODE_TRANS_KEY)
	{
		g_pc_handler[CMD_SET_MODE] = on_set_mode;
		g_default_pc_handler = do_nothing;
	}
}

//��䱨��������buf�������ر��ĳ���
int get_package_data(Package* p,uchar* buf)
{
    //ͷ
    buf[0] = 0x55;
    buf[1] = 0xAA;
    //���ݶγ���
    FLOP_UINT(buf, p->data_len, 2);
    //������
    buf[4] = p->head.control;
    //������
    buf[5] = p->head.command;
    //Դ��ַ��Ŀ�ĵ�ַ
    FLOP_UINT(buf, p->head.src_address, 6);
    FLOP_UINT(buf, p->head.dest_address, 8);
    //ͷ��У��
    buf[10] = (uchar)get_sum(buf, 2, 8);
    //���ݶ�
    uchar* data = &buf[11];
    if(p->data_len != 0)
    {
        memcpy(data, p->data, p->data_len);
        uint sum = get_sum(p->data, 0, p->data_len);
        FLOP_UINT(buf, sum, 11+p->data_len);
        buf[13 + p->data_len] = PACKAGE_EOF_FLAG;
        return 14 + p->data_len;
    }
    else
    {
        buf[11+p->data_len] = PACKAGE_EOF_FLAG;
        return 12;
    }
}

//��ȡ���ĵĽ��շ����Լ���Կ�ף�
uint get_package_direction(Package* p)
{
    g_temp_uint = LOCAL_ADDRESS;
    if(g_temp_uint != DEFAULT_ADAPTER_ADDRESS && p->head.dest_address == g_temp_uint)return DIR_LOCAL;
    if(g_key_address != DEFAULT_KEY_ADDRESS && p->head.dest_address == g_key_address)return DIR_KEY;
    return DIR_NONE;
}

//����Կ�׶�����
void handle_key_data(uint8_t data)
{
    SET_UART0_ACTIVE;
    RxData0 = data;
    
    //����������ģʽ������Ӧ����
	if(g_mode == MODE_NORMAL)
	{
		PARSE_PACKAGE(0)

        #ifdef DEBUG_TEST
            print_log("%02X ", data);
        #endif
		if(headerFlag0 == GotPackage)
		{
            #ifdef DEBUG_TEST
                print_log("    Data form key\n");
            #endif
			headerFlag0 = NoHeader;
            if ((g_key_address == 0xFFFF) && (g_uart_package0.head.command == 0xFC))
            {
                g_key_address = g_uart_package0.head.src_address;
            }
			SET_KEY_AT(TRUE);
			
			cmd_handler handler = get_key_handler(g_uart_package0.head.command);
			handler();
		}
	}
	else if(g_mode == MODE_TRANS_KEY)
	{
		g_buf[0] = RxData0;
        
		send_to_pc(g_buf,1); 
	}
	SetWatchDog;
}

void handle_pc_data(uint8_t data)
{
    RxData1 = data;
   
    PARSE_PACKAGE(1)
    #ifdef DEBUG_TEST
        print_log("%02X ", data);
    #endif
    switch(g_mode)
    {
        case MODE_ECHO:
            g_buf[0] = RxData1;
            send_to_pc(g_buf,1); 
            break;
        case MODE_TRANS_KEY:
            g_buf[0] = RxData1;
            send_to_key(g_buf,1); 
            break;
        case MODE_NORMAL:
            if(headerFlag1 == GotPackage)
            {
                #ifdef DEBUG_TEST
                    print_log("    Data form pc\n");
                #endif
                headerFlag1 = NoHeader;
                if(g_pc_handler[g_uart_package1.head.command] != NULL)
                    g_pc_handler[g_uart_package1.head.command]();
                else
                    g_default_pc_handler();
            }//end if
            break;
        default:
            break;
    }
    SetWatchDog;
}

void handle_key_state(void)
{
    static uint8_t first_start = 1;
    //ÿ�̶ȱ�ʾ10ms 
    g_timerA_tick++;
      
    //����ϵͳʱ��
    g_now_time.million_second+=10;

    if (g_now_time.million_second >= 1000) {
        g_now_time.million_second = 0;
        g_now_time.second++;

        // g_query_charge_timeout--; 
        // if(g_query_charge_timeout < 0) g_query_charge_timeout = 0;
        // g_query_key_timeout--;
        // if(g_query_key_timeout < 0) g_query_key_timeout = 0;

        if(g_now_time.second >= 60) {
            g_now_time.second = 0;
            g_now_time.minute++;
            if(g_now_time.minute >= 60) {
                g_now_time.minute = 0;
                g_now_time.hour++; 
            }
        }
    }

    if (g_now_time.million_second == 110) {
        //ÿ����110����ʱ��⴮��0�����Ƿ�ʱ
        CHECK_UART_PACKAGE_TIMEOUT(0);
    } else if (g_now_time.million_second == 120) {
        //ÿ����120����ʱ��⴮��1�����Ƿ�ʱ
        CHECK_UART_PACKAGE_TIMEOUT(1);
    } else if (!g_key_sync 
            && g_now_time.million_second == 130 
            && g_query_key_timeout > 0 //��ͨ��ʱg_query_key_timeout<=0
            && g_query_key_timeout < (QUERY_KEY_TIMEOUT - 2)) {
        //ÿ����130����ʱ�������ͨ�ŵ���ͬ��״̬������Կ���ڲ���λ
        //Ӧ��ʹ��Uart0Rx�������ж�Key�Ƿ���λ������������ͨ���У�����������
        if (headerFlag0 == NoHeader) {
            DisnableChongDian;
            g_temp_uint = 0;
            while(g_temp_uint++ < 6);
            
            g_port1_error = 0;
            EnableChongDian;
            delay_2_5us_optimized();
            g_temp_uint = CHECK_KEY_UART;

            if (g_temp_uint) {
                //g_comm_port_error++;
                g_temp_uint = 0;
                while (CHECK_KEY_UART) {//�ȴ����ݵĳ�����û�磬����״̬��
                    g_temp_uint++;
                    if(g_temp_uint >= 60)
                    {
                        g_comm_port_error++;
                        break;
                    }
                }
                if (g_comm_port_error > 2) {
                    g_comm_port_error = 0;
                    //����λ
                    SET_KEY_AT(FALSE);
                    //��λԿ�׵�ַ
                    g_key_address = 0xFFFF; 
                    QueueEmpty(keyBuf);
                    for(g_temp_uint = 0 ;g_temp_uint < 16;g_temp_uint++)
                        g_key_id[g_temp_uint] = 0xFF;
                    
                    g_query_key_timeout = 0;
                    g_check_key_flag = KEY_FLAG_TAKEN;
                    g_on_new_state = TRUE;
                } else if (g_temp_uint < 60) {
                    g_comm_port_error = 0;
                }
            } else {
                g_comm_port_error = 0;
            }
        }/**/
    }
    //ÿ��500�����ѯһ��Կ��״̬
    if(g_now_time.million_second % 200 == 0) {
        if(g_query_key_timeout <= 0) {
            g_comm_port_error = 0;
            g_query_key_timeout = 0;
            switch (g_check_key_flag) {
                case KEY_FLAG_TAKEN://Կ�ײ���λ
                {
                    if (NO_RIGHT == key_status_poll()) {
                       OPEN_FMQ();
                    }
                    g_temp_uint = get_current_key_state();
                    if(g_temp_uint == KEY_ON) {
                        //��λ
                        g_check_key_flag = KEY_FLAG_CHARGING;
                        actuator_lock();
                        g_on_new_state = TRUE;
                        SET_KEY_AT(TRUE);
                        g_key_charge_state = KEY_CHARGING;
                    }
                    else if(g_temp_uint == KEY_OFF) {
                        if (first_start) {
                            actuator_unlock();
                            first_start = 0;
                        }
                    } else if(g_temp_uint == CHARGING_TIMEIN) {
                        __NOP();
                    } else {
                    }
                }
                break;
                
                case KEY_FLAG_CHARGING:
                {
                    if(g_key_charge_state == KEY_CHARGE_FULL) {
                        g_check_key_flag = KEY_FLAG_ON_FULL;
                        g_on_new_state = TRUE;
                    } else {
                        g_temp_uint = get_current_charging_state();
                        if (g_temp_uint == CHARGING_TIMEOUT)
                        {
                        //QUERY_KEY_STATE;
                        g_on_new_state = TRUE;
                        } else if (g_temp_uint == CHARGING_TIMEIN) {
                        __NOP();
                        } else {
                            g_check_key_flag = KEY_FLAG_TAKEN;
                            g_on_new_state = TRUE;
                        }
                    }
                }
                break;
                
                case KEY_FLAG_ON_FULL:
                {
                    g_temp_uint = get_current_charging_state();
                    if (g_temp_uint == CHARGING_TIMEOUT || g_temp_uint == KEY_FULL) {
                        g_check_key_flag = KEY_FLAG_FULL;
                        g_on_new_state = TRUE;
                    } else if (g_temp_uint == CHARGING_TIMEIN) {
                        if(g_key_charge_state == KEY_CHARGING)
                        {
                            g_check_key_flag = KEY_FLAG_CHARGING;
                            g_on_new_state = TRUE;
                        }
                    } else if (g_temp_uint == KEY_OFF) {//���͸�key�ı���û�л�Ӧʱ
                        g_check_key_flag = KEY_FLAG_TAKEN;
                        g_on_new_state = TRUE; 
                    }
                }
                break;
                
                case KEY_FLAG_FULL: 
                {
                    g_temp_uint = get_current_key_state();
                    
                    if (g_temp_uint == KEY_ON) { //��λ
                        SET_KEY_AT(TRUE);
                        DisnableChongDian;
                        __NOP();
                    } else if (g_temp_uint == KEY_OFF) {//Կ�ײ���λ
                        SET_KEY_AT(FALSE);
                        //��λԿ�׵�ַ
                        g_key_address = 0xFFFF; 
                        for(g_temp_uint = 0 ;g_temp_uint < 16;g_temp_uint++) {
                            g_key_id[g_temp_uint] = 0xFF;
                        }
                            
                        g_check_key_flag = KEY_FLAG_TAKEN;
                        g_on_new_state = TRUE;
                        
                        QueueEmpty(keyBuf);
                    } else if (g_temp_uint == CHARGING_TIMEIN) {
                        __NOP();
                        if (GET_HOUR_DIFF(g_now_time,g_last_checking_time) / 24 >= 7) {
                            g_check_key_flag = KEY_FLAG_CHARGING;
                            g_on_new_state = TRUE;
                        }
                    } else {

                    }
                }
                break;
            
                default:
                    g_check_key_flag = KEY_FLAG_TAKEN;
                    g_on_new_state = TRUE;
                break;
            }
        } else {
        
        }
    }
    g_last_time = g_now_time; 
}


void handle_led_state(void)
{
//    g_temp_uint = TBIV;

    //�̶�10ms
    g_timerB_tick++;     
    
    if(g_timerB_tick % 50 == 0)
    {
        if(g_blink)         //�����ͳɹ����̵���˸
        {
            if(g_blink & 0x20)
            {
                LED1OFF;
                LED2OFF;
                LED3OFF;
            }
            else
            {
                LED1ONGr;
                LED2ONGr;
                LED3ONGr;
            }
            g_blink ^= 0x20;
        }
        else              //ƽʱ��
        {
            LED1ONRed;

            SET_UART0_UNACTIVE;
            if(GET_KEY_TALK > 0)    //Կ��ͨ��ʱ��LED3���̵�
            {
                LED3ONGr;
                SET_KEY_TALK(FALSE);
            }
            else
            {
                LED3OFF;
            }
            
            if(GET_KEY_AT)         //Կ����λʱ��LED2
            {
                if(g_key_charge_state == KEY_CHARGE_FULL)   //������ʱ�����Ƶ�
                { 
                    LED2OFF;
                    LED2ONYe;
                }
                else                        //�����ʱ�����̵�
                {
                    LED2OFF;
                    LED2ONGr;
                }
            }
            else
            { 
                LED2OFF;
            }
        }
    }
    if(send_delay_from_uart1 != 0xFFFF)
    {
        if(--send_delay_from_uart1 == 0)
        {
            //IE1 &= ~URXIE0;	//�رս����ж�
            disable_uart_rx_interrupt(KEY_USART);
            g_temp_uint = get_package_data(&g_uart_package1, g_buf);
            send_to_key(g_buf, g_temp_uint);
            //IE1 |= URXIE0;
            enable_uart_rx_interrupt(KEY_USART);
            send_delay_from_uart1 = 0xFFFF;
        }
    }
}

//
void handle_duanlu(void)
{
    //����·���� P13 ����  �ɵ͵�����Ծ 
    if(g_port1_error++ > 50)
    {
        OPEN_FMQ();

        g_port1_error = 0;
        DisnableChongDian;
        delay_ms(1);
        EnableChongDian;
    } 
//    P1IFG=0x00;	//����жϱ�־   
}

//����У���
static uint get_sum(uchar* data, uint startIndex, uint len)
{
    uint sum = 0;
    for(int i=0;i<len;i++)
        sum += data[i+startIndex];
    return sum;
}

static void delay_2_5us_optimized(void) 
{
    // ��Լ 160 - 10������ѭ���������� 150
    for (volatile int i = 0; i < 30; i++) {
        __NOP();
    }
}
//��ȡ��ǰԿ����λ״̬
static uint get_current_key_state()
{
	if (g_on_new_state) {
		g_on_new_state = FALSE;
		g_checking_key_state = FALSE;
        gc_query_key_count = 0;
		g_last_checking_time = g_now_time;
		if(g_check_key_flag == KEY_FLAG_TAKEN)
        {
            g_disable_charging_time = 2;
            //����λ
            SET_KEY_AT(FALSE);
            //��λԿ�׵�ַ
            g_key_address = 0xFFFF; 
            QueueEmpty(keyBuf);
            for (g_temp_uint = 0 ;g_temp_uint < 16;g_temp_uint++) {
                g_key_id[g_temp_uint] = 0xFF; 
            }
        } else if (g_check_key_flag == KEY_FLAG_FULL) {
            g_disable_charging_time = 7;
            SET_KEY_AT(TRUE);
        }
#ifdef LOGGING
		g_buf[0] = 0x77;
        g_buf[1] = (uchar)g_check_key_flag;
        g_buf[2] = 0x77;
        
		send_to_pc(g_buf,3);
#endif
	} /*ifend*/
    if (gc_query_key_count++ >= g_disable_charging_time) {
        g_port1_error = 0;
        gc_query_key_count = 0;

        EnableChongDian;
        /*g_temp_uint=0;
        while(g_temp_uint<ENABLE_CHARGING_TIMEOUT) g_temp_uint++;
        */
        //CHARGE_DIANRONG
        //CHARGE_DIANRONG
        //CHARGE_DIANRONG
        //CHARGE_DIANRONG
        delay_2_5us_optimized();
        g_temp_uint = CHECK_KEY_UART;

        if(!g_temp_uint) {
            return KEY_ON;				
        } else {             
            DisnableChongDian;
            return KEY_OFF;				
        }
    } else {
        DisnableChongDian;
    }/*ifend*/
    return CHARGING_TIMEIN;
}

static void query_key_charging_state(void)
{
	g_package.head.control = 0x00;
	g_package.head.command = CMD_QUERY_KEY_STATE;
	g_package.head.src_address = LOCAL_ADDRESS;
	g_package.head.dest_address = g_key_address;
	g_package.data_len = 0;
	g_temp_uint = get_package_data(&g_package, g_buf);
	send_to_key(g_buf, g_temp_uint);
}

//��ȡ��ǰԿ�׳��״̬
static uint get_current_charging_state(void)
{
    if(g_on_new_state)
    {
        g_on_new_state = FALSE;
        g_last_checking_time = g_now_time;
        g_checking_key_state = FALSE;
        //g_query_charge_timeout = 0;
        g_p35_high_count=0;
        g_recv_key_state = FALSE;
        gc_check_full_count = 0;
        gc_query_key_count = 0;
        EnableChongDian;
        if(g_check_key_flag == KEY_FLAG_ON_FULL)
        {
            g_key_charge_state = KEY_CHARGE_FULL;
        //g_recv_key_state = TRUE;
        }
        g_disable_charging_time = 2;// DISABLE_CHARGING_TIMEOUT;
#ifdef LOGGING
		g_buf[0] = 0x77;
        g_buf[1] = (uchar)g_check_key_flag;
        g_buf[2] = 0x77;
        
		send_to_pc(g_buf,3);
#endif
	}
	g_port1_error = 0;
	
	//��ȡ��ѹ��200msѭ��һ��
    if(!g_recv_key_state)
    {
        if(gc_query_key_count++ < 3)
        {
            query_key_charging_state();//QUERY_KEY_STATE;          //���Ͳ�ѯ��������
            return CHARGING_TIMEIN;
        }
        else
        {
            return KEY_OFF;
        }
    }
	else
	{    
        if(!CHECK_KEY_UART)
        {
            #ifdef LOGGING
            g_buf[0] = 0xAA;			
            send_to_pc(g_buf,1);
            #endif

            //�˿�Ϊ��ʱ�ȴ���ʱ
            g_p35_high_count=0;

            if(GET_MINUTE_DIFF (g_now_time, g_last_checking_time) >= QUERY_CHARGING_TIMEOUT)
            {
                g_last_checking_time = g_now_time;
                return 	CHARGING_TIMEOUT;
            }
            return CHARGING_TIMEIN;
        }
		else
		{
            #ifdef LOGGING
			g_buf[0] = 0xAB;			
			send_to_pc(g_buf,1);
			#endif
            
            SET_KEY_AT(TRUE);
			g_p35_high_count++;
				
			//����Ϊ�ߵ�ƽ
			if(g_p35_high_count > CKECK_P35_HIGH_COUNT)
			{
				#ifdef LOGGING		
				g_buf[0] = 0xBB;			
				send_to_pc(g_buf,1);
				#endif
                
                g_p35_high_count = 0;
                gc_check_full_count++;
                if(g_check_key_flag == KEY_FLAG_ON_FULL && gc_check_full_count > 10)
                    return KEY_FULL;
                else
                {
                    g_p35_high_count = 0;
                    g_recv_key_state = FALSE;
                    gc_query_key_count = 0;
                }
			}
			return CHARGING_TIMEIN;
		}
	}
}

//����������
void OPEN_FMQ(void)
{
    uint32_t Loopj;
    

    for (Loopj = 0; Loopj < 500; Loopj++)   //500
    {      
        SET_FMQ;
        delay_us(125);               // �ߵ�ƽ��ʱ
        SET_FMQ;
        delay_us(125);               // �͵�ƽ��ʱ
    }
}

