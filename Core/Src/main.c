/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "DHT11lib.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ESP8266_STM32.h"
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
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t Rh_byte1=0,Rh_byte2=0,Temp_byte1=0,Temp_byte2=0,SUM=0;
uint16_t RH,TEMP;
uint8_t DATA[5]={0};
float temperatura=0,wilgotnosc=0;
char Autor[]="Aleks Kusztelak";
char ip_buf[16];
int first_on = 1;
char tempStr[16];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void delay_us(uint16_t us)
{

    __HAL_TIM_SET_COUNTER(&htim6,0);
    while ((uint16_t)(__HAL_TIM_GET_COUNTER(&htim6) < us));


}

void DHT11_Init(void)
{
	DHT11_SetPinOutput (DHT11_PORT, DHT11_PIN);  //output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);   //pin na low
	delay_us (20000);   // czekam 20ms
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
	delay_us(30);
	DHT11_SetPinInput(DHT11_PORT, DHT11_PIN);    //pin na input
}

void DHT11_Resp(void){
	uint8_t test = 0;
	delay_us(40);
	if (!(HAL_GPIO_ReadPin(DHT11_PORT,DHT11_PIN)))
	{
		delay_us(80);
		if ((HAL_GPIO_ReadPin(DHT11_PORT,DHT11_PIN))) {
			test =1;
		}
		else {
			test = -1;
		}

	}
}

/*uint8_t DHT11_ReadData(void){
	uint8_t i=0;
	for (uint8_t j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		delay_us(40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}
*/
void DHT11_ReadData(uint8_t *dane){ //Zczytujemy dane do tablicy ktora przekazujemy do tej funkcji
	for(uint8_t i=0;i<5;i++){
		for (uint8_t j=0;j<8;j++)
		{
			while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // czekamy az pin high
			delay_us(40);   // czekamy 40 us
			if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // jesli pin low
			{
				dane[i]&= ~(1<<(7-j));   // 0
			}
			else dane[i]|= (1<<(7-j));  // jesli pin high, 1
			while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));  // czekamy az pin low
		}
	}
}


void Oled_Temp(float temperatura){
	char string[20] ={0};
	sprintf(string,"Temp: %.2f",temperatura);
	ssd1306_SetCursor(5,5);
	ssd1306_WriteString(string,Font_7x10,White);
	ssd1306_UpdateScreen();
}

void Oled_Wilg(float wilgotnosc){
	char string[20] ={0};
	sprintf(string,"RH: %.2f",wilgotnosc);
	ssd1306_SetCursor(5,20);
	ssd1306_WriteString(string,Font_7x10,White);
	ssd1306_UpdateScreen();
}

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 100);
	return len;
}
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
  MX_TIM6_Init();
  MX_I2C3_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  if (ESP_Init() != ESP8266_OK){
	  USER_LOG("Failed to initialize... Check Debug logs");
	  Error_Handler();
  }

  if (ESP_ConnectWiFi("XYZ", "XYZ", ip_buf, sizeof(ip_buf)) != ESP8266_OK){
	  USER_LOG("Failed to connect to wifi... Check Debug logs");
	  Error_Handler();
  }

  if (ESP_MQTT_Connect("broker.hivemq.com", 1883, "STM32Client", NULL, NULL, 60) != ESP8266_OK){
	  Error_Handler();
  }
  HAL_Delay(30000);
  HAL_TIM_Base_Start(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  ssd1306_Init();
	  ssd1306_SetCursor(5,35);
	  ssd1306_WriteString(Autor,Font_7x10,White);
	  ssd1306_UpdateScreen();
	  DHT11_Init();
	  DHT11_Resp();
	 /* Rh_byte1=DHT11_ReadData();
	  Rh_byte2=DHT11_ReadData();
	  Temp_byte1=DHT11_ReadData();
	  Temp_byte2=DHT11_ReadData();
	  SUM=DHT11_ReadData();
*/

	  DHT11_ReadData(DATA);
	  uint8_t suma = 0;


	  suma = DATA[0] + DATA[1] +DATA[2] +DATA[3];
	 // temperatura = (float) Temp_byte1;
	 // wilgotnosc = (float) Rh_byte1;
	  wilgotnosc = (float) DATA[0];
	  temperatura = (float) DATA[2];
	  HAL_Delay(3000); //Delay zeby czujnik nie zwariowal
	 /* if(suma==DATA[4]){ //poprawne CHECKSUM
		  if(first_on) {
			  if (ESP_Init() != ESP8266_OK){
				  USER_LOG("Failed to initialize... Check Debug logs");
				  Error_Handler();
			  }

			  if (ESP_ConnectWiFi("XYZ", "XYZ", ip_buf, sizeof(ip_buf)) != ESP8266_OK){
				  USER_LOG("Failed to connect to wifi... Check Debug logs");
				  Error_Handler();
			  }

			  if (ESP_MQTT_Connect("Broker", 1883, "STM32Client", NULL, NULL, 60) != ESP8266_OK){
				  Error_Handler();
			  }
			  first_on=0;
			  HAL_Delay(30000);
		  } */

	  ssd1306_Init();
	  Oled_Temp(temperatura); //odczyt temperatury
	  Oled_Wilg(wilgotnosc); //odczyt wilgotnosci
	  ssd1306_SetCursor(5,35);
	  ssd1306_WriteString(Autor,Font_7x10,White);
	  ssd1306_UpdateScreen();
	  HAL_Delay(3000); //delay zeby czujnik nie zwariowal
	 // static char bufmg[32];
	 // snprintf(bufmg, sizeof(bufmg), "T=%d°C  H=%d%%", DATA[2], DATA[0]);
	  snprintf(tempStr, sizeof(tempStr), "T:%d H:%d", DATA[2],DATA[0]);
	 // const char *tekst = bufmg;
	  if (ESP_CheckTCPConnection() == ESP8266_OK){
		  ESP_MQTT_Publish("Pomiar/wynik", tempStr, 0);
	  }
	 // }

	  HAL_Delay(3000);



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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00201D2B;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 72-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
