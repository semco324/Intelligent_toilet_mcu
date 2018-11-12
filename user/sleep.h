#ifndef __SLEEP_H
#define __SLEEP_H

#include "stm32f0xx.h"
#include "timer.h" 
#include "driver_csm92p10.h" 
#include "csm37f59.h"
#include "lib_myiic.h"
#include "usart.h"

extern uint8_t addr_mat;
extern uint16_t sleep_cnt;

#define SLEEP_TIME  2000

void check_sleep(void); 
void no_use_peripherals(void);
#endif
