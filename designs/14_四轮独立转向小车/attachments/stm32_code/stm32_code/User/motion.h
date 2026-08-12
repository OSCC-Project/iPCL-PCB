/*
 * moton.h
 *
 *  Created on: Feb 20, 2025
 *      Author: yangtao
 */

#ifndef MOTON_H_
#define MOTON_H_

#include <stdint.h>
/**
 * @brief 输入参数结构体
 *
 * 该结构体包含了车体的线速度（vx, vy, vz）。
 */
typedef struct {
    int8_t vx;    /**< x轴方向的线速度 */
    int8_t vy;    /**< y轴方向的线速度 */
    int8_t vz;    /**< z轴方向的角速度 */
} MotionInput;

/**
 * @brief 输出参数结构体
 *
 * 该结构体包含了四个轮子的速度（vlf, vrf, vlr, vrr）和角度（poslf, posrf, poslr, posrr）。
 */
typedef struct {
    int8_t vlf;   /**< 左前轮速度 */
    int8_t vrf;   /**< 右前轮速度 */
    int8_t vlr;   /**< 左后轮速度 */
    int8_t vrr;   /**< 右后轮速度 */
    int8_t poslf; /**< 左前轮角度 */
    int8_t posrf; /**< 右前轮角度 */
    int8_t poslr; /**< 左后轮角度 */
    int8_t posrr; /**< 右后轮角度 */
} MotionOutput;

/**
 * @brief 计算运动学反解的函数
 *
 * 该函数根据输入的速度（vx, vy, vz），计算四个轮子的速度和角度。
 *
 * @param[in] input 输入结构体，包含车体的线速度（vx, vy, vz）
 * @param[out] output 输出结构体，包含四个轮子的速度和角度
 */
void MOTION_Inverse_Resolve(MotionInput *input, MotionOutput *output);
void printMotionOutput(MotionOutput *motion);
#endif /* MOTON_H_ */
