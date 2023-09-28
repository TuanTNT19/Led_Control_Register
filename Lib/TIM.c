#include "TIM.h"
#include "stm32f1xx.h"

void Tim1_PWM_Config()
{
	RCC->APB2ENR |=(1<<2);//enable clock for GPIOA
	GPIOA->CRH &=(0xFFFFFFF0);
	GPIOA->CRH |=(10<<0); //set gpioa pin 8 as output alternate function push pull for timer 1 channel 1
//	GPIOA->CRH &=(0xFFFFF0FF);
//	GPIOA->CRH |=(10<<8);//set gpioa pin 10 as output alternate function push pull for timer 1 channel 3
	RCC->APB2ENR |=(1<<11);//tim 1 timer clock enable
	TIM1->PSC=7;
	TIM1->ARR=999;
	TIM1->CCER|=((1<<0));//enable caption / compare output for channel 1 vs 3
	TIM1->CCMR1 |=(6<<4);//set mode 1 for channel 1 
//	TIM1->CCMR2 |=(6<<4);//set mode 1 for channel 3
	TIM1->CCR1=0;
//	TIM1->CCR3=0;
	TIM1->BDTR |= TIM_BDTR_MOE;// Enable the main output
	TIM1->CR1|=(1<<0);
	while(!(TIM1->SR & TIM_SR_UIF));
}
void set_duty_channel1(uint16_t duty1)
{
	TIM1->CCR1=duty1;
}
//void set_duty_channel3(uint16_t duty2)
//{
//	TIM1->CCR3=duty2;
//}