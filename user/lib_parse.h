#ifndef __LIB_PARSE_H
#define __LIB_PARSE_H

#include "lib_queue.h"


typedef struct{
	queue_t		 fifo;
	queue_type cmd_len; //串口一帧的len字段大小
}uart_data_t;


void lib_parse_find_head( uart_data_t * uart_data ,uint8_t header);

void lib_parse_find_len(uart_data_t * uart_data,uint8_t header,uint8_t max_len);

uint8_t lib_parse_check_xor( uint8_t check_front, uint8_t check_uartlen,queue_t *fifo);

#endif
