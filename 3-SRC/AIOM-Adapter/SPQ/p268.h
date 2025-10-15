#ifndef _MY_P268_H_
#define _MY_P268_H_

#include "gd32l23x.h"
#include "smart_adapter.h"
//#include "debug.h"
#include "bsp_port.h"
#include "bsp_uart.h"
#include "port_config.h"


typedef struct 
{
  //uchar header[2];
  uchar control;
  uchar command;
  uint src_address;
  uint dest_address;
//  uchar sum;  
}PackageHead;

//����ͨ�ű�������
typedef struct
{
  PackageHead head;
  uchar* data;
  uchar data_len;
  //uchar eof;
}Package;

//����ʱ�������
typedef struct
{
  uint  hour;
  uint  minute;
  uint  second;
  uint  million_second;
}TimeSpan;

//�������Ĵ�����ָ��
typedef void (*cmd_handler)(void);

//���Ľ���״̬
#define         NoHeader        0x0
#define         GotFirstHeader  0x1
#define         GotSecondHeader 0x2
#define         GotData         0x03
#define         RecvData        0x4
#define         GotPackage      0x05
#define 		PACKAGE_EOF_FLAG  0x96

//������ģʽ����������Ӧ��ת��Կ�ף�
#define 	MODE_NORMAL	0
#define		MODE_ECHO	1
#define		MODE_TRANS_KEY	2

///////////////////////////////////////////////////////////
//--------------------��Լ����-----------------------
//��λ������ͨ�ź�,��Կ����ѯ����ʱ,��λ:��
#define QUERY_KEY_TIMEOUT       30  
//��ѯ���Կ�׵�ʱ��������λ:����;
#define QUERY_CHARGING_TIMEOUT    60

//ͨ�Ź�Լ������
#define CMD_ACK             	0x00//ACK
#define CMD_SET_KEY_ADDRESS     0x07//����Կ�׵�ַ
#define CMD_SET_KEY_ID  		0x03//����Կ��ID
#define CMD_QUERY_DEVICE_INFO 	0x01//��ѯ�豸��Ϣ�����͡��汾��
//#define CMD_QUERY_DEVICE_ID 	0x02//��ѯ�豸ID

#define CMD_SEND_DATA_COMPLETED 0x18//���ݷ��ͽ���
#define CMD_SET_ADAPTER_ADDRESS 0xFB//���ó������ַ
#define CMD_RECV_INT_TABLE      0x19//�����ж�������
#define CMD_UPDATE_PROGRAM      0x1B//����������
#define CMD_UPDATE_COMMAND      0x1C//��������

#define CMD_QUERY_KEY_STATE     0xFC//��ѯԿ��״̬
#define CMD_RESET_ADAPTER       0xFA//��λ�������ַ��ID
#define CMD_RW_ADAPTER_ID      	0xF8//��ȡ\���ó����ID
#define CMD_SEND_TASK           0x0E//��������Կ��
#define CMD_DELETE_TASK         0x12//ɾ������
#define CMD_SYN_LOCK    		0x0A//��Կ��ͬ������
#define	CMD_SET_MODE			0x35//����������ģʽ
#define	CMD_OPERA_ACTUATOR		0xF7//������������������
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////��־

#define KEY_CHARGE_FULL 0xFFFF//Կ�׳������־
#define KEY_CHARGING	0x0000//Կ�����ڳ���־

#define KEY_ON  0x55	//Կ����λ״̬��־
#define KEY_OFF 0x00	//Կ�ײ���λ״̬��־
#define KEY_FULL 0x0C	//Կ���ѳ䱥״̬��־

//////���Ľ�����//////
#define DIR_NONE 0		//�޽�����
#define DIR_LOCAL 1		//������Ϊ�Լ�
#define DIR_KEY 2		//������ΪԿ��
 
#define CHARGING_TIMEIN  0x0A//δ��ʱ
#define CHARGING_TIMEOUT 0x0B//�ѳ�ʱ

//���߸��±�־
#define UPDATE_OFF 0		//�޸���
#define UPDATE_DATA 2		//���ڽ�������
#define UPDATE_INT_VECTOR 3	//���ڽ����ж�����
#define UPDATE_END 4		//���ո����������

//��ѯԿ�׺���״̬��־
#define KEY_FLAG_TAKEN 0	//Կ�ײ���λ
#define KEY_FLAG_CHARGING 1	//Կ�����ڳ��
#define KEY_FLAG_ON_FULL 2	//Կ�׸ճ�����
#define KEY_FLAG_FULL 3		//Կ���ѳ�����

//��ֹ���ʱ��,��λ:��
#define DISABLE_CHARGING_TIMEOUT 3 
//��ѯԿ��״̬ʱ��ͨ��ʱ����
#define ENABLE_CHARGING_TIMEOUT 6
//���ͨ��˿ڸߵ�ƽ������
#define CKECK_P35_HIGH_COUNT 2

//���ݰ�����
#define GET_PACKAGE_INDEX(Package) (Package.head.control & 0x1F)			//��ȡָ�����ĵ�����
#define IS_FIRST_PACKAGE(Package) ((Package.head.control & 0x60) == 0x40)	//��ѯָ�������Ƿ�����ʼ���ģ������
#define IS_MID_PACKAGE(Package) ((Package.head.control & 0x60) == 0x00)		//��ѯָ�������Ƿ����м䱨�ģ������
#define IS_END_PACKAGE(Package) ((Package.head.control & 0x60) == 0x20)		//��ѯָ�������Ƿ��ǽ������ģ������
#define IS_SINGLE_PACKAGE(Package) ((Package.head.control & 0x60) == 0x60)	//��ѯָ�������Ƿ��ǵ������ģ�������

#define	SET_SINGLE_PACKAGE(Package) (Package.head.control |= 0x60);			//����Ϊ�������ĸ�ʽ
#define SET_FIRST_PACKAGE(Package)	(Package.head.control = (Package.head.control & 0x1F | 0x40));	//����Ϊ��ʼ���ĸ�ʽ
#define SET_END_PACKAGE(Package)	(Package.head.control = (Package.head.control & 0x1F | 0x20));	//����Ϊ�������ĸ�ʽ
#define SET_PACKAGE_INDEX(Package,Index)	(Package.head.control |= (Index % 32));					//����ָ�����ĵ�����

//�������ڱ��Ľ������õ��ı���
#define DECLARE_UART_DATA(UART_NUM) \
	extern 	uchar headerFlag##UART_NUM; \
	extern 	uchar RxData##UART_NUM ; \
	extern 	uint sum##UART_NUM ; \
	extern	uint dataIndex##UART_NUM;\
	extern	uint tempUartUint##UART_NUM ;\
	extern	uchar uartData##UART_NUM[] ;\
	extern	int dataLen##UART_NUM ;\
    extern	uint g_p_new_uart##UART_NUM ;\
    extern  uint g_p_old_uart##UART_NUM ;\
	extern	Package  g_uart_package##UART_NUM;

//���崮�ڱ��Ľ������õ��ı���	
#define DEFINE_UART_DATA(UART_NUM) \
	uchar headerFlag##UART_NUM = NoHeader; \
	uchar RxData##UART_NUM = 0; \
	uint sum##UART_NUM = 0; \
	uint dataIndex##UART_NUM;\
	uint tempUartUint##UART_NUM = 0;\
	uchar uartData##UART_NUM[256] = {0};\
	int dataLen##UART_NUM = 0;\
	uint g_p_new_uart##UART_NUM = 0;\
    uint g_p_old_uart##UART_NUM = 0;\
  Package  g_uart_package##UART_NUM;

//���ڱ��Ľ�������
#define PARSE_PACKAGE(UART_NUM)\
\
switch(headerFlag##UART_NUM)\
	{\
		case NoHeader:\
		{\
			dataIndex##UART_NUM = 0;\
			sum##UART_NUM = 0;\
			if(RxData##UART_NUM == 0x55)\
			{	\
				headerFlag##UART_NUM = GotFirstHeader;    \
				uartData##UART_NUM[dataIndex##UART_NUM++] = RxData##UART_NUM;\
            }\
		}\
		break;\
		case GotFirstHeader:\
		{\
			if(RxData##UART_NUM == 0xAA)\
			{\
				headerFlag##UART_NUM = GotSecondHeader;\
				uartData##UART_NUM[dataIndex##UART_NUM++] = RxData##UART_NUM;\
                g_p_new_uart##UART_NUM++;\
                if(g_p_new_uart##UART_NUM == 0) g_p_new_uart##UART_NUM = 1;\
			}\
            else if(RxData##UART_NUM == 0x55)\
			{	\
				headerFlag##UART_NUM = GotFirstHeader;    \
				uartData##UART_NUM[dataIndex##UART_NUM] = RxData##UART_NUM;\
            }\
			else\
				headerFlag##UART_NUM = NoHeader;\
		}\
		break;\
		case GotSecondHeader:\
		{\
			uartData##UART_NUM[dataIndex##UART_NUM++] = RxData##UART_NUM;\
			\
			if(dataIndex##UART_NUM==0x0B)\
			{\
			  if((uchar)sum##UART_NUM != RxData##UART_NUM)\
				headerFlag##UART_NUM = NoHeader;\
			  else\
			  {\
				tempUartUint##UART_NUM = (uartData##UART_NUM[2]<<8) + uartData##UART_NUM[3];\
				if(tempUartUint##UART_NUM > MaxDataLen)\
					headerFlag##UART_NUM = NoHeader;\
				else if(tempUartUint##UART_NUM == 0)\
				{\
					headerFlag##UART_NUM = GotData;\
				}\
				else\
				{\
					headerFlag##UART_NUM = RecvData; \
                                        dataLen##UART_NUM = tempUartUint##UART_NUM;\
					sum##UART_NUM = 0;\
				}\
			  }\
			} \
			else\
			  sum##UART_NUM+=RxData##UART_NUM;\
		}\
		break;\
		case RecvData:\
		{\
			uartData##UART_NUM[dataIndex##UART_NUM++] = RxData##UART_NUM;\
			dataLen##UART_NUM--;\
			if(dataLen##UART_NUM == -2)\
			{\
				if(sum##UART_NUM != (uartData##UART_NUM[dataIndex##UART_NUM-2]<<8) + uartData##UART_NUM[dataIndex##UART_NUM-1])\
					headerFlag##UART_NUM = NoHeader;\
				else\
					headerFlag##UART_NUM = GotData;\
			}\
			else if(dataLen##UART_NUM >= 0)\
			{\
			  sum##UART_NUM += RxData##UART_NUM;\
			}\
		}\
		break;\
		case GotData:\
		{\
			if(RxData##UART_NUM != PACKAGE_EOF_FLAG)\
				headerFlag##UART_NUM = NoHeader;\
			else\
			{\
				uartData##UART_NUM[dataIndex##UART_NUM++] = RxData##UART_NUM;\
				headerFlag##UART_NUM = GotPackage;\
                g_uart_package##UART_NUM.head.control = uartData##UART_NUM[4];\
				g_uart_package##UART_NUM.head.command = uartData##UART_NUM[5];\
				g_uart_package##UART_NUM.head.src_address = UINT16_HL(uartData##UART_NUM[6],uartData##UART_NUM[7]);\
				g_uart_package##UART_NUM.head.dest_address = UINT16_HL(uartData##UART_NUM[8],uartData##UART_NUM[9]);\
				if(uartData##UART_NUM[2] == 0 && uartData##UART_NUM[3] == 0)\
                { \
                    g_uart_package##UART_NUM.data_len = 0;\
                    g_uart_package##UART_NUM.data = NULL;\
                }\
                else\
                {\
                    g_uart_package##UART_NUM.data_len = UINT16_HL(uartData##UART_NUM[2],uartData##UART_NUM[3]);\
					g_uart_package##UART_NUM.data = &uartData##UART_NUM[11];\
                }\
			}\
		}\
		break;\
		default:\
		{\
			headerFlag##UART_NUM = NoHeader;\
			break;\
		}\
	}
	
//����ͨ�Ź�Լʱ�����ڹ涨ʱ�����޷����յ��������ģ���������ν���
#define CHECK_UART_PACKAGE_TIMEOUT(UART_NUM) \
  {\
	 if(headerFlag##UART_NUM != NoHeader)\
	 {\
		 if(g_p_new_uart##UART_NUM != g_p_old_uart##UART_NUM)\
			g_p_old_uart##UART_NUM = g_p_new_uart##UART_NUM;\
		else\
		{\
			headerFlag##UART_NUM = NoHeader;\
			g_p_old_uart##UART_NUM = 0;\
		}\
	 }\
	 else\
	   g_p_old_uart##UART_NUM = 0;\
  }


#define GET_KEY_AT (g_key_flags & 1)//��ȡԿ����λ״̬
#define GET_KEY_TALK (g_key_flags & 2)//��ȡԿ��ͨ��״̬

//����Կ����λ��־���������λ����ֹͣ��˸
#define SET_KEY_AT(State)   (g_key_flags = ((g_key_flags & (~1)) | (State & 1)));if(!State)g_blink=FALSE
//����Կ��ͨ��״̬
#define SET_KEY_TALK(State)   (g_key_flags = ((g_key_flags & (~2)) | ((State & 1)<< 1)))

//���ڻ��־λ����0λΪ����0����1λΪ����1��
#define UART_ACTIVE g_uart_data_flags
#define UART0_ACTIVE    (g_uart_data_flags & 1)		//��ѯ����0�Ƿ�
#define UART1_ACTIVE    (g_uart_data_flags & 2)		//��ѯ����1�Ƿ�

#define SET_UART0_ACTIVE (g_uart_data_flags |= 1)	//���ô���0�
#define SET_UART1_ACTIVE (g_uart_data_flags |= 2)	//���ô���1�

#define SET_UART0_UNACTIVE (g_uart_data_flags &= (~1))	//���ô���0�ǻ
#define SET_UART1_UNACTIVE (g_uart_data_flags &= (~2))	//���ô���1�ǻ

//ȡtype���͵�field��Ա��ƫ����
//#define	FPOS(type,field) (uint)(&(((type*) 0)-> field))
//���޷�������val��䵽ray����index��ʼ��λ��
#define	FLOP_UINT(ray,val,index) ray[index] = ((val & 0xFF00) >> 8); ray[index+1] = ((val) & 0xFF)

//��ȡ����ʱ��μ��Сʱ����
#define GET_HOUR_DIFF(time_span1,time_span2) \
  (time_span1.hour - time_span2.hour)
  
//��ȡ����ʱ��μ�ķ�������
#define GET_MINUTE_DIFF(time_span1,time_span2) \
  ((time_span1.hour - time_span2.hour) * 60 + time_span1.minute - time_span2.minute)

//��ȡ����ʱ��μ��������
#define	GET_SECOND_DIFF(time_span1,time_span2) \
  ((time_span1.hour-time_span2.hour) * 3600 + time_span1.minute * 60 +time_span1.second - time_span2.minute * 60 - time_span2.second)

//��ȡ����ʱ��μ�ĺ�������
#define	GET_M_SECOND_DIFF(time_span1,time_span2,time_span_diff) \
  time_span_diff = time_span1;\
if(time_span_diff.million_second < time_span2.million_second){\
    time_span_diff.million_second += 1000;\
	if(time_span_diff.second == 0){\
	  time_span_diff.second=59;\
	  if(time_span_diff.minute==0){\
		time_span_diff.minute=59;\
		time_span_diff.hour--; }\
	  else{time_span_diff.minute--;}\
	}\
	else{time_span_diff.second--;}}\
    time_span_diff.million_second -= time_span2.million_second;\
    time_span_diff.second = ((time_span_diff.hour - time_span2.hour) * 3600 + time_span_diff.minute * 60 + time_span_diff.second - time_span2.minute * 60 - time_span2.second);\
    time_span_diff.minute = 0;\
     time_span_diff.hour = 0
//time_span_diff.second = ((time_span1.hour * 3600 + time_span1.minute * 60 +time_span1.second) - (time_span2.hour * 3600 + time_span2.minute * 60 + time_span2.second))

//���ָ����ʱ�������
#define CLEAR_TIMESPAN(timespan) \
	timespan.hour = 0; \
	timespan.minute = 0; \
	timespan.second = 0; \
	timespan.million_second = 0;

//��ѯָ����ʱ��������Ƿ�Ϊ��
#define TIMESPAN_EMPTY(timespan) \
    (timespan.hour == 0 && timespan.minute == 0 && timespan.second == 0 && timespan.million_second == 0)

//���ݳ����ʱ
//#define CHARGE_DIANRONG \
//  __NOP();__NOP();__NOP();__NOP();__NOP();
#endif
