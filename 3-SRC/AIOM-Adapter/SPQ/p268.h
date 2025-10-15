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

//声明通信报文类型
typedef struct
{
  PackageHead head;
  uchar* data;
  uchar data_len;
  //uchar eof;
}Package;

//声明时间段类型
typedef struct
{
  uint  hour;
  uint  minute;
  uint  second;
  uint  million_second;
}TimeSpan;

//声明报文处理函数指针
typedef void (*cmd_handler)(void);

//报文解析状态
#define         NoHeader        0x0
#define         GotFirstHeader  0x1
#define         GotSecondHeader 0x2
#define         GotData         0x03
#define         RecvData        0x4
#define         GotPackage      0x05
#define 		PACKAGE_EOF_FLAG  0x96

//适配器模式（正常、回应、转发钥匙）
#define 	MODE_NORMAL	0
#define		MODE_ECHO	1
#define		MODE_TRANS_KEY	2

///////////////////////////////////////////////////////////
//--------------------规约定义-----------------------
//上位机发送通信后,向钥匙轮询的延时,单位:秒
#define QUERY_KEY_TIMEOUT       30  
//查询充电钥匙的时间间隔。单位:分钟;
#define QUERY_CHARGING_TIMEOUT    60

//通信规约命令码
#define CMD_ACK             	0x00//ACK
#define CMD_SET_KEY_ADDRESS     0x07//设置钥匙地址
#define CMD_SET_KEY_ID  		0x03//设置钥匙ID
#define CMD_QUERY_DEVICE_INFO 	0x01//查询设备信息（类型、版本）
//#define CMD_QUERY_DEVICE_ID 	0x02//查询设备ID

#define CMD_SEND_DATA_COMPLETED 0x18//数据发送结束
#define CMD_SET_ADAPTER_ADDRESS 0xFB//设置充电座地址
#define CMD_RECV_INT_TABLE      0x19//接收中断向量表
#define CMD_UPDATE_PROGRAM      0x1B//接收主程序
#define CMD_UPDATE_COMMAND      0x1C//更新命令

#define CMD_QUERY_KEY_STATE     0xFC//查询钥匙状态
#define CMD_RESET_ADAPTER       0xFA//复位充电座地址和ID
#define CMD_RW_ADAPTER_ID      	0xF8//获取\设置充电座ID
#define CMD_SEND_TASK           0x0E//发送任务到钥匙
#define CMD_DELETE_TASK         0x12//删除任务
#define CMD_SYN_LOCK    		0x0A//向钥匙同步锁具
#define	CMD_SET_MODE			0x35//设置适配器模式
#define	CMD_OPERA_ACTUATOR		0xF7//操作适配器闭锁机构
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////标志

#define KEY_CHARGE_FULL 0xFFFF//钥匙充满电标志
#define KEY_CHARGING	0x0000//钥匙正在充电标志

#define KEY_ON  0x55	//钥匙在位状态标志
#define KEY_OFF 0x00	//钥匙不在位状态标志
#define KEY_FULL 0x0C	//钥匙已充饱状态标志

//////报文接收者//////
#define DIR_NONE 0		//无接收者
#define DIR_LOCAL 1		//接收者为自己
#define DIR_KEY 2		//接收者为钥匙
 
#define CHARGING_TIMEIN  0x0A//未超时
#define CHARGING_TIMEOUT 0x0B//已超时

//在线更新标志
#define UPDATE_OFF 0		//无更新
#define UPDATE_DATA 2		//正在接收数据
#define UPDATE_INT_VECTOR 3	//正在接收中断向量
#define UPDATE_END 4		//接收更新数据完毕

//查询钥匙函数状态标志
#define KEY_FLAG_TAKEN 0	//钥匙不在位
#define KEY_FLAG_CHARGING 1	//钥匙正在充电
#define KEY_FLAG_ON_FULL 2	//钥匙刚充满电
#define KEY_FLAG_FULL 3		//钥匙已充满电

//禁止充电时间,单位:秒
#define DISABLE_CHARGING_TIMEOUT 3 
//查询钥匙状态时的通电时间间隔
#define ENABLE_CHARGING_TIMEOUT 6
//检测通电端口高电平最大次数
#define CKECK_P35_HIGH_COUNT 2

//数据包属性
#define GET_PACKAGE_INDEX(Package) (Package.head.control & 0x1F)			//获取指定报文的索引
#define IS_FIRST_PACKAGE(Package) ((Package.head.control & 0x60) == 0x40)	//查询指定报文是否是起始报文（多包）
#define IS_MID_PACKAGE(Package) ((Package.head.control & 0x60) == 0x00)		//查询指定报文是否是中间报文（多包）
#define IS_END_PACKAGE(Package) ((Package.head.control & 0x60) == 0x20)		//查询指定报文是否是结束报文（多包）
#define IS_SINGLE_PACKAGE(Package) ((Package.head.control & 0x60) == 0x60)	//查询指定报文是否是单个报文（单包）

#define	SET_SINGLE_PACKAGE(Package) (Package.head.control |= 0x60);			//设置为单个报文格式
#define SET_FIRST_PACKAGE(Package)	(Package.head.control = (Package.head.control & 0x1F | 0x40));	//设置为起始报文格式
#define SET_END_PACKAGE(Package)	(Package.head.control = (Package.head.control & 0x1F | 0x20));	//设置为结束报文格式
#define SET_PACKAGE_INDEX(Package,Index)	(Package.head.control |= (Index % 32));					//设置指定报文的索引

//声明串口报文解析所用到的变量
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

//定义串口报文解析所用到的变量	
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

//串口报文解析代码
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
	
//串口通信规约时，若在规定时间内无法接收到完整报文，则放弃本次接收
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


#define GET_KEY_AT (g_key_flags & 1)//获取钥匙在位状态
#define GET_KEY_TALK (g_key_flags & 2)//获取钥匙通信状态

//设置钥匙在位标志，如果不在位，则停止闪烁
#define SET_KEY_AT(State)   (g_key_flags = ((g_key_flags & (~1)) | (State & 1)));if(!State)g_blink=FALSE
//设置钥匙通信状态
#define SET_KEY_TALK(State)   (g_key_flags = ((g_key_flags & (~2)) | ((State & 1)<< 1)))

//串口活动标志位，第0位为串口0，第1位为串口1。
#define UART_ACTIVE g_uart_data_flags
#define UART0_ACTIVE    (g_uart_data_flags & 1)		//查询串口0是否活动
#define UART1_ACTIVE    (g_uart_data_flags & 2)		//查询串口1是否活动

#define SET_UART0_ACTIVE (g_uart_data_flags |= 1)	//设置串口0活动
#define SET_UART1_ACTIVE (g_uart_data_flags |= 2)	//设置串口1活动

#define SET_UART0_UNACTIVE (g_uart_data_flags &= (~1))	//设置串口0非活动
#define SET_UART1_UNACTIVE (g_uart_data_flags &= (~2))	//设置串口1非活动

//取type类型的field成员的偏移量
//#define	FPOS(type,field) (uint)(&(((type*) 0)-> field))
//将无符号整数val填充到ray数组index开始的位置
#define	FLOP_UINT(ray,val,index) ray[index] = ((val & 0xFF00) >> 8); ray[index+1] = ((val) & 0xFF)

//获取两个时间段间的小时数差
#define GET_HOUR_DIFF(time_span1,time_span2) \
  (time_span1.hour - time_span2.hour)
  
//获取两个时间段间的分钟数差
#define GET_MINUTE_DIFF(time_span1,time_span2) \
  ((time_span1.hour - time_span2.hour) * 60 + time_span1.minute - time_span2.minute)

//获取两个时间段间的秒数差
#define	GET_SECOND_DIFF(time_span1,time_span2) \
  ((time_span1.hour-time_span2.hour) * 3600 + time_span1.minute * 60 +time_span1.second - time_span2.minute * 60 - time_span2.second)

//获取两个时间段间的毫秒数差
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

//清空指定的时间段数据
#define CLEAR_TIMESPAN(timespan) \
	timespan.hour = 0; \
	timespan.minute = 0; \
	timespan.second = 0; \
	timespan.million_second = 0;

//查询指定的时间段数据是否为空
#define TIMESPAN_EMPTY(timespan) \
    (timespan.hour == 0 && timespan.minute == 0 && timespan.second == 0 && timespan.million_second == 0)

//电容充电延时
//#define CHARGE_DIANRONG \
//  __NOP();__NOP();__NOP();__NOP();__NOP();
#endif
