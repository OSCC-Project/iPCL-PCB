/*
 * gear_motor.h
 *
 *  Created on: Feb 17, 2025
 *      Author: yangtao
 */

#ifndef GEAR_MOTOR_H_
#define GEAR_MOTOR_H_

#include "main.h"


extern  int32_t Motor_target_speed[4];



void Motor_Wheels_Init(void) ;
void Motor_Wheels_SetPWM(uint8_t motor_id, int16_t speed);
void Motor_Wheels_SetPWMs(int16_t wheel1, int16_t wheel2, int16_t wheel3, int16_t wheel4);

void Motor_Encoder_Init(void);
int16_t Motor_Encoder_GetSpeed(uint8_t encoder_id) ;
void Motor_Schedule(TIM_HandleTypeDef *htim);


typedef struct {
    float kp, ki, kd;
    float prev_error, integral;
    float integral_limit;  // 积分限幅
} Motor_PID;
extern Motor_PID motor_pid[4];

void Motor_PID_Init(Motor_PID *pid, float kp, float ki, float kd, float integral_limit);
float Motor_PID_Calculate(Motor_PID *pid, float setpoint, float measured_value);


void Motor_Init(void);
void Motor_Update(void);

#endif /* GEAR_MOTOR_H_ */
