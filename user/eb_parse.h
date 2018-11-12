#ifndef __EB_PARSE_H
#define __EB_PARSE_H
#include "lib_parse.h"

#include "driver_csm92p10.h"

#define	  DEF_EB_HAEDER   0xCC //eb 透传的header
	  
#define	  DEF_EB_MAX_LEN  20-3   //eb 透传模块len字段最大值
		
#define   EB_USED_UART    USART2 //ble 透传模块的所用串口
		
#define   EBRXSIZE         128   //EB_USED_UART fifo 最多放EBRXSIZE-1个元素


//CMD
#define   CMD_EB_PASSTHROUGH  0x59  //电控板透传命令

extern uart_data_t ebrx_fifo; //be fifo manage struct      
extern queue_type ebrx_arr[EBRXSIZE]; //be fifo arr

void eb_dealwith_queue(void);

#endif


