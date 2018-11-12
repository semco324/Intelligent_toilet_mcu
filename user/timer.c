#include "timer.h"

void timer3_init(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		/* TIM3 clock enable */		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=48M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);

		/* Enable the TIM3 gloabal Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_Period = BASIC_TIM_Period;	

	  // ʱ��Ԥ��Ƶ��Ϊ
    TIM_TimeBaseStructure.TIM_Prescaler= BASIC_TIM_Prescaler;
	
		// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		
		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		
		// �ظ���������ֵ��������ʱ��û�У����ù�
		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	
	  // ��ʼ����ʱ��
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);
		
		// ����������жϱ�־λ
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);
	  
		// �����������ж�
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
		
		// ʹ�ܼ�����
    TIM_Cmd(BASIC_TIM, ENABLE);	
	
}


//void timer2_init(void)
//{
//		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//		NVIC_InitTypeDef NVIC_InitStructure;

//		/* TIM2 clock enable */		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=48M
//    BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);

//		/* Enable the TIM2 gloabal Interrupt */
//		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
//    TIM_TimeBaseStructure.TIM_Period = (50-1);	 //5ms

//	  // ʱ��Ԥ��Ƶ��Ϊ
//    TIM_TimeBaseStructure.TIM_Prescaler= (4800-1);
//	
//		// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
//    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//		
//		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
//    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//		
//		// �ظ���������ֵ��������ʱ��û�У����ù�
//		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
//	
//	  // ��ʼ����ʱ��
//    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//		
//		// ����������жϱ�־λ
//    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	  
//		// �����������ж�
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		// ʹ�ܼ�����
//    TIM_Cmd(TIM2, DISABLE);	
//	
//}

//void timer2_en(void)
//{
//		// �����������ж�
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		// ʹ�ܼ�����
//    TIM_Cmd(TIM2, DISABLE);	

//}

//void timer2_dis(void)
//{
//		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//		NVIC_InitTypeDef NVIC_InitStructure;

//		/* TIM2 clock enable */		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=48M
//    BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);

//		/* Enable the TIM2 gloabal Interrupt */
//		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
//    TIM_TimeBaseStructure.TIM_Period = (50-1);	 //5ms

//	  // ʱ��Ԥ��Ƶ��Ϊ
//    TIM_TimeBaseStructure.TIM_Prescaler= (4800-1);
//	
//		// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
//    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//		
//		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
//    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//		
//		// �ظ���������ֵ��������ʱ��û�У����ù�
//		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
//	
//	  // ��ʼ����ʱ��
//    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//		
//		// ����������жϱ�־λ
//    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	  
//		// �����������ж�
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		// ʹ�ܼ�����
//    TIM_Cmd(TIM2, DISABLE);	
//	
//}

