#include "DMA.h"
#include "stm32f1xx.h"
void DMA_Init()
{
	RCC->AHBENR |= 1<<0;//enable clock for DMA1
	DMA1_Channel1->CCR &=~ (1<<4);//read from peripheral
	DMA1_Channel1->CCR |=(1<<5);//enable circular mode
	DMA1_Channel1->CCR |=(1<<7);//enable memory increment
	DMA1_Channel1->CCR|=(1<<8);//peripheral size : 16 bit
	DMA1_Channel1->CCR|=(1<<10);//memory size : 16 bit
	
}
void DMA1_Config(uint32_t srcAdd, uint32_t destAdd, uint16_t size)
{
	DMA1_Channel1->CNDTR = size;
	DMA1_Channel1->CPAR = srcAdd;
	DMA1_Channel1->CMAR=destAdd;
	DMA1_Channel1->CCR |=(1<<0);//enable channel DMA
}