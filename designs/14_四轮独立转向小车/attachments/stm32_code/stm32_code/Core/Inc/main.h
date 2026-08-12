/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define M2A_Pin GPIO_PIN_5
#define M2A_GPIO_Port GPIOE
#define M2B_Pin GPIO_PIN_6
#define M2B_GPIO_Port GPIOE
#define M3A_Pin GPIO_PIN_0
#define M3A_GPIO_Port GPIOA
#define M3B_Pin GPIO_PIN_1
#define M3B_GPIO_Port GPIOA
#define M4A_Pin GPIO_PIN_2
#define M4A_GPIO_Port GPIOA
#define M4B_Pin GPIO_PIN_3
#define M4B_GPIO_Port GPIOA
#define B2A_Pin GPIO_PIN_6
#define B2A_GPIO_Port GPIOA
#define B2B_Pin GPIO_PIN_7
#define B2B_GPIO_Port GPIOA
#define B1A_Pin GPIO_PIN_9
#define B1A_GPIO_Port GPIOE
#define B1B_Pin GPIO_PIN_11
#define B1B_GPIO_Port GPIOE
#define D3_Pin GPIO_PIN_10
#define D3_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_11
#define D4_GPIO_Port GPIOB
#define CS2_Pin GPIO_PIN_12
#define CS2_GPIO_Port GPIOB
#define CS1_Pin GPIO_PIN_13
#define CS1_GPIO_Port GPIOB
#define M1A_Pin GPIO_PIN_14
#define M1A_GPIO_Port GPIOB
#define M1B_Pin GPIO_PIN_15
#define M1B_GPIO_Port GPIOB
#define IMU_TEST_Pin GPIO_PIN_8
#define IMU_TEST_GPIO_Port GPIOD
#define BUZZ_Pin GPIO_PIN_9
#define BUZZ_GPIO_Port GPIOD
#define B3A_Pin GPIO_PIN_12
#define B3A_GPIO_Port GPIOD
#define B3B_Pin GPIO_PIN_13
#define B3B_GPIO_Port GPIOD
#define B4A_Pin GPIO_PIN_6
#define B4A_GPIO_Port GPIOC
#define B4B_Pin GPIO_PIN_7
#define B4B_GPIO_Port GPIOC
#define D1_Pin GPIO_PIN_15
#define D1_GPIO_Port GPIOA
#define NRF_IQR_Pin GPIO_PIN_3
#define NRF_IQR_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_3
#define D2_GPIO_Port GPIOB
#define NRF_CSN_Pin GPIO_PIN_6
#define NRF_CSN_GPIO_Port GPIOB
#define NRF_CE_Pin GPIO_PIN_7
#define NRF_CE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void processData(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
