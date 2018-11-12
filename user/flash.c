#include "flash.h"


void User_FMC_Init(void)                                  //flash 存储初始化
{

}
	
void load_fmc_data(void)                                  //加载默认数据
{
		//load default parm
		EquMent.FREQ         = 0x08;  //freq 50k
		EquMent.M_PART       = 0x01;  //part hand (1251)
	
	  EquInfo.SW_VER_H     = DEF_SW_H;
	  EquInfo.SW_VER_L     = DEF_SW_L;
	
	  EquInfo.HW_VER_H     = DEF_HW_H;
	  EquInfo.HW_VER_L     = DEF_HW_L; //ver
	
	  EquMent.FREQ1K_VAL_H = FLASH_PageRead_halfword(F_FREQ1K_VAL_H_ADDR)&0xff;
		EquMent.FREQ1K_VAL_L = FLASH_PageRead_halfword(F_FREQ1K_VAL_L_ADDR)&0xff; //k

	  if(EquMent.FREQ1K_VAL_H == 0xff || EquMent.FREQ1K_VAL_L == 0xff)
		{
				EquMent.FREQ1K_VAL_H = EquMent.FREQ1K_VAL_L = 0x00;  //1频k值 H L
		}
	
	  EquMent.HR_CAL_VAL   = FLASH_PageRead_halfword(F_HR_CAL_VAL_ADDR)&0xff; //cal_hr
	  if(  EquMent.HR_CAL_VAL>=(CAL_HR*CAL_HR/(CAL_HR-20)) ||\
				 EquMent.HR_CAL_VAL<=(CAL_HR*CAL_HR/(CAL_HR+20)) )
		{
			  EquMent.HR_CAL_VAL = CAL_HR;
				FLASH_PageWrite_halfword(F_HR_CAL_VAL_ADDR,EquMent.HR_CAL_VAL);
		}
		
		all_result_pkg = 1+ ((sizeof(Analys)) + VAILE_PKG_RESULT_LEN -1)/VAILE_PKG_RESULT_LEN ;
}
		
		
int8_t  FLASH_PageWrite_halfword(uint32_t WriteAddr,uint16_t halfword)
{
			FLASH_Unlock();
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
	
	    if( FLASH_ErasePage(WriteAddr) != FLASH_COMPLETE)
				return -1;
			
			if( FLASH_ProgramHalfWord(WriteAddr, halfword) != FLASH_COMPLETE)
				return -1;
			
			FLASH_Lock();
			return 0;
}

uint16_t  FLASH_PageRead_halfword(uint32_t WriteAddr)
{
			uint32_t data=0;
	
	    data = *(__IO uint32_t *)WriteAddr;
	
	    return data&0xffff;
}
//1 16bit
//0 8bit
int8_t Cmd_WriteTypeByteToFlash(uint16_t data,uint32_t WriteAddr,uint8_t type)
{  
  if(type)
	{
	   FLASH_PageWrite_halfword(WriteAddr,data>>8);
	   if(FLASH_PageRead_halfword(WriteAddr) != (data>>8))
		  return FALSE; 
		 FLASH_PageWrite_halfword(WriteAddr+4,data&0xff);
	   if(FLASH_PageRead_halfword(WriteAddr+4) != (data&0xff))
		  return FALSE; 	 
	}
	else
  {
		 FLASH_PageWrite_halfword(WriteAddr,data);
	   if(FLASH_PageRead_halfword(WriteAddr) != data)
		  return FALSE; 
	}
  return TRUE;
}



