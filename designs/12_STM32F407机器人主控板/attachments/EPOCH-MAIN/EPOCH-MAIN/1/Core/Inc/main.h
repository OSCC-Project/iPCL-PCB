/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI2_CS_Pin GPIO_PIN_1
#define SPI2_CS_GPIO_Port GPIOC
#define U4_RF_TX_Pin GPIO_PIN_0
#define U4_RF_TX_GPIO_Port GPIOA
#define U4_RF_RX_Pin GPIO_PIN_1
#define U4_RF_RX_GPIO_Port GPIOA
#define U2_R232_TX_Pin GPIO_PIN_2
#define U2_R232_TX_GPIO_Port GPIOA
#define U2_R232_RX_Pin GPIO_PIN_3
#define U2_R232_RX_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_4
#define FLASH_CS_GPIO_Port GPIOA
#define FLASH_SCK_Pin GPIO_PIN_5
#define FLASH_SCK_GPIO_Port GPIOA
#define FLASH_MISO_Pin GPIO_PIN_6
#define FLASH_MISO_GPIO_Port GPIOA
#define FLASH_MOSI_Pin GPIO_PIN_7
#define FLASH_MOSI_GPIO_Port GPIOA
#define U3_R232_TX_Pin GPIO_PIN_8
#define U3_R232_TX_GPIO_Port GPIOD
#define U3_R232_RX_Pin GPIO_PIN_9
#define U3_R232_RX_GPIO_Port GPIOD
#define U6_R485_TX_Pin GPIO_PIN_6
#define U6_R485_TX_GPIO_Port GPIOC
#define U6_R485_RX_Pin GPIO_PIN_7
#define U6_R485_RX_GPIO_Port GPIOC
#define WS2812_Pin GPIO_PIN_8
#define WS2812_GPIO_Port GPIOA
#define U1_R485_TX_Pin GPIO_PIN_9
#define U1_R485_TX_GPIO_Port GPIOA
#define U1_R485_RX_Pin GPIO_PIN_10
#define U1_R485_RX_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
#define U5_TEST_TX_Pin GPIO_PIN_12
#define U5_TEST_TX_GPIO_Port GPIOC
#define U5_TEST_RX_Pin GPIO_PIN_2
#define U5_TEST_RX_GPIO_Port GPIOD
#define TEST_LED_Pin GPIO_PIN_4
#define TEST_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
