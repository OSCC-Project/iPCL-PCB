/**
******************************************************************************
  * @file           : os_handles.h
  * @brief          : freertos.c 的头文件
  * @date           : 2026/1/31
  * @license        : CC-BY-NC-SA 4.0
  * @note           : 本文件用于声明 freertos 任务/互斥锁/二进制量/消息队列 等句柄
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 雪萌_Xuemeng
  * All rights reserved.
  *
  * It is released under the CC-BY-NC-SA 4.0 open sourse license.
  ******************************************************************************
  */
#ifndef XM_POWER_KIT_OS_HANDLES_H
#define XM_POWER_KIT_OS_HANDLES_H

#include "cmsis_os2.h"
#include "ff.h"

// ============================= Task_Handles =============================//
// 底层任务（上电即运行）
extern osThreadId_t InitTaskHandle;               // 系统初始化任务句柄

// ============================= Mutex_Handles =============================//
extern osMutexId_t IIC1_MutexHandle;              // IIC1总线互斥锁句柄
extern osMutexId_t IIC3_MutexHandle;              // IIC3总线互斥锁句柄
extern osMutexId_t Flash_MutexHandle;             // FLASH使用互斥锁句柄

// ============================= Semaphore_Handles =============================//
extern osSemaphoreId_t FLASH_TX_Cplt_SemHandle;   // FLASH发送完成信号量句柄
extern osSemaphoreId_t FLASH_RX_Cplt_SemHandle;   // FLASH接收完成信号量句柄
extern osSemaphoreId_t LCD_DMA_Cplt_SemHandle;    // LCD DMA传输完成信号量句柄

// ============================= FileSystem_Handles =============================//
extern FATFS fs;
extern FIL fil;
extern FRESULT res;
extern UINT br;


#endif //XM_POWER_KIT_OS_HANDLES_H