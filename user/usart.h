#ifndef __USART_H
#define __USART_H

#include "stm32f0xx.h"


void uart_init(USART_TypeDef* USARTx,uint32_t bound);
void uart_close(void);

void UARTx_SendDate(USART_TypeDef* USARTx,uint8_t *arr,uint8_t len);

void usart_rx_mode(USART_TypeDef* USARTx,FunctionalState NewState);
#endif

