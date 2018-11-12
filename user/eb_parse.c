#include "eb_parse.h"

uart_data_t ebrx_fifo; //be fifo manage struct      
queue_type ebrx_arr[EBRXSIZE]; //be fifo arr


//1.���EB��������xor
//front��ʼ��uartlen��check      
//          
//           xx  head  len  cmd   data0  data1  data2  data3 ...  datan  xorsum
//                ^
//                |
//          front ָ��head
//len = data0 ~ datan �ֽ���
//xorsum = head ~ datan ����

void eb_switch_uartcmd(queue_t *fifo,uint8_t frame_len)//��������
{
		uint8_t i;
    switch( fifo->p_arr[ ((fifo->front+2)%fifo->size) ] )
			{
			case CMD_EB_PASSTHROUGH: //0x59
				   //if(fifo->p_arr[ ((fifo->front+19)%fifo->size) ] == 0x92)
					 {
								passthrough_to_ble(&fifo->p_arr[ ((fifo->front)%fifo->size) ]);
					 }
				break;
       
			default:break;			
			}
			
	 for(i=0;i<(frame_len+3);i++)  ///////head  len  xorsum  frame_len=len
     { 
		   lib_de_queue(fifo,NULL); 
		 } 
}


void eb_parse_cmd(uart_data_t * uart_data)
{
	 uint8_t i;	 
   if(uart_data->cmd_len == 0) return;

    lib_get_queue_len(&uart_data->fifo); //�õ�����Ԫ�ظ���
	 if( uart_data->fifo.valid_len >= (uart_data->cmd_len+3) ) //�����յ�һ����ע��������>=,��Ȼ���Ӻ�һ������
	 {
					if( lib_parse_check_xor( uart_data->fifo.front , uart_data->cmd_len ,&uart_data->fifo) == 0 ) //���ж��ⲿУ��
						{
							eb_switch_uartcmd(&uart_data->fifo,uart_data->cmd_len);//��������
						}
					else //����
						{
							for(i=0;i<(uart_data->cmd_len+3);i++)
								{
									lib_de_queue(&uart_data->fifo,NULL); 
								}
						}
	  }	 
}

void eb_dealwith_queue(void)
{
	  //Ӱ��ȫ�ֱ���btrx_frontָ��C7
    //��������Ԫ��Ѱ�ҵ�һ��btrx_front
	  lib_parse_find_head(&ebrx_fifo,DEF_EB_HAEDER);
		//Ӱ��ȫ�ֱ���uart_data->cmd_len
		//�õ�һ֡����
	  lib_parse_find_len(&ebrx_fifo,DEF_EB_HAEDER,DEF_EB_MAX_LEN);
	  eb_parse_cmd(&ebrx_fifo);
}





