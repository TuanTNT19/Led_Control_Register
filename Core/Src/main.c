#include "stdio.h"
#include "main.h"
#include "cmsis_os.h"
#include "ADC.h"
#include "DMA.h"
#include "USART.h"
#include "TIM.h"

osThreadId defaultTaskHandle;
TaskHandle_t AboveNormalHandle;
TaskHandle_t NormalTaskHandle;
QueueHandle_t QueuexHandle;
QueueHandle_t QueueyHandle;
void StartDefaultTask(void const * argument);
void AboveNormalTask (void *parameter);
void NormalTask(void *para);
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