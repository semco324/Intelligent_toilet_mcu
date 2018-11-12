#ifndef __BLE_PARSE_H
#define __BLE_PARSE_H

#include "driver_csm92p10.h"
#include "stm32f0xx.h"
#include "stdlib.h"
#include "lib_parse.h"
#include "eb_parse.h"


#define	  DEF_BLE_HAEDER   0xC7 //ble ͸��ģ���header
	  
#define	  DEF_BLE_MAX_LEN  23   //ble ͸��ģ��len�ֶ����ֵ
		
#define   BLE_USED_UART   USART1 //ble ͸��ģ������ô���
		
#define   BTRXSIZE         128   //BLE_USED_UART fifo ����MAXSIZE-1��Ԫ��

#define   BLE_START_TEST_CMD_LEN  12 //ble ���Ϳ�ʼ���������


extern rtc_t ble_rtc;    //ble rtc
       
extern queue_type btrx_arr[BTRXSIZE]; //bt fifo arr
extern uart_data_t    btrx_fifo;   //bt fifo manage struct

void ble_dealwith_queue(void);

#endif

