#ifndef __LIB_COMM_H
#define __LIB_COMM_H


#include "stm32f0xx.h"

#define TRUE          0
#define FALSE         1


typedef struct{
	uint8_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t week;
}rtc_t;

#endif

