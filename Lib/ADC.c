#include "ADC.h"
#include "stm32f1xx.h"
void ADC1_Config()
{
	RCC->APB2ENR |= 1<<9;  // enable ADC1 clock
	RCC->APB2ENR |= (1<<2);  // enable GPIOA clock
	GPIOA->CRL &=(0xFFFFFF0F);
	GPIOA->CRL &=~(1<<4);// set GPIOA_PIN1 as ADC1_Channel 1
	GPIOA->CRL &=(0xFFF0FFFF);
	GPIOA->CRL &=~(1<<16);// set GPIOA_PIN4 as ADC1_Channel 4
	RCC->APB2ENR |= (1<<4);
	GPIOC-> CRH &= (0xF0FFFFFF);
	GPIOC-> CRH |= (2<<24);
        ADC1->CR1 |=(1<<8);//enable scan mode
	ADC1->CR2|=(1<<1);//enable continous mode
	ADC1->CR2|=(1<<8);//enble DMA
	ADC1->CR2 &=~(1<<11); //right alignment
	ADC1->CR2 |=(7<<17);//SWSTART in EXTSEL bit
	ADC1->CR2|=(1<<20);//Conversion on external event enabled
	ADC1->SMPR2 &= ~((7<<3) | (7<<12));  // Sampling time of 1.5 cycles for channel 1 and channel 4
	ADC1->SQR1|=(1<<20);//2 conversions
	ADC1->SQR3|=((1<<0)|(4<<5));//set up ADC channel for channel 1 and 4
	ADC1->CR2 |= (1 << 2); // turn calibrate on 
	while(!(ADC1->CR2 & (1 << 2)));//check calibrate
	ADC1->CR2|=(1<<0);//enable ADC1
	ADC1->CR2 |=(1<<22);//enable conversion
}