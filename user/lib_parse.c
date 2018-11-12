#include "lib_parse.h"


//Ӱ��ȫ�ֱ���xxrx_frontָ��header
//��������Ԫ��Ѱ�ҵ�һ��xxrx_front
void lib_parse_find_head( uart_data_t * uart_data ,uint8_t header)
{
	 uint8_t i;

   i = uart_data->fifo.front;
	 while(i != uart_data->fifo.rear)
	 {
			if( uart_data->fifo.p_arr[i] != header)
			{
				lib_de_queue(&uart_data->fifo,NULL); //��Ϊframe head��clean
			}
			else if(uart_data->fifo.p_arr[i] == header)
			{
				break;
			}
			i=(i+1)%uart_data->fifo.size;
	 }
}

//Ӱ��ȫ�ֱ���uart_data->cmd_len
//�õ�һ֡����
void lib_parse_find_len(uart_data_t * uart_data,uint8_t header,uint8_t max_len)
{
	 uint8_t i;
   lib_get_queue_len(&uart_data->fifo); //�õ�����Ԫ�ظ���
	
   if( uart_data->fifo.p_arr[uart_data->fifo.front] == header && uart_data->fifo.valid_len >= 2 )
	 {
		 i=((uart_data->fifo.front+1)%uart_data->fifo.size); // len index
	   uart_data->cmd_len = uart_data->fifo.p_arr[i];
		 if(uart_data->cmd_len > max_len)  //һ����󲻻ᳬ�� max_len byte
		 {
			 uart_data->fifo.p_arr[uart_data->fifo.front] = 0;
			 lib_de_queue(&uart_data->fifo,NULL);
		   uart_data->cmd_len=0;
		 }
	 }
	 else
	 {
		 uart_data->cmd_len=0;
	 }
}


//�����check_front��ʼcheck_uartlen+1(header)+1(len) �� xor У���Ƿ���ȷ
//ģ��  header len cmd data0 data1 ... xor
//        ^
//        |
//check_front ָ��header

//1 err 0 ok
uint8_t lib_parse_check_xor( uint8_t check_front, uint8_t check_uartlen,queue_t *fifo)
{
	uint8_t i,tmpcheck=0;
	                                         //         btrx_front + len + 2
	 for(i = check_front; i != ( ( check_front+check_uartlen + 2 ) % (fifo->size) ); i= ( ( i + 1 ) % (fifo->size) ) )
	 {
		 tmpcheck = tmpcheck ^ fifo->p_arr[i];
	 }	 
	 //USART_SendData(USART2, tmpcheck);
	 if( tmpcheck == fifo->p_arr[ ((check_front+check_uartlen+2)%(fifo->size)) ] )
		 return 0;
	 else
		 return 1;	
}



