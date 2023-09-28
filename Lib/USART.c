#include "USART.h"
#include "stm32f1xx.h"

void USART1_Config()
{
	RCC->APB2ENR |= (1<<2);//enable clock for gpioa
	GPIOA->CRH&=(0xFFFFFF0F);
	GPIOA->CRH|=(10<<4);//SET gpioa pin 9 as alternate outout function push pull for TX
	RCC->APB2ENR|=(1<<14);//enable clock for USART1
	USART1->BRR=833;
	USART1->CR1|=(1<<3);//Transmit enable
	USART1->CR1|=(1<<13);// USART enable
}	
void USART1_Send_char(char str)
{
	while (!(USART1->SR & USART_SR_TXE));
	USART1->DR = str;
	
}
void USART1_Send_String(char *str)
{
	
	while(*str)
	{
		
		USART1_Send_char(*str++);
	}
}