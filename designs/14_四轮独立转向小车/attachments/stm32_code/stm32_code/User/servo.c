/*
 * steering_engine.c
 *
 *  Created on: Feb 17, 2025
 *      Author: yangtao
 */

/*TIM2 挂在 APB1- timer(84MHZ)总线上
 * Prescaler (PSC) = 84-1
 * Counter Period (ARR) = 20000-1
 * Pulse=1500   1.5ms,0度
 * IO 开漏输出（电平转换）
 */

#include "servo.h"
#include "tim.h"
// 用于保存各通道当前角度
static uint8_t s_servoAngleCH1 = 90;
static uint8_t s_servoAngleCH2 = 90;
static uint8_t s_servoAngleCH3 = 90;
static uint8_t s_servoAngleCH4 = 90;


/**
  * @brief  四路舵机的初始化
  * @param  None
  * @retval None
  */
void Servo_Init(void)
{
  // 启动所有 PWM 通道，假设 TIM2 已初始化完成
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

  // 初始化所有舵机为 90° (1.5ms 中心脉宽)
  Servo_SetAngle(&htim2, TIM_CHANNEL_1, 90);
  Servo_SetAngle(&htim2, TIM_CHANNEL_2, 90);
  Servo_SetAngle(&htim2, TIM_CHANNEL_3, 80);
  Servo_SetAngle(&htim2, TIM_CHANNEL_4, 94);
}


/**
  * @brief  设置某个通道的舵机角度
  * @param  htim     定时器句柄(例如 &htim2)
  * @param  channel  通道(例如 TIM_CHANNEL_1)
  * @param  angle    目标角度(0~180)
  * @retval None
  */
void Servo_SetAngle(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle)
{
    // 角度范围限制
    if (angle > 180) {
        angle = 180;
    }
    if (angle < 0) {
            angle = 0;
        }

    // 计算对应的 CCR 值
    uint16_t ccrVal = 500 + (angle * 2000 / 180);

    // 更新比较匹配值(占空比)
    __HAL_TIM_SET_COMPARE(htim, channel, ccrVal);
}


/**
  * @brief  设置四路舵机角度
  * @param  angle1  通道1舵机目标角度 (0~180)
  * @param  angle2  通道2舵机目标角度 (0~180)
  * @param  angle3  通道3舵机目标角度 (0~180)
  * @param  angle4  通道4舵机目标角度 (0~180)
  * @retval None
  */
void Servo_SetAngles(uint8_t angle1, uint8_t angle2, uint8_t angle3, uint8_t angle4)
{
    // 通道1角度变化判断
    if (s_servoAngleCH1 != angle1)
    {
    	Servo_SetAngle(&htim2, TIM_CHANNEL_1, angle1);
        s_servoAngleCH1 = angle1;
    }

    // 通道2角度变化判断
    if (s_servoAngleCH2 != angle2)
    {
    	Servo_SetAngle(&htim2, TIM_CHANNEL_2, angle2);
        s_servoAngleCH2 = angle2;
    }

    // 通道3角度变化判断
    if (s_servoAngleCH3 != angle3)
    {
    	Servo_SetAngle(&htim2, TIM_CHANNEL_3, angle3);
        s_servoAngleCH3 = angle3;
    }

    // 通道4角度变化判断
    if (s_servoAngleCH4 != angle4)
    {
    	Servo_SetAngle(&htim2, TIM_CHANNEL_4, angle4);
        s_servoAngleCH4 = angle4;
    }
}



