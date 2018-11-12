#include "usart.h"
#include "lib_delay.h"
//PB6.7
void uart_init(USART_TypeDef* USARTx,uint32_t bound) //BLE/EB
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  if(USARTx == USART1)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//使能USART1，GPIOB时钟
		
		/* USART1 Pins configuration ************************************************/
		/* Connect pin to Periph */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_0); 
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_0);    
		
		/* Configure pins as AF pushpull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	}
	else if(USARTx == USART2)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART2，GPIOA时钟

		/* USART2 Pins configuration ************************************************/
		/* Connect pin to Periph */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1); 
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);    
		
		/* Configure pins as AF pushpull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2| GPIO_Pin_3;
	}
	else 
		return;
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
  if(USARTx == USART1)
	{	 
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
		 /* USART1 IRQ Channel configuration */
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	}
	else if(USARTx == USART2)
	{
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    /* USART2 IRQ Channel configuration */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;		
	}
 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USARTx, &USART_InitStructure); //初始化串口1
	USART_ClearFlag(USARTx, USART_FLAG_RXNE);//防止配置完就进入中断
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//开启串口接受中断
	//USART_ITConfig(USARTx, USART_IT_ORE, ENABLE);//开启串口溢出中断
  USART_Cmd(USARTx, ENABLE);                    //使能串口1 

}

//使能/失能USARTx 的 tx功能
void usart_rx_mode(USART_TypeDef* USARTx,FunctionalState NewState)
{
   /* Write to USART CR1 */
	if(NewState == ENABLE)  
     USARTx->CR1 |= USART_Mode_Tx;
	else
		 USARTx->CR1 &= ~USART_Mode_Tx;
}

void uart_close(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,DISABLE);	//使能USART1，GPIOB时钟

  /* USART1 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
	//TXD 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //RXD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
  USART_Cmd(USART1, DISABLE);                    //使能串口1 

}



//PA2--->uart2.tx
//PA3--->uart2.rx
//void uart2_init(uint32_t bound) //电控板
//{
//  //GPIO端口设置
//  GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART2，GPIOA时钟


//  /* USART2 Pins configuration ************************************************/
//  /* Connect pin to Periph */
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1); 
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);    
//  
//  /* Configure pins as AF pushpull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2| GPIO_Pin_3;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);  

//  /* USART2 IRQ Channel configuration */
//  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //DISABLE
//  NVIC_Init(&NVIC_InitStructure);

//  //USART 初始化设置
//	USART_InitStructure.USART_BaudRate = bound;//串口波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//  USART_Init(USART2, &USART_InitStructure); //初始化串口2
//  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
//  USART_Cmd(USART2, ENABLE);                    //使能串口2 
//}


void UARTx_SendDate(USART_TypeDef* USARTx,uint8_t *arr,uint8_t len)
{
  uint8_t i=0;
	for(i=0;i<len;i++)
	{
		 while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		 USART_SendData(USARTx, arr[i]);
	}
	 while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

#include "ble_parse.h"
#include "eb_parse.h"
/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
int t=0;
void USART1_IRQHandler(void) //BLE
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
    {
       USART_ReceiveData(USART1);
       USART_ClearFlag(USART1, USART_FLAG_PE);
    }

		if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
		{
			 USART_ReceiveData(USART1);
		   USART_ClearFlag(USART1, USART_FLAG_ORE);
		}

		if (USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
		{
				USART_ReceiveData(USART1);
		    USART_ClearFlag(USART1, USART_FLAG_FE);
		}
		
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {   
        USART_ClearFlag(USART1, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			  lib_en_queue(&btrx_fifo.fifo,USART_ReceiveData(USART1));	
        t++;			
    }
}

/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
int s=0;
void USART2_IRQHandler(void)
{
    if (USART_GetFlagStatus(USART2, USART_FLAG_PE) != RESET)
    {
       USART_ReceiveData(USART2);
       USART_ClearFlag(USART2, USART_FLAG_PE);
    }

		if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
		{
			 USART_ReceiveData(USART2);
		   USART_ClearFlag(USART2, USART_FLAG_ORE);
		}

		if (USART_GetFlagStatus(USART2, USART_FLAG_FE) != RESET)
		{
				USART_ReceiveData(USART2);
		    USART_ClearFlag(USART2, USART_FLAG_FE);
		}
		
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {   
        USART_ClearFlag(USART2, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			  lib_en_queue(&ebrx_fifo.fifo,USART_ReceiveData(USART2));	
	s++;
    }
}