#include "timer.h"

void timer3_init(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		/* TIM3 clock enable */		// 开启定时器时钟,即内部时钟CK_INT=48M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);

		/* Enable the TIM3 gloabal Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = BASIC_TIM_Period;	

	  // 时钟预分频数为
    TIM_TimeBaseStructure.TIM_Prescaler= BASIC_TIM_Prescaler;
	
		// 时钟分频因子 ，基本定时器没有，不用管
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		
		// 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		
		// 重复计数器的值，基本定时器没有，不用管
		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	
	  // 初始化定时器
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);
		
		// 清除计数器中断标志位
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);
	  
		// 开启计数器中断
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
		
		// 使能计数器
    TIM_Cmd(BASIC_TIM, ENABLE);	
	
}


//void timer2_init(void)
//{
//		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//		NVIC_InitTypeDef NVIC_InitStructure;

//		/* TIM2 clock enable */		// 开启定时器时钟,即内部时钟CK_INT=48M
//    BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);

//		/* Enable the TIM2 gloabal Interrupt */
//		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
//    TIM_TimeBaseStructure.TIM_Period = (50-1);	 //5ms

//	  // 时钟预分频数为
//    TIM_TimeBaseStructure.TIM_Prescaler= (4800-1);
//	
//		// 时钟分频因子 ，基本定时器没有，不用管
//    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//		
//		// 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
//    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//		
//		// 重复计数器的值，基本定时器没有，不用管
//		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
//	
//	  // 初始化定时器
//    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//		
//		// 清除计数器中断标志位
//    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	  
//		// 开启计数器中断
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		// 使能计数器
//    TIM_Cmd(TIM2, DISABLE);	
//	
//}

//void timer2_en(void)
//{
//		// 开启计数器中断
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		// 使能计数器
//    TIM_Cmd(TIM2, DISABLE);	

//}

//void timer2_dis(void)
//{
//		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//		NVIC_InitTypeDef NVIC_InitStructure;

//		/* TIM2 clock enable */		// 开启定时器时钟,即内部时钟CK_INT=48M
//    BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);

//		/* Enable the TIM2 gloabal Interrupt */
//		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
//    TIM_TimeBaseStructure.TIM_Period = (50-1);	 //5ms

//	  // 时钟预分频数为
//    TIM_TimeBaseStructure.TIM_Prescaler= (4800-1);
//	
//		// 时钟分频因子 ，基本定时器没有，不用管
//    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//		
//		// 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
//    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//		
//		// 重复计数器的值，基本定时器没有，不用管
//		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
//	
//	  // 初始化定时器
//    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//		
//		// 清除计数器中断标志位
//    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	  
//		// 开启计数器中断
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		// 使能计数器
//    TIM_Cmd(TIM2, DISABLE);	
//	
//}

