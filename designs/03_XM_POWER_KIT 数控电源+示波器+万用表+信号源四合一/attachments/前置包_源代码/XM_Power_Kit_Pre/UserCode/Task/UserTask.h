/**
******************************************************************************
  * @file           : UserTask.h
  * @brief          : Task文件夹下所有.c源文件的头文件
  * @date           : 2026/1/31
  * @license        : CC-BY-NC-SA 4.0
  * @note           : 本文件用于声明所有 freertos 任务用到的定义、变量、函数等
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 雪萌_Xuemeng
  * All rights reserved.
  *
  * It is released under the CC-BY-NC-SA 4.0 open sourse license.
  ******************************************************************************
  */

#ifndef XM_POWER_KIT_USERTASK_H
#define XM_POWER_KIT_USERTASK_H
#include <stdbool.h>

#include "font.h"
#include "images.h"
#include "MCP4725.h"
#include "stm32f4xx_hal.h"


/*================ PublicExtern ================*/
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3;

extern char Sys_Version[30];
/*==================== InitTask ===================*/

extern MCP4725_HandleTypeDef hmcp4725_DC;
extern MCP4725_HandleTypeDef hmcp4725_OSC;

/*================== SensorTask =================*/
extern volatile float Input_Voltage;    // 输入电压
extern volatile float Input_Current;    // 输入电流
extern volatile float Input_Power;      // 输入功率

extern volatile float DCDC_Temperature;    // DCDC 温度

extern volatile uint8_t Fan_Duty_Cycle;    // 风扇占空比


#endif //XM_POWER_KIT_USERTASK_H

