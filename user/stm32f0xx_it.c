/**
  ******************************************************************************
  * @file    ADC/ADC_AnalogWatchdog/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "timer.h"	
#include "csm37f59.h"	
#include "driver_cs1258.h"	
/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_AnalogWatchdog
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__asm void wait()
{
	BX lr
}
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
 wait();
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/******************************************************************************/
/**
  * @brief  This function handles ADC1 global interrupts requests.
  * @param  None
  * @retval None
  */
void ADC1_COMP_IRQHandler(void)
{
  if(ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET)
  {
    /* Clear ADC1 AWD pending interrupt bit */
    ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
  }
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

//#include "ble_parse.h"
//#include "eb_parse.h"
///**
//  * @brief  This function handles USARTy global interrupt request.
//  * @param  None
//  * @retval None
//  */
//int t=0;
//void USART1_IRQHandler(void) //BLE
//{
//    if (USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
//    {
//       USART_ReceiveData(USART1);
//       USART_ClearFlag(USART1, USART_FLAG_PE);
//    }

//		if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
//		{
//			 USART_ReceiveData(USART1);
//		   USART_ClearFlag(USART1, USART_FLAG_ORE);
//		}

//		if (USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
//		{
//				USART_ReceiveData(USART1);
//		    USART_ClearFlag(USART1, USART_FLAG_FE);
//		}
//		
//    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {   
//        USART_ClearFlag(USART1, USART_FLAG_RXNE);
//        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//			  lib_en_queue(&btrx_fifo.fifo,USART_ReceiveData(USART1));	
//        t++;			
//    }
//}

///**
//  * @brief  This function handles USARTy global interrupt request.
//  * @param  None
//  * @retval None
//  */
//int s=0;
//void USART2_IRQHandler(void)
//{
//    if (USART_GetFlagStatus(USART2, USART_FLAG_PE) != RESET)
//    {
//       USART_ReceiveData(USART2);
//       USART_ClearFlag(USART2, USART_FLAG_PE);
//    }

//		if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
//		{
//			 USART_ReceiveData(USART2);
//		   USART_ClearFlag(USART2, USART_FLAG_ORE);
//		}

//		if (USART_GetFlagStatus(USART2, USART_FLAG_FE) != RESET)
//		{
//				USART_ReceiveData(USART2);
//		    USART_ClearFlag(USART2, USART_FLAG_FE);
//		}
//		
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//    {   
//        USART_ClearFlag(USART2, USART_FLAG_RXNE);
//        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//			  lib_en_queue(&ebrx_fifo.fifo,USART_ReceiveData(USART2));	
//	s++;
//    }
//}


/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_8_IRQHandler(void)
{
  if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    //aRxBuffer[5][RxCounter++] = USART_ReceiveData(USART6);
  } 
}
#include "sleep.h"

void TIM2_IRQHandler(void)
{
//	if ( TIM_GetITStatus( TIM2, TIM_IT_Update) != RESET ) 
//	{	
//		wait_out++;
//	 if(wait_out>20)
//	 {
//	   wait_out=20;
//	 }
//	}
}
//uint8_t j=0;
void  BASIC_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
//		j=(j+1)%2;
//	 GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(j));
		wait_out++;
	 if(wait_out>20)
	 {
	   wait_out=20;
	 }
	 if(++sleep_cnt > SLEEP_TIME ) sleep_cnt = SLEEP_TIME;
	 if(++tim_cnt>INTERVAL_SEND) tim_cnt=INTERVAL_SEND;
		
   TIM_ClearITPendingBit(BASIC_TIM , TIM_FLAG_Update);  		 
	}		 	
}

/**
  * @brief  This function handles External lines 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
//EXTI4_15_IRQHandler DRDY
void EXTI4_15_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {    
    /* Clear the EXTI line 8 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line8);

  }
	
  if(EXTI_GetITStatus(EXTI_Line5) != RESET)  //¿∂—¿÷–∂œªΩ–—mcu
  {
    /* Clear the EXTI line 5 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line5);
		
		 if(GPIO_ReadInputDataBit(BLEStatus_GPIO,BLEStatus_Pin)==0)
		  {
				addr_mat=1;
			}//mes_flg=1;	sta_flg=0;	err_flg=0; done=0;}
  }
}
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
