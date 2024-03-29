/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <math.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char ADatabuffer[32] = {0};
char BDatabuffer[32] = {0};
char Menu[]="Sawtooth[1]\r\nSine wave[2]\r\nSquare wave[3]\r\n";
char Menusawtooth[]="Frequency Up[+]\r\nFrequency Down[-]\r\nVoltage High[h]\r\nVoltage Low[l]\r\nSlope Up[u]\r\nSlope Down[d]\r\nExit[x]\r\n";
char Menusinwave[]="Frequency Up[+]\r\nFrequency Down[-]\r\nVoltage High[h]\r\nVoltage Low[l]\r\nExit[x]\r\n";
char Menusquarewave[]="Frequency Up[+]\r\nFrequency Down[-]\r\nVoltage High[h]\r\nVoltage Low[l]\r\nDuty cycle[p-m]\r\nExit[x]\r\n";
char Showfrequency[15]="";
char Showdutycycle[15]="";
char Showvolhigh[15]="";
char Showvollow[15]="";
char Showslopeup[]="Slope Up\r\n";
char Showslopedown[]="Slope Down\r\n";
int Wave = 0;
float Frequency;
float Voltage;
float Volhigh = 3.3;
float Vollow = 0;
float angle;
int Slope = 1;
float Duty_cycle = 50;
int16_t inputchar = 0;
uint16_t ADCin = 0;
uint64_t _micro = 0;
uint16_t dataOut = 0;
uint8_t DACConfig = 0b0011;
uint8_t State_Now = 0;
float TimeNow;
float Period ;
float Amplitude;
enum State
{
	State_Start = 0000,
	State_Menu = 0001,
	State_Menu_Sawtooth = 0010,
	State_Menu_SinWave = 0011,
	State_Menu_Squarewave = 0100,
	State_Choose_Work = 0101,

};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM11_Init(void);
/* USER CODE BEGIN PFP */
void MCP4922SetOutput(uint8_t Config, uint16_t DACOutput);
uint64_t micros();
int16_t UARTRecieve();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_SPI3_Init();
  MX_TIM3_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &ADCin, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		static uint64_t timestamp = 0;
		static uint64_t timewave = 0;
		HAL_UART_Receive_IT(&huart2, (u_int8_t*)BDatabuffer, 32);
		inputchar = UARTRecieve();
		if(inputchar > 0)
		{
			 sprintf(ADatabuffer,"%c\r\n",inputchar);
			 HAL_UART_Transmit(&huart2, (u_int8_t*)ADatabuffer, strlen(ADatabuffer),10);
		}
		switch (State_Now)
		{
			case State_Start:
				Frequency = 0;
				Voltage = 0;
				Volhigh = 3.3;
				Vollow = 0;
				Wave = 0;
				Slope = 1;
				Duty_cycle = 50;
				HAL_UART_Transmit(&huart2, (u_int8_t*)Menu, strlen(Menu),10);
				State_Now = State_Menu;
				break;
			case State_Menu:
				if(inputchar == '1')
				{
					Frequency = 0.1;
					sprintf(Showfrequency,"Frequency = %.1f Hz\r\n",Frequency);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showfrequency, strlen(Showfrequency),10);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showslopeup, strlen(Showslopeup),10);
					State_Now = State_Menu_Sawtooth;
				}
				if(inputchar == '2')
				{
					Frequency = 0.1;
					sprintf(Showfrequency,"Frequency = %.1f Hz\r\n",Frequency);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showfrequency, strlen(Showfrequency),10);
					State_Now = State_Menu_SinWave;
				}
				if(inputchar == '3')
				{
					Frequency = 0.1;
					sprintf(Showfrequency,"Frequency = %.1f Hz\r\n",Frequency);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showfrequency, strlen(Showfrequency),10);
					sprintf(Showdutycycle,"Duty Cycle = %d\r\n",Duty_cycle);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showdutycycle, strlen(Showdutycycle),10);
					State_Now = State_Menu_Squarewave;
				}
				break;
			case State_Menu_Sawtooth:
				Wave = 1;
				HAL_UART_Transmit(&huart2, (u_int8_t*) Menusawtooth, strlen( Menusawtooth),10);
				State_Now = State_Choose_Work;
				break;
			case State_Menu_SinWave:
				Wave = 2;
				HAL_UART_Transmit(&huart2, (u_int8_t*) Menusinwave, strlen( Menusinwave),10);
				State_Now = State_Choose_Work;
				break;
			case State_Menu_Squarewave:
				Wave = 3;
				HAL_UART_Transmit(&huart2, (u_int8_t*) Menusquarewave, strlen( Menusquarewave),10);
				State_Now = State_Choose_Work;
				break;
			case State_Choose_Work:
				if(inputchar == '+')
				{
					Frequency += 0.1;
					if(Frequency > 10)
					{
						Frequency = 10;
					}
					 sprintf(Showfrequency,"Frequency = %.1f Hz\r\n",Frequency);
					 HAL_UART_Transmit(&huart2, (u_int8_t*)Showfrequency, strlen(Showfrequency),10);
					 State_Now = State_Choose_Work;
				}
				if(inputchar == '-')
				{
					Frequency -= 0.1;
					if(Frequency < 0.1)
					{
						Frequency = 0;
					}
					sprintf(Showfrequency,"Frequency = %.1f Hz\r\n",Frequency);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showfrequency, strlen(Showfrequency),10);
				}
				if(inputchar == 'h')
				{
					Volhigh += 0.1;
					if(Volhigh > 10)
					{
						Volhigh = 10;
					}
					sprintf(Showvolhigh,"Vol_high = %.1f V\r\n",Volhigh);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showvolhigh, strlen(Showvolhigh),10);
				}
				if(inputchar == 'j')
				{
					Volhigh -= 0.1;
					if(Volhigh < 0)
					{
						Volhigh = 0;
					}
					sprintf(Showvolhigh,"Vol_high = %.1f V\r\n",Volhigh);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showvolhigh, strlen(Showvolhigh),10);
				}
				if(inputchar == 'l')
				{
					Vollow += 0.1;
					if(Vollow > 10)
					{
						Vollow = 10;
					}
						sprintf(Showvollow,"Vol_low = %.1f V\r\n",Vollow);
						HAL_UART_Transmit(&huart2, (u_int8_t*)Showvollow, strlen(Showvollow),10);
				}
				if(inputchar == 'k')
				{
					Vollow -= 0.1;
					if(Vollow < 0)
					{
						Vollow = 0;
					}
					sprintf(Showvollow,"Vol_low = %.1f V\r\n",Vollow);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showvollow, strlen(Showvollow),10);
				}
				if(inputchar == 'u')
				{
					Slope = 1;
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showslopeup, strlen(Showslopeup),10);
				}
				if(inputchar == 'd')
				{
					Slope = 0;
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showslopedown, strlen(Showslopedown),10);
				}
				if(inputchar == 'p')
				{
					Duty_cycle += 10;
					sprintf(Showdutycycle,"Duty Cycle = %d %\r\n",Duty_cycle);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showdutycycle, strlen(Showdutycycle),10);
				}
				if(inputchar == 'm')
				{
					Duty_cycle -= 10;
					sprintf(Showdutycycle,"Duty Cycle = %d %\r\n",Duty_cycle);
					HAL_UART_Transmit(&huart2, (u_int8_t*)Showdutycycle, strlen(Showdutycycle),10);
				}
				if(inputchar == 'x')
				{
					State_Now = State_Start;
				}
				break;
		}
		if (micros() - timestamp > 100)
		{
			timestamp = micros();
			TimeNow = micros()-timewave;
			Period = 1000000/Frequency;
			Amplitude = Volhigh - Vollow;
			if(Wave == 1)
			{
				if(Slope == 1)
				{
					if(TimeNow <= Period)
					{
						Voltage =  Vollow + (Amplitude * (TimeNow / Period));
					}
					else if (TimeNow > Period)
					{
						timewave = micros();
					}
				}
				else if(Slope == 0)
				{
					if(TimeNow <= Period)
					{
						Voltage = Volhigh - (Amplitude * (TimeNow / Period) );
					}
					else if (TimeNow > Period)
					{
						timewave = micros();
					}
				}
			}
			if(Wave == 2)
			{
				if(TimeNow <= Period)
				{
					Voltage = Vollow + (Amplitude/2 * (sin((2*3.14/Period)*TimeNow) + 1));
				}
				else if(TimeNow > Period)
				{
					timewave = micros();
				}
			}
			if(Wave == 3)
			{
				if(TimeNow <= Period)
				{
					if(TimeNow <= (Period) * (Duty_cycle/100))
					{
						Voltage = Volhigh;
					}
					else if (TimeNow > (Period) * (Duty_cycle/100))
					{
						Voltage = Vollow;
					}
				}
				else if(TimeNow > Period)
				{
					timewave = micros();
				}
			}
			dataOut = (Voltage*4096)/3.3;
			if (hspi3.State == HAL_SPI_STATE_READY && HAL_GPIO_ReadPin(SPI_SS_GPIO_Port, SPI_SS_Pin) == 1)
			{
				MCP4922SetOutput(DACConfig, dataOut);
			}
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 99;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 99;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 65535;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LOAD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LOAD_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_SS_Pin */
  GPIO_InitStruct.Pin = SPI_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI_SS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SHDN_Pin */
  GPIO_InitStruct.Pin = SHDN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHDN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void MCP4922SetOutput(uint8_t Config, uint16_t DACOutput)
{
	uint32_t OutputPacket = (DACOutput & 0x0fff) | ((Config & 0xf) << 12);
	HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, 0);
	HAL_SPI_Transmit_IT(&hspi3, &OutputPacket, 1);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi3)
	{
		HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin,1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim11)
	{
		_micro += 65535;
	}
}
inline uint64_t micros()
{
	return htim11.Instance->CNT + _micro;
}
int16_t UARTRecieve()
{
	static uint32_t DataPosition = 0;
	int16_t Data = -1;
	if(huart2.RxXferSize - huart2.RxXferCount != DataPosition)
	{
		Data = BDatabuffer[DataPosition];
		DataPosition = (DataPosition + 1) % huart2.RxXferSize;
	}
	return Data;

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	sprintf(ADatabuffer, "%s\r\n", BDatabuffer);
	HAL_UART_Transmit(&huart2, (uint8_t*)ADatabuffer, strlen(ADatabuffer), 1000);
}

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
