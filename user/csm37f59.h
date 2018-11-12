#ifndef __TEST_CSM_H
#define __TEST_CSM_H

#include "stm32f0xx.h"
#include "lib_myiic.h"
#include "lib_comm.h"
#include "driver_csm92p10.h"
#include "driver_cs1258.h"
#include "lib_delay.h"
#include "flash.h"
#include "usart.h"
#include "addr_map.h"


#define A_RES_SIZE 6
typedef struct{
	uint16_t res[A_RES_SIZE];
	uint8_t num;    //��¼res������ЧԪ�ظ���
	uint8_t err_num;//��¼����ʧ�ܴ���
	uint8_t ok_num; //��¼res���ݷ���Ҫ�����
}a_res_t;



//sta_flg��״̬˵��
#define CS1258_IDLE                       0       // 0 ����״̬
#define CS1258_SEND_MEASURE_RES_CMD       1       // 1 �����˲�����������
#define CS1258_START_MEASURE_RES          2       // 2 DRDY���߿�ʼ��������
#define CS1258_END_MEASURE_RES            3       // 3 DRDY���͵���������
#define CS1258_SEND_MEASURE_HR_CMD        4       // 4 ���������ʲ�������
#define CS1258_START_MEASURE_HR           5       // 5 DRDY���߿�ʼ��������
#define CS1258_END_MEASURE_HR             6       // 6 DRDY�������ʲ������

#define CS1258_SEND_CAL_K_CMD             7       // 7 ������У׼Kֵ����
#define CS1258_START_CAL_K                8       // 8 DRDY���߿�ʼУ׼Kֵ
#define CS1258_END_CAL_K                  9       // 9 DRDY����У׼Kֵ���

#define CS1258_SEND_CAL_HR_CMD            10      // 10����������У׼����
#define CS1258_START_CAL_HR               11      // 11DRDY���߿�ʼУ׼����
#define CS1258_END_CAL_HR                 12      // 12DRDY����У׼�������

#define DEF_CALRES1_H   0x17
#define DEF_CALRES1_L   0x70 //��һƵУ׼����600ŷ

#define CAL_HR   60 //����У׼ֵ


extern uint8_t mes_flg;
extern uint8_t calres_flg;
extern uint8_t calhr_flg;


extern uint8_t tim_cnt;
extern uint8_t sta_flg;

extern uint8_t bh_val;
extern uint8_t mt_status;
extern uint8_t premt_status;

extern uint8_t cal_err;
	



void measure(void);
void update_sta_flg(void);
void Get_BR(void);

void setup_key_enable(void);
void setup_key_disable(void);
#endif
