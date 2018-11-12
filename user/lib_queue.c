#include "lib_queue.h"

/*
 *    out------------------in
 *<----a1,a2,a3,...,an     <----
 *      ^                ^
 *   front             rear
 *    head             tail
*/

/*
ret  1 success     0 fail
*/
uint8_t lib_init_queue(queue_t * p_queue,queue_type * buf, size_type size)
{
	if(buf == NULL) 
		return 0;
			
	p_queue->p_arr = buf;
	p_queue->valid_len = p_queue->front = p_queue->rear = 0;
	p_queue->size = size;
	  return 1;
}


void reset_queue(queue_t * p_queue)
{
   p_queue->front = p_queue->rear = 0;
}

/*
ret  1 empty     0 not empty
*/
uint8_t lib_queue_is_empty(queue_t * p_queue)
{
	if( (p_queue->front) == (p_queue->rear) )
	  return 1;
	return 0;
}

/*
ret  1 full     0 not full
*/
uint8_t lib_queue_is_full(queue_t * p_queue)
{
	if( ((p_queue->rear+1)%p_queue->size) == p_queue->front )
	  return 1;
	return 0;
}

/*
ret  queue len
*/
size_type lib_get_queue_len(queue_t * p_queue)
{
	  //(tail - head + size)%size
	  p_queue->valid_len = (p_queue->rear - p_queue->front + p_queue->size)%p_queue->size;
	  return p_queue->valid_len;
}


/*
ret  1 sucess     0 fail
*/
uint8_t lib_en_queue(queue_t * p_queue,queue_type m)
{
    #ifdef FULL_JUDGE // full judge
			if(lib_queue_is_full(p_queue) == 1)
				return 0;
	  #endif
		
		p_queue->p_arr[(p_queue->rear)] = m; //add m to tail
		p_queue->rear = (p_queue->rear + 1)%p_queue->size; //updete rear
			return 1;
}


/*
ret  1 sucess     0 fail(empty)
*/
uint8_t lib_de_queue(queue_t * p_queue,queue_type * m)
{

		if(lib_queue_is_empty(p_queue) == 1)
			return 0;
		if(m != NULL)
			*m = p_queue->p_arr[p_queue->front]; //got *m
		
    #ifdef LIB_QUEUE_DEBUG
		   p_queue->p_arr[p_queue->front] = 0;
		#endif
		
		p_queue->front = (p_queue->front + 1)%p_queue->size; //updete front
			return 1;
}

/*
ret  queue avg
*/
queue_type lib_avg_queue(queue_t * p_queue)
{
		uint8_t i;
	  uint32_t sum=0;
	
		if(lib_queue_is_empty(p_queue) == 1)
			return 0;
		
		i = p_queue->front;
		
		while( i != p_queue->rear)
		{
			sum = sum + p_queue->p_arr[i];
			i = (i+1)%p_queue->size;
		}
		return sum/lib_get_queue_len(p_queue);
}






