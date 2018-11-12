#ifndef __LIB_QUEUE_H
#define __LIB_QUEUE_H

#include "stm32f0xx.h"

typedef uint8_t    queue_type;
typedef uint8_t size_type;

#ifndef NULL
#define NULL ((void *)0)
#endif

#define FULL_JUDGE
#define LIB_QUEUE_DEBUG

typedef struct{
	queue_type *   p_arr;
	size_type      front;//head
	size_type      rear; //tail
	size_type      size; 
	size_type      valid_len;//队列有效元素个数
}queue_t;


uint8_t lib_init_queue(queue_t * p_queue,queue_type * buf, size_type size);

void reset_queue(queue_t * p_queue);

/* ret  1 empty     0 not empty*/
uint8_t lib_queue_is_empty(queue_t * p_queue);

/*ret  1 full     0 not full*/
uint8_t lib_queue_is_full(queue_t * p_queue);

size_type lib_get_queue_len(queue_t * p_queue);

/*ret  1 sucess     0 fail*/
uint8_t lib_en_queue(queue_t * p_queue,queue_type m);

/*ret  1 sucess     0 fail(empty)*/
uint8_t lib_de_queue(queue_t * p_queue,queue_type * m);

/*ret  queue avg*/
queue_type lib_avg_queue(queue_t * p_queue);

#endif
