

#include "stdio.h"
#include "main.h"
#include "cmsis_os.h"

osThreadId defaultTaskHandle;
TaskHandle_t AboveNormalHandle;
TaskHandle_t NormalTaskHandle;
QueueHandle_t QueuexHandle;
QueueHandle_t QueueyHandle;
void StartDefaultTask(void const * argument);
void AboveNormalTask (void *parameter);
void NormalTask(void *para);
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

uint16_t adc_value[2];
uint16_t data_receive[2];
char str[10];
uint16_t duty;

int main(void)
{
  ADC1_Config();
	DMA_Init();
	USART1_Config();
	Tim1_PWM_Config();
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
  xTaskCreate(AboveNormalTask, "Task01", 128, NULL,3, &AboveNormalHandle);
	xTaskCreate(NormalTask,"Task02",128,NULL,2,&NormalTaskHandle);
  QueuexHandle = xQueueCreate(1,2);
	QueueyHandle = xQueueCreate(1,2);
  osKernelStart();


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


void AboveNormalTask (void *parameter){
	
	while(1)
	{

	xQueueReceive(QueuexHandle,&data_receive[0],osWaitForever);
		if (data_receive[0] <= 10)
		{
			  GPIOC->ODR |= (1<<14);
			USART1_Send_String("1");
		}
		else if (data_receive[0] > 10)
		{
			  GPIOC->ODR &=~ (1<<14);
			 USART1_Send_String("0");
		}
	}
}
void NormalTask(void *para)
{
	while(1)
	{
		xQueueReceive(QueueyHandle,&data_receive[1],osWaitForever);
		 duty = data_receive[1]*999/4095;
      set_duty_channel1(duty);
     int len1=sprintf(str,"%3d",duty/10);
		 USART1_Send_String((char *)str);		
	}
	
}
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	DMA1_Config((uint32_t)&ADC1->DR, (uint32_t)adc_value, 2);
  for(;;)
  {
     

		xQueueSend(QueuexHandle,&adc_value[0],NULL);
		xQueueSend(QueueyHandle,&adc_value[1],NULL);

		vTaskDelay(50);
  }
  /* USER CODE END 5 */
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}