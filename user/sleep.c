#include "sleep.h"

uint16_t sleep_cnt = 0;
uint8_t addr_mat = 0 ; 

void check_sleep(void)
{
//	  int32_t work_cnt; //唤醒后工作一段时间
//	  uint8_t sleep_cmd=0x35;	 //sleep cmd
//	  uint8_t wakeup_cmd=0x53; //wakeup cmd
	
//    work_cnt=0;
	 
    if(READ_STATUS()==1 && sleep_cnt>=SLEEP_TIME) //蓝牙断开时间长达SLEEP_TIME，就sleep
		{	   		  
			  PowerDownCS125x();
			  TIM_Cmd(BASIC_TIM, DISABLE);						//关tim3
//			  TIM_Cmd(TIM2, DISABLE);						      //关tim2
			  //User_WDT_DISABLE();                   //关wdt
			  //USART_Cmd(USART1, DISABLE);       		//关uart  
				uart_close();
		  
			  IIC_Init();
			  setup_key_enable();//使能status中断

			  while(1)
				{
					  /* Enable the PWR clock */
					//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
					/* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 */
				  PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
				  SystemInit();

					if(addr_mat)
					{				   
					   addr_mat=0;sleep_cnt=0;
						 _CS1258bug();				
						 setup_key_disable();                 //关闭status中断	       
	           load_fmc_data();                     //加载默认数据
	           //User_WDT_Init();
	           //uart_init(115200);      //蓝牙uart 初始化  PA10--rx  PA11--tx
             uart_init(BLE_USED_UART,115200);
	           //uart_init(115200);          //电控uart 初始化  PA6--rx  PA7--tx  
						 uart_init(EB_USED_UART,115200);					
             timer3_init();     			//定时查询CSM37F58 初始化
//						 timer2_init(); 
             break;						
					}
				}	
		}
		else if(READ_STATUS()==0)
    {
		  sleep_cnt = 0;
		}
}


void no_use_peripherals(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  /* Configure PA9 and PA10 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|\
																GPIO_Pin_7|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_0); 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_0);	
	
  /* Configure PA9 and PA10 in output pushpull mode */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|\
//																GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure PA9 and PA10 in output pushpull mode */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /* Configure PA9 and PA10 in output pushpull mode */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
  /* Configure PA9 and PA10 in output pushpull mode */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
//  /* Configure PA9 and PA10 in output pushpull mode */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

	
	  SystemInit();
		ADC_DeInit(ADC1);
		CAN_DeInit(CAN);
	  CEC_DeInit();
		COMP_DeInit();
		CRS_DeInit();
		DAC_DeInit();
    //DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx)
		GPIO_DeInit(GPIOA);
		GPIO_DeInit(GPIOB);
		GPIO_DeInit(GPIOC);
		GPIO_DeInit(GPIOD);
		GPIO_DeInit(GPIOE);
		GPIO_DeInit(GPIOF);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, DISABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, DISABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, DISABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, DISABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, DISABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, DISABLE);
		I2C_DeInit(I2C1);
		I2C_DeInit(I2C2);
		RTC_DeInit();
    SPI_I2S_DeInit(SPI1);
	  SPI_I2S_DeInit(SPI2);

		TIM_DeInit(TIM1);
		TIM_DeInit(TIM3);
		TIM_DeInit(TIM6);	
		TIM_DeInit(TIM7);
		TIM_DeInit(TIM14);
		TIM_DeInit(TIM15);	
		TIM_DeInit(TIM16);
		TIM_DeInit(TIM17);
		
		USART_DeInit(USART1);
		USART_DeInit(USART2);
		
		WWDG_DeInit();
}

