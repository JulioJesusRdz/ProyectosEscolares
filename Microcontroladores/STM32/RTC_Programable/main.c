#include "main.h"
RTC_HandleTypeDef hrtc;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);

void displayNumber(int valor);
void setDisplay(int dig1, int dig2, int dig3, int dig4, int dig5, int dig6, int dig7, int dig8);
int numeros[10]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};

int D1=0x70;
int D2=0x60;
int D3=0x50;
int D4=0x40;
int D5=0x30;
int D6=0x20;
int D7=0x10;
int D8=0x00;

int seg, min, hr, hrs;
RTC_TimeTypeDef sTime1;
RTC_DateTypeDef sDate1;

int tecla =0;
int teclado=0;

int modo_programacion = 0;

int GetTime(void)
{
	  HAL_RTC_GetDate(&hrtc, &sDate1,RTC_FORMAT_BIN);
	  HAL_RTC_GetTime(&hrtc, &sTime1,RTC_FORMAT_BIN);

	  seg=sTime1.Seconds;
	  min=sTime1.Minutes;
	  hr=sTime1.Hours;

	  hr=hr*1000000;
	  min=min*1000;
	  hrs=hr+min+seg;

	  return hrs;
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_RTC_Init();
  GPIOA->ODR=0xF0;
  while (1)
  {
	  if(modo_programacion==0)
	  {
		  GetTime();
		  displayNumber(hrs);
	  }else if(modo_programacion==1)
	  {
		  displayNumber(teclado);
	  }
  }
}


void displayNumber(int valor)
{
	int dig1,dig2,dig3,dig4,dig5,dig6,dig7,dig8;

	dig8=valor%10; //Unidades (8)
	dig7=(valor%100)/10; //Decenas (7)
	dig6=(valor%1000)/100; //Centenas (6)
	dig5=(valor%10000)/1000; //millares almacenar el (5)
	dig4=(valor%100000)/10000;//Decenas de millares (4)
	dig3=(valor%1000000)/100000;//Centenas de millares (3)
	dig2=(valor%10000000)/1000000; //Millones		(2)
	dig1=(valor%100000000)/10000000;//Decenas de millones (1)

	setDisplay(dig1,dig2,dig3,dig4,dig5,dig6,dig7,dig8);
}

void setDisplay(int dig1, int dig2, int dig3, int dig4, int dig5,int dig6,int dig7,int dig8)
{

	GPIOC->ODR=D8+numeros[dig8];// Unidades
	HAL_Delay(1);
	GPIOC->ODR=D7+numeros[dig7];// Decenas
	HAL_Delay(1);
    //GPIOC->ODR=D6+numeros[dig6];
	//HAL_Delay(1);
	GPIOC->ODR=D5+numeros[dig5];
	HAL_Delay(1);
	GPIOC->ODR=D4+numeros[dig4];
	HAL_Delay(1);
	//GPIOC->ODR=D3+numeros[dig3];
	//HAL_Delay(1);
	GPIOC->ODR=D2+numeros[dig2];
	HAL_Delay(1);
	GPIOC->ODR=D1+numeros[dig1]; //Decenas de Millones
	HAL_Delay(1);

}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	uint32_t time_actual;
	    static uint32_t time_antes = 0;
    time_actual = HAL_GetTick();
    if (time_actual - time_antes < 50)
    {
        return;
    }
    time_antes = time_actual;
	GPIOA->ODR=0x00;

	switch(GPIO_Pin)

	{
	case GPIO_PIN_0:

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==1){
			tecla = 1;
		teclado=(teclado%100000000)*10 + tecla;

		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==1){
			tecla = 2;
			teclado=(teclado%100000000)*10 + tecla;
		}

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==1){
			tecla = 3;
			teclado=(teclado%100000000)*10 + tecla;
		}

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==1){
			hr=0;
			if(teclado>=0 && teclado<=23)
			{
				hr = teclado;
				sTime1.Hours = hr;
				HAL_RTC_SetTime(&hrtc, &sTime1, RTC_FORMAT_BIN);
				teclado = 0;
			}
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET);
		break;

	case GPIO_PIN_1:

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==1){
			tecla = 4;
			teclado=(teclado%100000000)*10 + tecla;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==1){
			tecla = 5;
			teclado=(teclado%100000000)*10 + tecla;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==1){
			tecla = 6;
			teclado=(teclado%100000000)*10 + tecla;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==1){
			min=0;
			if(teclado<=59)
			{
				min = teclado;
				sTime1.Minutes = min;
				HAL_RTC_SetTime(&hrtc, &sTime1, RTC_FORMAT_BIN);
				teclado=0;
			}
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET);
		break;

	case GPIO_PIN_2:

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==1){
			tecla = 7;
			teclado=(teclado%100000000)*10 + tecla;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==1){
			tecla = 8;
			teclado=(teclado%100000000)*10 + tecla;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==1){
			tecla = 9;
			teclado=(teclado%100000000)*10 + tecla;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==1){
			seg=0;
			if(teclado<=59)
			{
				seg = teclado;
				sTime1.Seconds = seg;
				HAL_RTC_SetTime(&hrtc, &sTime1, RTC_FORMAT_BIN);
				teclado=0;
			}
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET);
		break;

	case GPIO_PIN_3:

	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET);
	  	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1){
		  modo_programacion = 1;
		  HAL_RTC_SetTime(&hrtc, &sTime1, RTC_FORMAT_BIN);
	  	}
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, RESET);
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
	  	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1){
	  	  tecla = 0;
	  	  teclado=(teclado%100000000)*10 + tecla;
	  	}
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET);
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
	  	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1){
		  	  modo_programacion = 0;
		  	  teclado=0;
	  	}
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, RESET);
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET);
	  	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1){
	  	  teclado=0;
	  	}
	  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET);
		break;

		default:
		break;
	}
	GPIOA->ODR=0xF0;
}













void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV4;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x5;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|User_LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 PC6 User_LED2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|User_LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
