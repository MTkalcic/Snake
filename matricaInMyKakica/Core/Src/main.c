/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DECODE_MODE_REG     0x09
#define INTENSITY_REG       0x0A
#define SCAN_LIMIT_REG      0x0B
#define SHUTDOWN_REG        0x0C
#define DISPLAY_TEST_REG    0x0F
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t smjer = 0;
static uint16_t global_gpio_pin = 0;
uint8_t reset = 0;
uint8_t matrica[8][8] =
   {{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0}};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//slanje podataka na MAX7219 (matricu)
void mat_Init(uint8_t reg, uint8_t value){
	uint8_t tx_data[2] = { reg, value };
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, tx_data, 2, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

//crtanje na matricu (nebi nikad pogodio)
void crtanjeNaMatricu(uint8_t matrica[8][8]){
	uint8_t pomoc = 0b0;
	for(int i = 0; i <= 7; i++){
	  for(int j = 0; j <= 8; j++){
		  pomoc |= matrica[i][j] << j;
	  }
	  mat_Init(i+1, pomoc);
	  pomoc = 0;
	}
}

void clear(uint8_t matrica[8][8]) {
  for(int i = 0; i <= 7; i++){
	  for(int j = 0; j <= 8; j++){
		  //brisanje matrice
		  matrica[i][j] = 0;
	  }
  //brisanje na ledicama
  mat_Init(i + 1, 0x00);
  }
}

//inicijalizacija MAX7219 (matrice)
void max7219_init() {
  mat_Init(DISPLAY_TEST_REG, 0);
  mat_Init(SCAN_LIMIT_REG, 7);
  mat_Init(DECODE_MODE_REG, 0);
  mat_Init(SHUTDOWN_REG, 1);
  mat_Init(INTENSITY_REG, 1);
  clear(matrica);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	srand(((unsigned int)NULL));
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
  MX_SPI1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  max7219_init();


  uint8_t snakeHeadX[500];
  uint8_t snakeHeadY[500];
  uint16_t brojac = 0;
  uint8_t redak = 3;
  uint8_t stupac = 4;
  uint8_t applesAte = 0;
  uint8_t jabukaPostoji = 0;
  uint8_t jabukaX = 0;
  uint8_t jabukaY = 0;
  int vrijeme = 300;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  //she switch() on my case till I break;
	  switch(smjer){
	  case 0:
		  redak += 1;
		  if(redak >= 9)
			  redak = 1;
		  break;
	  case 1:
		  stupac -= 1;
		  if (stupac <= 0)
			  stupac = 8;
		  break;
	  case 2:
		  redak -= 1;
		  if(redak <= 0)
			  redak = 8;
		  break;
	  case 3:
		  stupac += 1;
		  if (stupac >= 9)
			  stupac = 1;
		  break;
	  }

	  //stvaranje jabuke
	  if (jabukaPostoji == 0){
		  jabukaX = rand() % 8;
		  jabukaY = rand() % 8;
		  jabukaPostoji = 1;
	  }

	  matrica[jabukaX][jabukaY] = 1;

	  snakeHeadX[brojac] = redak-1;
	  snakeHeadY[brojac] = stupac-1;

	  if(reset == 1){
		  applesAte = 0;
		  brojac = 0;
		  redak = 3;
		  stupac = 4;
		  smjer = 0;
		  reset = 0;
		  vrijeme = 300;
	  }

	  //an apple a day keeps the doctor away :)
	  if(snakeHeadX[brojac]== jabukaX && snakeHeadY[brojac]== jabukaY){
		  applesAte += 1;
		  jabukaPostoji = 0;
	  }

	  //ucrtavanje zmije u virtualnu matricu i provjera sudara
	  for(int i = applesAte; i >= 0; i--){
		  matrica[snakeHeadX[brojac-i]][snakeHeadY[brojac-i]] = 1;
		  if (i != 0){
			  if(snakeHeadX[brojac] == snakeHeadX[brojac-i] && snakeHeadY[brojac] == snakeHeadY[brojac-i]){
				  applesAte = 0;
				  brojac = 0;
				  redak = 3;
				  stupac = 4;
				  smjer = 0;
				  vrijeme = 300;
			  }
		  }
	  }


	  //logika levela
	  brojac += 1;
	  if(brojac >= 499){
		  brojac = 0;
		  vrijeme -= 50;
		  applesAte = 0;
	  }


	  crtanjeNaMatricu(matrica);
	  HAL_Delay(vrijeme);
	  clear(matrica);

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	global_gpio_pin = GPIO_Pin;
	 __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
	 HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	 if (htim->Instance == TIM7){
		 if (HAL_GPIO_ReadPin(GPIOB, global_gpio_pin) == GPIO_PIN_RESET){
			 if (global_gpio_pin == GPIO_PIN_10){
				 if(smjer == 1 || smjer == 3)
					 smjer = 0;
			 }
			 if (global_gpio_pin == GPIO_PIN_8){
				 if(smjer == 0 || smjer == 2)
					 smjer = 1;
			 }
		 }
		 if (HAL_GPIO_ReadPin(GPIOA, global_gpio_pin) == GPIO_PIN_RESET){
			 if (global_gpio_pin == GPIO_PIN_9){
				 if(smjer == 1 || smjer == 3)
					 smjer = 2;
			 }
		 }
		 if (HAL_GPIO_ReadPin(GPIOC, global_gpio_pin) == GPIO_PIN_RESET){
			 if (global_gpio_pin == GPIO_PIN_7){
				 if(smjer == 2 || smjer == 0)
					 smjer = 3;
			 }
			 if (global_gpio_pin == GPIO_PIN_6){
				  reset = 1;
			 }
		 }
		 HAL_TIM_Base_Stop_IT(&htim7);
	 }
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
