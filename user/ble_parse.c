#include "ble_parse.h"
#include <string.h>

uart_data_t btrx_fifo;         //bt fifo manage struct      
queue_type btrx_arr[BTRXSIZE]; //bt fifo arr
rtc_t ble_rtc;                 //ble rtc



//1.���BT��������xor
//front��ʼ��uartlen��check      
//                                                      head   len    cmd    data0 ...  xorsum
//           xx  head  len  notify  handle_l  handle_h  data0  data1  data2  data3 ...  datan  xorsum
//                ^
//                |
//          front ָ��head
//len = notify ~ datan �ֽ���
//xorsum = head ~ datan ����

//2.���data0 ~ datan����xor
// data0  data1 data2 ...  datan
// ����ͷ ����  ָ��   ���� У��


void ble_switch_uartcmd(queue_t *fifo,uint8_t frame_len)
{
		uint8_t uartcmdfront;
		uint8_t uartcmdlen,i;
	
		uartcmdfront = ( fifo->front+5 ) %  fifo->size;//5
		uartcmdlen =   fifo->p_arr[ ((fifo->front+6)%fifo->size) ];//5+1
		
		if( lib_parse_check_xor( uartcmdfront, uartcmdlen ,fifo) == 0) //���ж��ڲ�У��
		{
			 switch( fifo->p_arr[ ((fifo->front+2)%fifo->size) ] )
				{
				 case 0x13:  //�ֻ��´�����֪ͨ
					        switch( fifo->p_arr[ ((fifo->front+7)%(fifo->size)) ] )
									{
										  
										  case 0x57: result_ack_flg=1; //���Ӧ��
																 //all_result_pkg = fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ]; //�ܰ���
																 cur_result_pkg = fifo->p_arr[ ((fifo->front+9)%(fifo->size)) ]; //��ǰ��Ӧ����
										             break;
										  case 0x56: ack_flg=1;        
										             break; //app Ӧ����Ͱ�ϴ�״̬��У׼���衢У׼����
										
										  case 0x55: app_mes=1;done=0; //app ble �·���������
										             if(fifo->p_arr[ ((fifo->front+6)%(fifo->size)) ] == BLE_START_TEST_CMD_LEN)
																 { 
																	  memcpy(&UsrInfo,&fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ],sizeof(UsrInfo)); 
																	  memcpy(&ble_rtc,&fifo->p_arr[ ((fifo->front+8+sizeof(UsrInfo))%(fifo->size)) ],sizeof(ble_rtc));
																 }
										             break; //app �·���ʼ��������
										  case 0x58:  //BLE͸������ذ�
										             if(fifo->p_arr[ ((fifo->front+6)%(fifo->size)) ] <=17 && fifo->p_arr[ ((fifo->front+6)%(fifo->size)) ]>0)// 16 data + 1 cmd(0x58)
																 { 
																	 //usart_rx_mode(BLE_USED_UART, DISABLE); //ʧ��BLE����2 tx
																	 //usart_rx_mode(EB_USED_UART,ENABLE);   //ʹ�ܵ�ش���1 tx
																	 //delay_ms(1);
//																	 if(fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ] != 0x5A)
//																		 fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ] = 0xA7;
   																 UARTx_SendDate(EB_USED_UART,&fifo->p_arr[ ((fifo->front+5)%(fifo->size)) ],fifo->p_arr[ ((fifo->front+6)%(fifo->size)) ]+3);			   
											             //delay_ms(1);
																	 //usart_rx_mode(EB_USED_UART,DISABLE); //ʧ�ܵ�ش���1 tx
																	 //usart_rx_mode(BLE_USED_UART, ENABLE); //ʹ��BLE����2 tx
																 }
												         break;
										
										  case 0xCA: if(fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ] == 0xCA)  //У׼��������
											            {app_calres=1;done=0;} 
																 else if(fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ] == 0xCB)  //У׼��������
											            {app_calhr=1;done=0;} 	
																	break; 
																	
										  default:break;									
									}	break;															 
				 default:break;
				}
		}
	 for(i=0;i<(frame_len+3);i++)  ///////head  len  xorsum frame_len=len
     { 
		   lib_de_queue(fifo,NULL); 
		 } 	 
}



void ble_parse_cmd(uart_data_t * uart_data)
{
	 uint8_t i;	 
   if(uart_data->cmd_len == 0) return;

    lib_get_queue_len(&uart_data->fifo); //�õ�����Ԫ�ظ���
	 if( uart_data->fifo.valid_len >= (uart_data->cmd_len+3) ) //�����յ�һ����ע��������>=,��Ȼ���Ӻ�һ������
	 {
					if( lib_parse_check_xor( uart_data->fifo.front , uart_data->cmd_len ,&uart_data->fifo) == 0 ) //���ж��ⲿУ��
						{
							ble_switch_uartcmd(&uart_data->fifo,uart_data->cmd_len);//��������
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


void ble_dealwith_queue(void)
{
	  //Ӱ��ȫ�ֱ���btrx_frontָ��C7
    //��������Ԫ��Ѱ�ҵ�һ��btrx_front
	  lib_parse_find_head(&btrx_fifo,DEF_BLE_HAEDER);
		//Ӱ��ȫ�ֱ���uart_data->cmd_len
		//�õ�һ֡����
	  lib_parse_find_len(&btrx_fifo,DEF_BLE_HAEDER,DEF_BLE_MAX_LEN);
	  ble_parse_cmd(&btrx_fifo);
}
















//																		UsrInfo.WTH  = fifo->p_arr[ ((fifo->front+8)%(fifo->size)) ]; //WGT
//																	  UsrInfo.WTL  = fifo->p_arr[ ((fifo->front+9)%(fifo->size)) ]; 
//																	  UsrInfo.HT   = fifo->p_arr[ ((fifo->front+10)%(fifo->size)) ];//HT
//																	  UsrInfo.SAGE = fifo->p_arr[ ((fifo->front+11)%(fifo->size)) ];//SEAG
//																	  ble_rtc.year = fifo->p_arr[ ((fifo->front+12)%(fifo->size)) ];//year
//																	  ble_rtc.mon  = fifo->p_arr[ ((fifo->front+13)%(fifo->size)) ];//mon
//																	  ble_rtc.day  = fifo->p_arr[ ((fifo->front+14)%(fifo->size)) ];//day
//																	  ble_rtc.hour = fifo->p_arr[ ((fifo->front+15)%(fifo->size)) ];//hour
//																	  ble_rtc.min  = fifo->p_arr[ ((fifo->front+16)%(fifo->size)) ];//min
//																	  ble_rtc.sec  = fifo->p_arr[ ((fifo->front+17)%(fifo->size)) ];//sec
//																	  ble_rtc.week = fifo->p_arr[ ((fifo->front+18)%(fifo->size)) ];//week
