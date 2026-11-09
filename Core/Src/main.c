/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "sensor.h"
#include "ESP8266.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "sht4x_i2c.h"
#include "bh1750.h"
#include "CCS811.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t adc_acs712;
uint32_t adc_photo;
char msg[512];
float light;
float v_mq;
float current;
uint16_t adc_dma[3] = {0};
char buffer[256];
STATE state_dev;
uint8_t flag;
uint8_t flag1;
uint8_t flag2;

int16_t error = NO_ERROR;

int32_t temperature_milli_degC = 0;
int32_t humidity_milli_RH = 0;

float temperature = 0;
float humidity = 0;

float BH1750_lux;

CCS811_Data_t ccs811_data;
HAL_StatusTypeDef ccs811_status;
uint16_t tvoc;
uint16_t eco2;
void uart_print(uint8_t* message)
{
	HAL_UART_Transmit(&huart2, message,strlen(message), 100);
}
float ACS_Irms(void) {
  float sum = 0; const int N = 200;
  for (int i = 0; i < N; i++) {
    float v = adc_dma[2] * 3.3f / 4095 * K_DIV;  // điện áp thật tại OUT
    float a = (v - V_ZERO) / 0.185f;          // 185 mV/A (bản 5A), V_ZERO đo khi không tải
    sum += a * a; delay_us(500);
  }
  return sqrtf(sum / N);
}
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
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

sht4x_init(SHT45_I2C_ADDR_44);
sht4x_soft_reset();
sensirion_i2c_hal_sleep_usec(10000);

BH1750_Init(&hi2c1);
BH1750_SetMode(CONTINUOUS_HIGH_RES_MODE_2);

ccs811_status = CCS811_Init(&hi2c1);
//HAL_TIM_Base_Start(&htim3);
//uint16_t adc_dma[3] = {0};
//HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma, 3);
//
//ESP8266_init();
//ESP8266connect_Wifi(buffer, sizeof(buffer), "Connected", "12345678");
//MQTT_Sub("demo_f401_7392/home/cmd");
//uint32_t demgiay = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    HAL_Delay(2000);

	    error = sht4x_measure_high_precision(&temperature_milli_degC, &humidity_milli_RH);

	    if (error == NO_ERROR)
	    {
	        temperature = temperature_milli_degC / 1000.0f;

	        humidity = humidity_milli_RH / 1000.0f;


	        // Xu ly du lieu tai day
	    }
	    else
	    {
	    	_log("hello from stm32\r\n");
	        // Loi giao tiep I2C
	    }
	    BH1750_ReadLight(&BH1750_lux);

        snprintf(msg, sizeof(msg), "Temp: %0.2f\r\nhumi: %0.2f\r\nlight: %0.2f\r\neCO2: %d\r\ntVOC: %d\r\n",temperature,humidity,BH1750_lux,eco2,tvoc);
        _log(msg);
//if((uint32_t)(HAL_GetTick()- demgiay) > 10000)
//{
//	v_mq =  adc_dma[0] * 3.3f / 4095 * K_DIV;
//	light = 100.0f - adc_dma[1]*3.3/4095* K_DIV*100.0f/5;
//	current = ACS_Irms();
//	demgiay = HAL_GetTick();
//	MQTT_Pub("demo_f401_7392/home/sensor",msg);
//
//}
//	state_dev = Subcribe_event_get_data(&flag,&flag1,&flag2);
//	if(state_dev == LED0_ON && flag)
//	{
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12 , 1);
//		flag = 0;
//
//	}
//	else if(state_dev == LED0_OFF && flag)
//	{
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
//		flag = 0;
//	}
//	if(state_dev == LED1_ON && flag1)
//	{
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13 , 1);
//		flag1 = 0;
//	}
//	else if(state_dev == LED1_OFF && flag1)
//	{
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
//		flag1 = 0;
//	}
//	if(state_dev == FAN_ON && flag2)
//	{
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14 , 1);
//		flag2 = 0;
//	}
//	else if(state_dev == FAN_OFF && flag2)
//	{
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//		flag2 = 0;
//	}
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
