#include "stm32f1xx.h"
#include <stdint.h>
#include <stdio.h>
uint16_t adc_value;
uint16_t duty;
char str[10];
uint16_t data_sent;
void TIM2_conf()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC=7999;
	TIM2->CR1 |=(1<<0);
	while (!(TIM2->SR & TIM_SR_UIF));
}
void delay_ms(uint16_t ms)
{
	TIM2->CNT=0;
	while(TIM2->CNT <= ms);
}
void Tim1_PWM_Config()
{
	RCC->APB2ENR |=(1<<2);//enable clock for GPIOA
	GPIOA->CRH &=(0xFFFFFFF0);
	GPIOA->CRH |=(10<<0); //set gpioa pin 8 as output alternate function push pull for timer 1 channel 1
	GPIOA->CRH &=(0xFFFFF0FF);
	GPIOA->CRH |=(10<<8);//set gpioa pin 10 as output alternate function push pull for timer 1 channel 3
	RCC->APB2ENR |=(1<<11);
	TIM1->PSC=7;
	TIM1->ARR=999;
	TIM1->CCER|=((1<<0)|(1<<8));//enable caption / compare output for channel 1 vs 3
	TIM1->CCMR1 |=(6<<4);//set mode 1 for channel 1 
	TIM1->CCMR2 |=(6<<4);//set mode 1 for channel 3
	TIM1->CCR1=0;
	TIM1->CCR3=0;
	TIM1->BDTR |= TIM_BDTR_MOE;// Enable the main output
	TIM1->CR1|=(1<<0);
	while(!(TIM1->SR & TIM_SR_UIF));
}
void set_duty_channel1(uint16_t duty1)
{
	TIM1->CCR1=duty1;
}
void set_duty_channel3(uint16_t duty2)
{
	TIM1->CCR3=duty2;
}

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
void ADC_Config()
{
	RCC->APB2ENR|=(1<<2);//enable clock for GPIOA
	RCC->APB2ENR|=(1<<9);//enable clock fof ADC1
	GPIOA->CRL &=(0xFFFF0FFF);
	GPIOA->CRL |=(0<<3);// set GPIOA_PIN3 as ADC1_Channel 3
	ADC1->CR2 |= (1<<1);//turn on ADC continuous mode
	ADC1->CR2 |=(1<<0);//enable ADC
	ADC1->SMPR2|=(4<<9);//set up sample time
	ADC1->SQR1 |=(0<<20);//total number of channel : 1
	ADC1->SQR3 |=(3<<0); // set channel 3 in first conversation
	ADC1->CR2 |= (1 << 2); // turn calibrate on 
	while(!(ADC1->CR2 & (1 << 2)));//check calibrate
	ADC1->CR2|=(1<<0);//enable ADC1
	ADC1->CR2 |=(1<<22);//Perform an initial conversion (discarded)
}
uint16_t Read_ADC()
	{
		ADC1->CR2 |= ADC_CR2_SWSTART;
		while(!(ADC1->SR &(1<<1)));
		uint16_t value=ADC1->DR;
		return value;
		
		}
int main()
{
			ADC_Config();
	    TIM2_conf();
      USART1_Config();
	    Tim1_PWM_Config();
			while(1)
			{
				
				adc_value=Read_ADC();
                                USART1_Send_String((char *)str);
				duty = (adc_value)*999/4095;
				data_sent = duty/10;
				set_duty_channel1(duty);//set duty for red led
				set_duty_channel3(1000-duty); // set duty for yellow led
				int len1=sprintf(str,"%3d%3d",data_sent,100-data_sent);
				delay_ms(100);
			}
}