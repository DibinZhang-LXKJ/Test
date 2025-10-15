//#include "stm8l10x.h"
#include "gd32l23x.h"
#include "Queque.h"
#include "bsp_cpu.h"



/********************************************************************************************************
*  @函数名   S_QueueEmpty                                                           
*  @描述     清空一个队列                                     
*  @参数     Head-队列头地址,  Tail-队列尾地址,   HBuff-队列缓存
*  @返回值   无   
*  @注意    无
********************************************************************************************************/
void S_QueueEmpty(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff)
{
    *Head = HBuff;
    *Tail = HBuff;
}

/********************************************************************************************************
*  @函数名   S_QueueDataIn                                                           
*  @描述     输入一个字节数据进队列                                     
*  @参数     Head-队列头地址,  Tail-队列尾地址,   HBuff-队列缓存
*  @返回值   无   
*  @注意     无
********************************************************************************************************/
void S_QueueDataIn(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *HData, unsigned short DataLen)
{
    unsigned short num;
    unsigned char IptStatus;

    //关闭中断，防止中断对数据入队列造成影响
//    IptStatus = __get_interrupt_state();                    
//    disableInterrupts();
    enter_critical_section();
    
    for (num = 0; num < DataLen; num++, HData++)
    {
        **Tail = *HData;
        (*Tail)++;
        //若入队列时，超出队列缓存区的大小，则从缓存区的开头开始入列
        if (*Tail == HBuff+Len)         
        {
            *Tail = HBuff;
        }
        //若入队列时，队尾和队头相等，则将队头往后移动一位（这会导致丢失一位数据）
        if (*Tail == *Head)
        {
            //若队头后移一位时，刚好超过队列缓存区，则队头将重新指向缓存区的开头
            if (++(*Head) == HBuff+Len)
            {
                *Head = HBuff;
            }
        }
    }
    //入队列成功后，重新开启中断
    //__set_interrupt_state(IptStatus);
    exit_critical_section();
}

/********************************************************************************************************
*  @函数名   S_QueueDataOut                                                           
*  @描述     从队列里取出一个数据                                     
*  @参数     Head-队列头地址,  Tail-队列尾地址,   HBuff-队列缓存
*  @返回值   取出的数据长度  
*  @注意     无
********************************************************************************************************/
unsigned char S_QueueDataOut(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *Data)
{   
    unsigned char back = 0;
    unsigned char IptStatus;
    
//    IptStatus = __get_interrupt_state();   
//    disableInterrupts();
    enter_critical_section();
    *Data = 0;
    //若队尾等于队头，则表示队列中没有数据；只有队尾不等于队头时，消息才能正常出列
    if (*Tail != *Head)
    {
        *Data = **Head;
        back = 1; 
        //若出列后时，队头刚好超过队列缓存区，则队头将重新指向缓存区的开头
        if (++(*Head) == HBuff+Len)
        {
            *Head = HBuff;
        }
    }
    //__set_interrupt_state(IptStatus);
    exit_critical_section();
    return back;
}

/********************************************************************************************************
*  @函数名   S_QueueDataLen                                                           
*  @描述     判断队列里数据的长度                                     
*  @参数     Head-队列头地址,  Tail-队列尾地址,   HBuff-队列缓存
*  @返回值   队列里有数据个数
*  @注意     无
********************************************************************************************************/
unsigned short S_QueueDataLen(unsigned char **Head, unsigned char **Tail, unsigned short Len)
{
    if (*Tail > *Head)
    {
        return *Tail-*Head;
    }
    if (*Tail < *Head)
    {
        return *Tail+Len-*Head;
    }
    return 0;
}
