/*
 * gear_motor.c
 *
 *  Created on: Feb 17, 2025
 *      Author: yangtao
 */
/**
 * TIM9 CH1、CH2：用于控制第一个电机。
 * TIM12 CH1、CH2：用于控制第二个电机。
 * TIM5 CH1、CH2：用于控制第三个电机。
 * TIM5 CH3、CH4：用于控制第四个电机。
 * Prescaler (PSC) = 84-1
 * Counter Period (ARR) = 100
 * Pulse=0~100占空比
 *
 * TIM1、TIM3、TIM4、TIM9配置为编码器模式
 *
 * TIM14每隔1s中断清除编码器计数
 *
 * */
#include "gear_motor.h"
#include "tim.h"
#include <stdint.h>

static int32_t Motor_Encoder_Counts[4] = {0};  // 存储四个编码器的脉冲数
int32_t Motor_target_speed[4]={0};  //存储四个编码器的脉冲的目标脉冲数

/**
 * @fn void Motor_Init(void)
 * @brief 电机初始化
 *
 */
void Motor_Wheels_Init(void) {
    // 配置TIM9, TIM12, TIM5为PWM输出模式
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);  // 启动TIM9通道1 PWM
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);  // 启动TIM9通道2 PWM

    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1); // 启动TIM12通道1 PWM
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2); // 启动TIM12通道2 PWM

    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);  // 启动TIM5通道1 PWM
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);  // 启动TIM5通道2 PWM

    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);  // 启动TIM5通道3 PWM
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);  // 启动TIM5通道4 PWM
}

/**
 * @fn void Motor_SetPWM(uint8_t, int16_t)
 * @brief 设置电机PWM占空比
 *
 * @param motor_id
 * @param speed(-100~100)
 */
void Motor_Wheels_SetPWM(uint8_t motor_id, int16_t speed) {
    uint16_t pwm_value;

    if (speed >= 0) {
        pwm_value = (uint16_t)(speed);
    } else {
        pwm_value = (uint16_t)(-speed);
    }

    switch (motor_id) {
        case 1:
            if (speed >= 0) {
                __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, pwm_value);
                __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, 0);
            } else {
                __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, pwm_value);
            }
            break;
        case 2:
            if (speed >= 0) {
                __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, pwm_value);
                __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);
            } else {
                __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, pwm_value);
            }
            break;
        case 3:
            if (speed >= 0) {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, pwm_value);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, 0);
            } else {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, pwm_value);
            }
            break;
        case 4:
            if (speed >= 0) {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, pwm_value);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, 0);
            } else {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, pwm_value);
            }
            break;
        default:
            break;
    }
}

/**
 * @fn void Set_Wheels_PWM(int16_t wheel1, int16_t wheel2, int16_t wheel3, int16_t wheel4)
 * @brief 设置四个轮子的占空比
 *
 * @param wheel1(-100~100)
 * @param wheel2(-100~100)
 * @param wheel3(-100~100)
 * @param wheel4(-100~100)
 */
void Motor_Wheels_SetPWMs(int16_t wheel1, int16_t wheel2, int16_t wheel3, int16_t wheel4) {
	Motor_Wheels_SetPWM(1, wheel1);  // 设置轮子1的PWM
	Motor_Wheels_SetPWM(2, wheel2);  // 设置轮子2的PWM
	Motor_Wheels_SetPWM(3, wheel3);  // 设置轮子3的PWM
	Motor_Wheels_SetPWM(4, wheel4);  // 设置轮子4的PWM
}



/**
 * @fn void Encoder_Init(void)
 * @brief 编码器初始化,启动tim14
 *
 */
void Motor_Encoder_Init(void) {
    // 配置TIM1, TIM3, TIM4, TIM9为编码器接口模式
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim9, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(&htim14);
}





/**
 * @fn int16_t Encoder_GetSpeed(uint8_t encoder_id)
 * @brief 获取编码器每秒脉冲数
 *
 * @param encoder_id
 * @return int16_t
 */
int16_t Motor_Encoder_GetSpeed(uint8_t encoder_id) {
    int16_t speed = 0;

    // 计算每秒脉冲数
    switch (encoder_id) {
        case 1:
            speed = (int16_t)(Motor_Encoder_Counts[0]);
            break;
        case 2:
            speed = (int16_t)(Motor_Encoder_Counts[1]);
            break;
        case 3:
            speed = (int16_t)(Motor_Encoder_Counts[2]);
            break;
        case 4:
            speed = (int16_t)(Motor_Encoder_Counts[3]);
            break;
        default:
            break;
    }
    return speed;
}

/**
 * @fn void PID_Init(PID*, float, float, float, float)
 * @brief
 *
 * @param pid
 * @param kp
 * @param ki
 * @param kd
 * @param integral_limit
 */
void Motor_PID_Init(Motor_PID *pid, float kp, float ki, float kd, float integral_limit) {
   pid->kp = kp;
   pid->ki = ki;
   pid->kd = kd;
   pid->integral_limit = integral_limit;
   pid->prev_error = 0;
   pid->integral = 0;
}

/**
* @fn float PID_Calculate(PID *pid, float setpoint, float measured_value)
* @brief 计算PID输出
*
* @param pid
* @param setpoint
* @param measured_value
* @return float
*/
float Motor_PID_Calculate(Motor_PID *pid, float setpoint, float measured_value) {
   float error = setpoint - measured_value;  // 计算误差
   pid->integral += error;  // 累积误差值（积分项）

   // 限制积分项，防止其过大或过小
   if (pid->integral > pid->integral_limit) {
       pid->integral = pid->integral_limit;
   } else if (pid->integral < -pid->integral_limit) {
       pid->integral = -pid->integral_limit;
   }

   float derivative = error - pid->prev_error;  // 计算误差变化率（微分项）
   float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;  // 计算控制输出

   // 限制输出值，确保输出在设定的范围内
   if (output > 100.0f) {
       output = 100;
   } else if (output < -100.0f) {
       output = -100.0f;
   }

   pid->prev_error = error;  // 保存当前误差作为下一次的prev_error

   return output;
}

/**
 * @fn void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
 * @brief 定时器溢出回调函数，每0.1秒触发一次
 */
void Motor_Schedule(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM14) {
        // 每秒读取一次编码器脉冲数并清零
    	Motor_Encoder_Counts[0] = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
    	Motor_Encoder_Counts[1] = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    	Motor_Encoder_Counts[2] = (int16_t)__HAL_TIM_GET_COUNTER(&htim4);
    	Motor_Encoder_Counts[3] = (int16_t)__HAL_TIM_GET_COUNTER(&htim9);

        // 清零编码器计数器
        __HAL_TIM_SET_COUNTER(&htim1, 0);
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        __HAL_TIM_SET_COUNTER(&htim4, 0);
        __HAL_TIM_SET_COUNTER(&htim9, 0);
    }
}

/**
 * @fn void Motor_Init(void)
 * @brief
 *
 */
void Motor_Init(void){
	Motor_Wheels_Init();
	Motor_Encoder_Init();

	Motor_PID motor_pid[4];
	//方便运行中修改初始参数
	Motor_PID_Init(&motor_pid[0], 1.0f, 0.1f, 0.01f, 100.0f);
	Motor_PID_Init(&motor_pid[1], 1.0f, 0.1f, 0.01f, 100.0f);
	Motor_PID_Init(&motor_pid[2], 1.0f, 0.1f, 0.01f, 100.0f);
	Motor_PID_Init(&motor_pid[3], 1.0f, 0.1f, 0.01f, 100.0f);

}
/**
 * @fn void Motor_Update(void)
 * @brief
 *
 */
void Motor_Update(void){
	float pwm_motor1 = Motor_PID_Calculate(&motor_pid[0], Motor_target_speed[0], Motor_Encoder_Counts[0]);
	float pwm_motor2 = Motor_PID_Calculate(&motor_pid[1], Motor_target_speed[1], Motor_Encoder_Counts[1]);
	float pwm_motor3 = Motor_PID_Calculate(&motor_pid[2], Motor_target_speed[2], Motor_Encoder_Counts[2]);
	float pwm_motor4 = Motor_PID_Calculate(&motor_pid[3], Motor_target_speed[3], Motor_Encoder_Counts[3]);

	// 设置四个轮子的PWM占空比
	Motor_Wheels_SetPWMs((int16_t)pwm_motor1, (int16_t)pwm_motor2, (int16_t)pwm_motor3, (int16_t)pwm_motor4);

}

