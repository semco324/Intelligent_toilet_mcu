#ifndef __DRIVER_CSM92P10_H
#define __DRIVER_CSM92P10_H

#include "stm32f0xx.h"
#include "lib_delay.h"
#include "csm37f59.h"
#include <string.h>
#include "ble_parse.h"

/*ble_Rx     --- 61_Tx PA11
	ble_Tx     --- 61_Rx PA10
	ble_Wakeup(5) --- f030    PB8
	ble_status(3) --- f030    PB5
	
*/



#define  BLESTATUS_REST     0
#define  BLESTATUS_BOOT     1
#define  BLESTATUS_STANDY   2
#define  BLESTATUS_ADV      3
#define  BLESTATUS_CONNECT  4
#define  BLESTATUS_SLEEP    5

#define  SETADVCMD          0x24
#define  SENDAPPDATACMD     0x30

//#define  HANDLE_FFF1_L      0x17
//#define  HANDLE_FFF1_H      0x00

#define  HANDLE_2A9C_L      0x15
#define  HANDLE_2A9C_H      0x00


#define BLEStatus_GPIO      GPIOB
#define BLEStatus_Pin       GPIO_Pin_5

#define BLEWakeup_GPIO      GPIOB
#define BLEWakeup_Pin       GPIO_Pin_8

	
#define  READ_STATUS()      GPIO_ReadInputDataBit(BLEStatus_GPIO,BLEStatus_Pin)
	
#define  INTERVAL_SEND 	100
#define  VAILE_PKG_RESULT_LEN 14 //20 - head(1) - len(1) - cmd(1) - all_pkg(1) - cur_pkg(1) - xor(1)

extern uint8_t TxBleData[30];
extern uint8_t blestatus;
extern uint8_t app_mes;
extern uint8_t app_calres;
extern uint8_t app_calhr;
extern uint8_t done;	
extern uint8_t ack_flg;		
extern uint8_t result_ack_flg; //结果应答
extern uint8_t all_result_pkg; //总包数
extern uint8_t cur_result_pkg; //当前包数

void Ble_SendAppAck(void);														
void McuToBle_Proc(void);
void passthrough_to_ble(uint8_t * src);
#endif

