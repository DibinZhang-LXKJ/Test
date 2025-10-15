/*-----------------------------------------------------------
 * Includes files
 *----------------------------------------------------------*/

/* segger rtt includes. */
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#define BUFFER_INDEX 0

/*!
* @brief ��װ RTT ��ӡ�ӿ�
*        ִ����������
*
* @retval: ��
*/
void print_log(const char * sFormat, ...)
{	
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_vprintf(BUFFER_INDEX, sFormat, &ParamList);
	va_end(ParamList);
}

/*!
* @brief ��ʼ�� RTT ��ӡ�ӿ�
*        ִ����������
*
* @retval: ��
*/
void segger_rtt_init(char * str)
{
	SEGGER_RTT_Init();
	print_log(str);  // ��װ�ӿڣ�������д BUFFER_INDEX
}
