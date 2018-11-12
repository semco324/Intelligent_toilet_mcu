#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f0xx.h"
/********************������ʱ��TIM�������壬ֻ��TIM6��7************/
#define BASIC_TIM3 // ���ʹ��TIM7��ע�͵�����꼴��

#ifdef  BASIC_TIM6 // ʹ�û�����ʱ��TIM6
#define            BASIC_TIM                   TIM6
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM6
#define            BASIC_TIM_Period            (100-1) //10ms
#define            BASIC_TIM_Prescaler         (4800-1)
#define            BASIC_TIM_IRQ               TIM6_IRQn
#define            BASIC_TIM_IRQHandler        TIM6_IRQHandler

#elif BASIC_TIM7  // ʹ�û�����ʱ��TIM7
#define            BASIC_TIM                   TIM7
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM7
#define            BASIC_TIM_Period            (100-1)   //10ms
#define            BASIC_TIM_Prescaler         (4800-1)
#define            BASIC_TIM_IRQ               TIM7_IRQn
#define            BASIC_TIM_IRQHandler        TIM7_IRQHandler

#else

#define            BASIC_TIM                   TIM3
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM3
#define            BASIC_TIM_Period            (100-1)   //10ms
#define            BASIC_TIM_Prescaler         (4800-1)
#define            BASIC_TIM_IRQ               TIM3_IRQn
#define            BASIC_TIM_IRQHandler        TIM3_IRQHandler

#endif
/**************************��������********************************/
#define OPEN_RXTIM()   TIM_Cmd(BASIC_TIM, ENABLE);	
#define CLOSE_RXTIM()  TIM_Cmd(BASIC_TIM, DISABLE);
void timer3_init(void);
void timer2_init(void);
void timer2_dis(void);
#endif
