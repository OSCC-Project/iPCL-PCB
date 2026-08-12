/*
 * motion.c
 *
 *  Created on: Feb 20, 2025
 *      Author: yangtao
 */

/**
 * @file motion.c
 * @brief 运动学反解计算函数实现
 *
 * 该文件包含了一个计算运动学反解的函数。根据输入的速度（vx, vy, vz），
 * 计算四个轮子的速度和角度。
 */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "motion.h"
#include "usart.h"

//#define L_A 0.18          /**< 车体的纵向尺寸 */
//#define L_B 0.18      /**< 车体的横向尺寸 */
//#define Y_OFFSET 0.01    /**< 速度偏移量 */
#define W 0.18
#define D 0.18

#define B 0.18          /**< 车体的纵向尺寸 */
#define T 0.18      /**< 车体的横向尺寸 */
/**
 * @brief 运动学反解计算函数
 *
 * 该函数根据输入的vx, vy, vz计算四个轮子的速度和角度。
 *
 * @param[in] input 输入结构体，包含车体的线速度（vx, vy, vz）
 * @param[out] output 输出结构体，包含四个轮子的速度和角度
 */
void MOTION_Inverse_Resolve(MotionInput *input, MotionOutput *output) {
    float vx = (float)input->vx;
    float vy = (float)input->vy;
    float vz = (float)input->vz;



    // 减少重复计算，提取相同表达式

    if(vx == 0. && vy == 0. && vz == 0){
    	output->vlf = output->vrf = output->vlr = output->vrr = (int8_t)(0);
    	output->poslf = output->posrf = output->poslr = output->posrr = (int8_t)(0);
    }
    else if (vx == 0. && vy == 0.) {
        // 自转模式
        output->vlf = (int8_t)fabsf(-vz);
        output->vrf = (int8_t)fabsf(vz);
        output->vlr = -(int8_t)fabsf(-vz);
        output->vrr = -(int8_t)fabsf(vz);
        output->poslf = (int8_t)(-atan(B / T) * 180 / M_PI);
        output->posrf = (int8_t)(atan(B / T) * 180 / M_PI);
        output->poslr = (int8_t)(-atan(B / T) * 180 / M_PI);
        output->posrr = (int8_t)(atan(B / T) * 180 / M_PI);
    } else if (vy == 0 && vz != 0) {

    	float R = vz/100;
    	if (  (R>0) & (R <= 0.1)){
    		R = 0.1;
    	}
    	else if((R<0) & (R >= -0.1)){
    		R=-0.1;
    	}



    	if (R>0){
    		float w_s = vx / (W+R/2);
			output->vlf = (int8_t)( w_s * (W) );
			output->vlr = - output->vlf;
			output->vrf = (int8_t) -(w_s * (W+R)  );
			output->vrr = - output->vrf;

			output->poslf = (int8_t)(-atan((D/2) / (R)) * 180 / M_PI);
			output->posrf = (int8_t)(-atan((D/2) / (R+W)) * 180 / M_PI);
			output->poslr = (int8_t)(- output->poslf);
			output->posrr = (int8_t)(- output->posrf);
    	}
    	else if(R<0){
    		float w_s = vx / (W-R/2);
    		output->vlf = (int8_t)( w_s * (W-R) );
			output->vlr = - output->vlf;
			output->vrf = (int8_t) -(w_s * (W)  );
			output->vrr = - output->vrf;

			output->poslf = (int8_t)(int8_t)(-atan((D/2) / (R-W)) * 180 / M_PI);
			output->posrf = (int8_t)(int8_t)(-atan((D/2) / (R)) * 180 / M_PI);
			output->poslr = (int8_t)(- output->poslf);
			output->posrr = (int8_t)(- output->posrf);
    	}





		// 圆环模式
//    	vz=500;
//    	float v_offset = vz * Y_OFFSET;
//		float R= vz;

//    	output->vlf = (int8_t)( vx* sqrtf(powf(R + L_B/2, 2) + powf(L_A/2, 2)) - v_offset );
//		output->vrf = (int8_t) -( vx* sqrtf(powf(R - L_B/2, 2) + powf(L_A/2, 2)) + v_offset);
//		output->vlr = - output->vlf;
//		output->vrr = - output->vrf;
//		output->poslf = (int8_t)(atanf((L_A/2) / (R + L_B/2)) * 180 / M_PI);
//		output->posrf = (int8_t)(atanf((L_A/2) / (R - L_B/2)) * 180 / M_PI);
//		output->poslr = (int8_t)(- output->poslf);
//		output->posrr = (int8_t)(- output->posrf);

		//    	output->vlf = (int8_t)vx;
//		output->vrf = (int8_t)-vx;
//		output->vlr = (int8_t)-vx;
//		output->vrr = (int8_t)vx;
//		output->poslf = (int8_t)vz/2;
//		output->posrf = (int8_t)vz/2;
//		output->poslr = (int8_t)-vz/2;
//		output->posrr = (int8_t)-vz/2;

    } else {
        // 平移模式
        float v = sqrtf(powf(vx, 2) + powf(vy, 2));
        if (vx<0)
		{
        	v=-v;
		}
        int8_t v_int8 = (int8_t)v;
         output->vrf = output->vlr  = -v_int8;
         output->vlf= output->vrr= v_int8;
        if (vx != 0) {
            int8_t angle = (int8_t)(atanf(vy / vx) * 180 / M_PI);
            output->poslf = output->posrf = output->poslr = output->posrr = angle;
        } else {
            output->poslf = output->posrf = output->poslr = output->posrr = (int8_t)(90);
        }
    }
}

void printMotionOutput(MotionOutput *motion) {
    Serial_Printf("左前轮速度: %d, 右前轮速度: %d, 左后轮速度: %d, 右后轮速度: %d, ",
    		motion->vlf, motion->vrf, motion->vlr, motion->vrr);
	Serial_Printf("左前轮角度: %d, 右前轮角度: %d, 左后轮角度: %d, 右后轮角度: %d\r\n",
		   (int8_t)motion->poslf+90, (int8_t)motion->posrf+90, (int8_t)motion->poslr+90, (int8_t)motion->posrr+90);
}
