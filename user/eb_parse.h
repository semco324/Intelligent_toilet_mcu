#ifndef __EB_PARSE_H
#define __EB_PARSE_H
#include "lib_parse.h"

#include "driver_csm92p10.h"

#define	  DEF_EB_HAEDER   0xCC //eb ͸����header
	  
#define	  DEF_EB_MAX_LEN  20-3   //eb ͸��ģ��len�ֶ����ֵ
		
#define   EB_USED_UART    USART2 //ble ͸��ģ������ô���
		
#define   EBRXSIZE         128   //EB_USED_UART fifo ����EBRXSIZE-1��Ԫ��


//CMD
#define   CMD_EB_PASSTHROUGH  0x59  //��ذ�͸������

extern uart_data_t ebrx_fifo; //be fifo manage struct      
extern queue_type ebrx_arr[EBRXSIZE]; //be fifo arr

void eb_dealwith_queue(void);

#endif


