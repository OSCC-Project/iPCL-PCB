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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "imu.h"
#include "gear_motor.h"
#include "servo.h"
#include "motion.h"
#include "nrf24l01.h"
#include <string.h>
#include <stdlib.h>
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define M_PI 3.1415


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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM9_Init();
  MX_TIM12_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM14_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  Servo_Init();

  Serial_Printf("Servo_Init");
  Motor_Wheels_Init();
  Serial_Printf("Motor_Wheels_Init");

  HAL_UART_Receive_IT(&huart1, &ByteRecv, 1);  // 启动串口接收中断，接收1个字节

  //imu_init();


  //NRF24L01_Init();


//  Serial_Printf("NRF24L01_Check:%d\r\n",NRF24L01_Check());
//  NRF24L01_RX_Mode(0);
//  uint8_t rx_buf[8];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


//	  Serial_Printf("ping\r\n");
//	  HAL_Delay(1000);

	  if (Serial_GetRxFlag() == 1){


		int8_t data1 = (int8_t)Serial_RxPacket[0];
		int8_t data2 = (int8_t)Serial_RxPacket[1];
		int8_t data3 = (int8_t)Serial_RxPacket[2];

			  // 处理数据
			  // 例如，打印数据或者将数据用于其他操作
		//上面叫做X，Y，Z，但对于下面是Y，X，Z
		 Serial_Printf("stmReceived data: %d, %d, %d\r\n", data1, data2, data3);

		 //换成下面的X，Y，Z，
		 MotionInput input = {data2, data1, data3};


		 MotionOutput output;
		 MOTION_Inverse_Resolve(&input, &output);

		Motor_Wheels_SetPWMs(output.vlf, output.vrf, output.vlr, output.vrr);
		Servo_SetAngles((uint8_t)(output.poslf+90 ),
					(uint8_t)(output.posrf +90),
						   (uint8_t)(output.poslr+80),
						   (uint8_t)(output.posrr+94));
		//printMotionOutput(&output);
	  }
//	  HAL_Delay(100);
	  //Serial_Printf("main");
//	  if(NRF24L01_RxPacket(rx_buf)==0X00)  //NRF24L01模块发�?�数据并判断是否发�?�成�?
//	  {
//
//		  Serial_Printf("\r\nrx_buf:%d\r\n",rx_buf[0]);
//	  }

//	  Motor_Wheels_SetPWMs(100-1,100-1,100-1,100-1);
//	  Serial_Printf("P100\r\n");
	  //Servo_SetAngles(180,180,180,180);
	  //Serial_Printf("180\r\n");
	  //HAL_Delay(5000);
//	  Motor_Wheels_SetPWMs(-100,-100,-100,-100);
//	  Serial_Printf("P-100\r\n");
	  //Servo_SetAngles(0,0,0,0);
	  //Serial_Printf("0\r\n");
	  //HAL_Delay(5000);
//	  Motor_Wheels_SetPWMs(0,0,0,0);
//	  Serial_Printf("P-100\r\n");
	  //Servo_SetAngles(90,90,90,90);
	  //Serial_Printf("90\r\n");
	  //HAL_Delay(5000);




//	  uint8_t i=0;
//	  for(i=0;i<180;i++){
//	  HAL_Delay(100);
//	  Serial_Printf("i:%d	,",i);
//	  Servo_SetAngles(i,i,i,i);
//	  }
//	  Motor_Wheels_SetPWMs(100-1,100-1,100-1,100-1);
//	  HAL_Delay(5000);
//	  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
//	  HAL_TIM_PWM_Stop(&htim9, TIM_CHANNEL_2);
//	  __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 99);
//	  HAL_Delay(5000);
//	  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
//	  HAL_TIM_PWM_Stop(&htim9, TIM_CHANNEL_1);
//	  __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, 99);
//	  HAL_Delay(5000);
//	  int i=0;
//	  for(i=0;i<100;i++){
//
//	  Motor_Wheels_SetPWMs(i-1,i-1,i-1,i-1);
//	  HAL_Delay(1000);
//	  }
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
  RCC_OscInitStruct.PLL.PLLN = 84;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	IMU_Schedule(htim);
	Motor_Schedule(htim);
}










//HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	if (huart == &huart1){
//	static uint8_t RxState = 0;
//	static uint8_t pRxPacket = 0;
//	uint8_t RxData=ByteRecv;
//	if (RxState == 0)
//	{
//	if (RxData == 0xFF)
//	{
//		RxState = 1;
//		pRxPacket = 0;
//	}
//	}
//	else if (RxState == 1)
//	{
//		Serial_RxPacket[pRxPacket] = RxData;
//		pRxPacket ++;
//	if (pRxPacket >= 3)
//	{
//		RxState = 2;
//	}
//	}
//	else if (RxState == 2)
//	{
//	if (RxData == 0xFE)
//	{
//		RxState = 0;
//		Serial_RxFlag = 1;
//	}
//	}
//	HAL_UART_Receive_IT(&huart1, &ByteRecv, 1);
//	}
//	}

//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == USART1) {
//        int last_crlf_pos = -1;
//
//        // 从后往前查找最后一个完整的\r\n
//        for (int i = rx_index - 2; i >= 0; i--) {
//            if (rx_buffer[i] == '\r' && rx_buffer[i+1] == '\n') {
//                last_crlf_pos = i;
//                break;
//            }
//        }
//
//        // 如果找到完整帧
//        if (last_crlf_pos != -1) {
//            // 提取最新帧（最后一个CRLF之后的数据）
//            char *frame_start = rx_buffer + last_crlf_pos + 2;
//            int frame_length = rx_index - (last_crlf_pos + 2);
//
//            // 临时拷贝到独立缓冲区防止数据覆盖
//            char latest_frame[64] = {0};
//            strncpy(latest_frame, frame_start, frame_length);
//
//            // 立即清理已处理数据（保留最后一个CRLF之后的不完整数据）
//            memmove(rx_buffer, frame_start, frame_length);
//            rx_index = frame_length;
//
//            // 解析最新帧
//            char *move_cmd = strtok(latest_frame, ",");
//            char *x_str = strtok(NULL, ",");
//            char *y_str = strtok(NULL, ",");
//            char *z_str = strtok(NULL, ",");
//
//            if (move_cmd && x_str && y_str && z_str) {
//                float x = atof(x_str);
//                float y = atof(y_str);
//                float z = atof(z_str);
//
//                // 立即执行最新指令
//                MotionInput input = {x, y, z};
//                MotionOutput output;
//                MOTION_Inverse_Resolve(&input, &output);
//
//				Motor_Wheels_SetPWMs(output.vlf, output.vrf, output.vlr, output.vrr);
//				Servo_SetAngles((uint8_t)(output.poslf * 180 / M_PI),
//								(uint8_t)(output.posrf * 180 / M_PI),
//                                    (uint8_t)(output.poslr * 180 / M_PI),
//                                    (uint8_t)(output.posrr * 180 / M_PI));
//
//            }
//        }
//
//        // 缓冲区溢出保护（保留最后128字节）
//        if (rx_index >= RX_BUFFER_SIZE) {
//            int keep_bytes = RX_BUFFER_SIZE / 4;  // 保留最后1/4数据
//            memmove(rx_buffer, rx_buffer + (RX_BUFFER_SIZE - keep_bytes), keep_bytes);
//            rx_index = keep_bytes;
//            Serial_Printf("[WARN] Buffer overflow, truncating!\r\n");
//        }
//
//        // 重新启动接收（总是从当前索引开始）
//        HAL_UART_Receive_IT(&huart1, rx_buffer + rx_index, RX_BUFFER_SIZE - rx_index);
//    }
//}

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
