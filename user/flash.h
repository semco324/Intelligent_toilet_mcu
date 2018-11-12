#ifndef __FLASH_H
#define __FLASH_H 	

#include "stm32f0xx.h"
#include "addr_map.h"
#include "csm37f59.h"
//带存储数据
/*
FREQ1K_VAL_H
FREQ1K_VAL_L
FREQ2K_VAL_H
FREQ2K_VAL_L
FREQ3K_VAL_H
FREQ3K_VAL_L
HR_CAL_VAL
*/

#define   DEF_SW_H          0x00
#define   DEF_SW_L          0x00

#define   DEF_HW_H          0x00
#define   DEF_HW_L          0x00


#define   FLASH_PAGE_ADDR  ((uint32_t)0x0800FC00)   /* hr_k of user Flash area */
#define   F_FREQ1K_VAL_H_ADDR     (FLASH_PAGE_ADDR+0)
#define   F_FREQ1K_VAL_L_ADDR     (FLASH_PAGE_ADDR+4)
#define   F_HR_CAL_VAL_ADDR       (FLASH_PAGE_ADDR+8)





int8_t    FLASH_PageWrite_halfword(uint32_t WriteAddr,uint16_t halfword);
uint16_t  FLASH_PageRead_halfword(uint32_t WriteAddr);
int8_t Cmd_WriteTypeByteToFlash(uint16_t data,uint32_t WriteAddr,uint8_t type);
void User_FMC_Init(void);//flash 存储初始化
void load_fmc_data(void);//加载默认数据

#endif

