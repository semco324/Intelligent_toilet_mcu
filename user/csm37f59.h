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
	uint8_t num;    //记录res数据有效元素个数
	uint8_t err_num;//记录测量失败次数
	uint8_t ok_num; //记录res数据符合要求次数
}a_res_t;



//sta_flg各状态说明
#define CS1258_IDLE                       0       // 0 空闲状态
#define CS1258_SEND_MEASURE_RES_CMD       1       // 1 发送了测量电阻命令
#define CS1258_START_MEASURE_RES          2       // 2 DRDY拉高开始测量电阻
#define CS1258_END_MEASURE_RES            3       // 3 DRDY拉低电阻测量完成
#define CS1258_SEND_MEASURE_HR_CMD        4       // 4 发送了心率测量命令
#define CS1258_START_MEASURE_HR           5       // 5 DRDY拉高开始测量心率
#define CS1258_END_MEASURE_HR             6       // 6 DRDY拉低心率测量完成

#define CS1258_SEND_CAL_K_CMD             7       // 7 发送了校准K值命令
#define CS1258_START_CAL_K                8       // 8 DRDY拉高开始校准K值
#define CS1258_END_CAL_K                  9       // 9 DRDY拉低校准K值完成

#define CS1258_SEND_CAL_HR_CMD            10      // 10发送了心率校准命令
#define CS1258_START_CAL_HR               11      // 11DRDY拉高开始校准心率
#define CS1258_END_CAL_HR                 12      // 12DRDY拉低校准心率完成

#define DEF_CALRES1_H   0x17
#define DEF_CALRES1_L   0x70 //第一频校准电阻600欧

#define CAL_HR   60 //心率校准值


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
