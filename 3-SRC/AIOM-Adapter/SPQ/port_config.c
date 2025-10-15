#include "port_config.h"
#include "bsp_flash.h"
#include "bsp_cpu.h"
#include "bsp_log.h"

#include "Queque.h"

volatile Queue512 pcTxBuf;
volatile Queue512 keyTxBuf;

/* 串口接口 */
void uart_send(uart_type_enum uart_id, uint8_t* data, uint32_t len)
{
    uint32_t i;

    //发送指定位置的报文，需求更改处
    for (i = 0; i < len; i++)
    {
        bsp_usart_send_byte(uart_id, *(data+i));
        //delay_ms(1);
    }
    #ifdef DEBUG_TEST
        print_log("\nsend to uart%d: ", uart_id);
        for (i = 0; i < len; i++)
        {
            print_log("%02X ", *(data+i));
            
        }
        print_log("\n");
    #endif
    SetWatchDog;
}

void send_buff_clear(void)
{
    QueueEmpty(pcTxBuf);
    QueueEmpty(keyTxBuf);
}

void send_buff_start(void)
{
    uint8_t data;
    
    if ((QueueDataLen(pcTxBuf) > 0) || (QueueDataLen(keyTxBuf) > 0))
    {
        SetWatchDog;
        while (QueueDataLen(pcTxBuf) > 0)
        {
            //处理PC端发送数据
            QueueDataOut(pcTxBuf, &data);
            bsp_usart_send_byte(PC_UART, data);
            SetWatchDog;
        }
        while (QueueDataLen(keyTxBuf) > 0)
        {
            //处理钥匙端数据
            QueueDataOut(keyTxBuf, &data);
            bsp_usart_send_byte(KEY_UART, data);
            SetWatchDog;
        }
    }
}

void send_to_pc(uint8_t* data, uint32_t len)
{
    QueueDataIn(pcTxBuf, data, len);
    
    
uint32_t i;

//    //发送往PC的报文，需求更改处
//    for (i = 0; i < len; i++)
//    {
//        bsp_usart_send_byte(PC_UART, *(data+i));
//        delay_ms(1);
//    }
    #ifdef DEBUG_TEST
        //uint32_t i;
        print_log("\nsend to pc: ");
        for (i = 0; i < len; i++)
        {
            print_log("%02X ", *(data+i));
        }
        print_log("\n");
    #endif
//    SetWatchDog;
}

void send_to_key(uint8_t* data, uint32_t len)
{
    QueueDataIn(keyTxBuf, data, len);
    uint32_t i;
//    delay_ms(3);
//    //发送往KEY的报文，需求更改处
//    for (i = 0; i < len; i++)
//    {
//        bsp_usart_send_byte(KEY_UART, *(data+i));
//    }
    #ifdef DEBUG_TEST
        //uint32_t i;
        print_log("\nsend to key: ");
        for (i = 0; i < len; i++)
        {
            print_log("%02X ", *(data+i));
        }
        print_log("\n");
    #endif
//    SetWatchDog;
}



//注意连续写时的字节对齐问题 和 不满足4字节时的处理
void fmc_contiune_write(uint32_t address, uint8_t *data, uint32_t len)
{
    uint32_t wrtie_len;
    uint32_t u32_Temp;

    if (len % 4 != 0)
    {
        wrtie_len = len + 4 - (len % 4);
    } 

    enter_critical_section();
    for (uint32_t i = 0; i < wrtie_len; i += 4)
    {
        u32_Temp = data[i + 3] << 24 
                 | data[i + 2] << 16 
                 | data[i + 1] << 8 
                 | data[i];
        fmc_write_word(address + i, u32_Temp);
    }
    exit_critical_section();
}

void watch_delay(void)
{
    uint32_t t;
    for (t = 0; t < 50; t++)
    {
        __NOP();
    }
    bsp_port_toggle(WATCHDOG);
}
