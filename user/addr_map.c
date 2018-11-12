
#include "addr_map.h"
#include <string.h>



EquipmentConfig_TypeDef EquMent; //设备配置区
EquipmentInfo_TypeDef   EquInfo; //设备信息区
UserInfo_TypeDef        UsrInfo; //用户信息区
UserBIAPart_TypeDef     UsrBIAP; //用户BIA测试区
Analysis_TypeDef        Analys;  //分析结果区
ADCVal_TypeDef          ADCVal;  //ADC值区
RESVal_TypeDef          RESVal;  //真实电阻区



/**
  * @brief  地址映射 根据用户传入的高地址和低地址找到内部对应的地址
  * @param  uint16_t user_addr
	          用户传入高地址、低地址
  * @retval 内部地址
  */
uint8_t addr_flag=0;




//清楚ram
void Clr_All_Block_Ram(void)
{
	memset(&EquMent,0,sizeof(EquMent));
	memset(&EquInfo,0,sizeof(EquInfo));
	memset(&UsrInfo,0,sizeof(UsrInfo));
	memset(&UsrBIAP,0,sizeof(UsrBIAP));
	memset(&Analys,0,sizeof(Analys));
	memset(&ADCVal,0,sizeof(ADCVal));
	memset(&RESVal,0,sizeof(RESVal));
}


void Clr_UserBIAPart_Ram(void)
{
		memset(&UsrBIAP,0,sizeof(UsrBIAP));
}

void Clr_Analysis_Ram(void)
{
		memset(&Analys,0,sizeof(Analys));
}

//注意123频的内段 r0 r1不能清
void Clr_ADCVal_Ram(void)
{
		memset(&(ADCVal.Freq1_Z12_AD_H),0,sizeof(ADCVal)-12);
}

void Clr_RESVal_Ram(void)
{
		memset(&RESVal,0,sizeof(RESVal));
}


void Clr_UsrInfo_ResF1(void)
{
		//memset(&UsrInfo,0,sizeof(UsrInfo));
}




