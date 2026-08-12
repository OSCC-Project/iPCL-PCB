/*
 * steering_engine.h
 *
 *  Created on: Feb 17, 2025
 *      Author: yangtao
 */

#ifndef STEERING_ENGINE_H_
#define STEERING_ENGINE_H_

#include "main.h"


void Servo_Init(void);                             // 初始化四路舵机
void Servo_SetAngle(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle); // 设置某个通道舵机角度
void Servo_SetAngles(uint8_t angle1, uint8_t angle2, uint8_t angle3, uint8_t angle4); // 设置四路舵机角度

#endif /* STEERING_ENGINE_H_ */
