#include "gd32f4xx.h"
#include "12leadapp.h"
#include "SEGGER_RTT.h"
#include "systick.h"
#include "stdbool.h"
#include "math.h"
#include "tgmath.h"

#ifndef M_PI
#define M_PI (3.141592f)
#endif


extern int led12_s_RR ;
extern int led12_s_QT ;
extern int led12_s_QTc ;

extern int led12_s_P ;
extern int led12_s_QRS ;
extern int led12_s_T ;

extern int led12_s_QRS_deg ;

extern float led12_s_RV1 ;
extern float led12_s_SV1 ;
extern float led12_s_RV5 ;
extern float led12_s_SV5 ;

extern int led12_s_HR ;


typedef struct
{
    float ts;       //采样周期(s)
    float fc;       //截至频率(hz)
    float lastYn;   //上一次滤波值
    float alpha;    //滤波系数
} low_pass_filter_t;
//初始化滤波系数
void Init_lowPass_alpha(low_pass_filter_t* const filter,const float ts, const float fc);
//低通滤波
float Low_pass_filter(low_pass_filter_t* const filter, const float data);

typedef struct
{
    float ts;       //采样周期(s)
    float fc;       //下限频率(hz)
    float lastYn;   //上一次滤波值
    float lastXn;   //上一次采样值
    float alpha;    //滤波系数
} hight_pass_filter_t;
//初始化滤波系数
void Init_hightPass_alpha(hight_pass_filter_t* const filter,const float ts, const float fc);
//高通滤波
float Hight_pass_filter(hight_pass_filter_t* const filter, const float data);


void solveecg10s();
void alltozeros();
