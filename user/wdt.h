#ifndef __WDT_H
#define __WDT_H

#include "stm32f0xx.h"
#define LsiFreq  40000

void User_WDT_Init(void);

void User_WDT_DISABLE(void);
#endif
