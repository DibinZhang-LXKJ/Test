//#include "stm8l10x.h"
#include "gd32l23x.h"
#include "Queque.h"
#include "bsp_cpu.h"



/********************************************************************************************************
*  @������   S_QueueEmpty                                                           
*  @����     ���һ������                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   ��   
*  @ע��    ��
********************************************************************************************************/
void S_QueueEmpty(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff)
{
    *Head = HBuff;
    *Tail = HBuff;
}

/********************************************************************************************************
*  @������   S_QueueDataIn                                                           
*  @����     ����һ���ֽ����ݽ�����                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   ��   
*  @ע��     ��
********************************************************************************************************/
void S_QueueDataIn(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *HData, unsigned short DataLen)
{
    unsigned short num;
    unsigned char IptStatus;

    //�ر��жϣ���ֹ�ж϶�������������Ӱ��
//    IptStatus = __get_interrupt_state();                    
//    disableInterrupts();
    enter_critical_section();
    
    for (num = 0; num < DataLen; num++, HData++)
    {
        **Tail = *HData;
        (*Tail)++;
        //�������ʱ���������л������Ĵ�С����ӻ������Ŀ�ͷ��ʼ����
        if (*Tail == HBuff+Len)         
        {
            *Tail = HBuff;
        }
        //�������ʱ����β�Ͷ�ͷ��ȣ��򽫶�ͷ�����ƶ�һλ����ᵼ�¶�ʧһλ���ݣ�
        if (*Tail == *Head)
        {
            //����ͷ����һλʱ���պó������л����������ͷ������ָ�򻺴����Ŀ�ͷ
            if (++(*Head) == HBuff+Len)
            {
                *Head = HBuff;
            }
        }
    }
    //����гɹ������¿����ж�
    //__set_interrupt_state(IptStatus);
    exit_critical_section();
}

/********************************************************************************************************
*  @������   S_QueueDataOut                                                           
*  @����     �Ӷ�����ȡ��һ������                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   ȡ�������ݳ���  
*  @ע��     ��
********************************************************************************************************/
unsigned char S_QueueDataOut(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *Data)
{   
    unsigned char back = 0;
    unsigned char IptStatus;
    
//    IptStatus = __get_interrupt_state();   
//    disableInterrupts();
    enter_critical_section();
    *Data = 0;
    //����β���ڶ�ͷ�����ʾ������û�����ݣ�ֻ�ж�β�����ڶ�ͷʱ����Ϣ������������
    if (*Tail != *Head)
    {
        *Data = **Head;
        back = 1; 
        //�����к�ʱ����ͷ�պó������л����������ͷ������ָ�򻺴����Ŀ�ͷ
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
*  @������   S_QueueDataLen                                                           
*  @����     �ж϶��������ݵĳ���                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   �����������ݸ���
*  @ע��     ��
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
