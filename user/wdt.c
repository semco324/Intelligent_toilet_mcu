#include "wdt.h"

/**
  * @brief  wdt 初始化 409.6ms 中断
  * @param  None
  * @retval None
  */
void User_WDT_Init(void)
{
    

	
}


void User_WDT_DISABLE(void)
{
//    WDT_InitTypeDef WDT_InitStruct; 
//    
//    //NVIC_IRQxEnable(IRQ_WDT_WWDT); //使能wdt中断
//    
//    RCC_APBPeriphClOCKCmd(RCC_APBPeriph_WDT , ENABLE); //打开wdt时钟源
//    RCC_WDTCLKSourceConfig(RCC_CLKSource_LIRC); //选择wdt时钟源

//    WDT_InitStruct.WDT_PeriodTime = WDT_PeriodTime_2q18;//WDT_PeriodTime_2q12; //设置计数周期 409.6 ms
//    WDT_InitStruct.WDT_Int_Enable = DISABLE; //允许中断
//	  WDT_InitStruct.WDT_IntWakeUp_Enable= DISABLE; //允许唤醒
//    WDT_InitStruct.WDT_ReSystem_Enable = DISABLE; //允许复位系统
//    WDT_InitStruct.WDT_DelayTime = WDT_DelayTime_130CLK; //复位延时时间
//    WDT_InitStruct.WDT_Enable = DISABLE; //使能
//    
//    WDT_Init(&WDT_InitStruct); //初始化wdt

}
