/*********************************************************************************************************
* 模块名称：SPO2.c
* 摘    要：SPO2模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日
* 内    容：
* 注    意：                                                                  
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "SPO2.h"
#include "ADC.h"
#include "DAC.h"
#include "Timer.h"
#include "gd32f4xx.h"
#include "SEGGER_RTT.h"
#include <stdbool.h>

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define DATA_ADJ_NUM 16
#define DATA_CAL_NUM 400
#define PEAK_NUM		 16
#define DATA_BUFFERING_NUM 4

#define RED_FLAG  0
#define IR_FLAG   1
#define SPO2_FLAG 2

#define PLUG_IN_MIN_VAL			2000	//插头未插入最小值
#define IR_ADJ_END_LINE   	1200	//红外光粗调中线
#define RED_ADJ_END_LINE  	1200	//红光粗调中线
#define IR_READJ_MAX_VAL		400		//红外光重调上限
#define RED_READJ_MAX_VAL 	400		//红光重调上限
#define IR_FINE_ADJ_OFFSET	200		//红外光细调偏移值
#define RED_FINE_ADJ_OFFSET 200   //红光细调偏移值
#define FIN_OFF_MAX_VAL   	100		//手指脱落上限

#define IR_PEAK_MAX_OFFSET  300		//寻找红外光峰值偏移值
#define RED_PEAK_MAX_OFFSET 300		//寻找红光峰值偏移值

#define MAX_RED_DA_VAL			240		//红外光最大DA值
#define MAX_IR_DA_VAL				240		//红外光最大DA值

#define MIN_RED_DA_VAL			100		//红外光最小DA值
#define MIN_IR_DA_VAL				100		//红外光最小DA值

#define ROUGH_ADJ_STEP			10		//粗调光DA值步进
#define FINE_ADJ_STEP				1			//微调光DA值步进
#define CLR_IR	gpio_bit_reset(GPIOD,GPIO_PIN_11)//已校对
#define SET_IR	gpio_bit_set(GPIOD,GPIO_PIN_11)

#define CLR_RED	gpio_bit_reset(GPIOD,GPIO_PIN_10)//已校对
#define SET_RED	gpio_bit_set(GPIOD,GPIO_PIN_10)

#define CLR_CAP	gpio_bit_reset(GPIOD,GPIO_PIN_9)//已校对
#define SET_CAP	gpio_bit_set(GPIOD,GPIO_PIN_9)

bool figer = 0;
bool online = 0;
/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量定义
*********************************************************************************************************/
static u16 s_arrIRAdjData[DATA_ADJ_NUM];	  //存放红外光ADC数据
static u16 s_arrREDAdjData[DATA_ADJ_NUM];	  //存放红光ADC数据

static u16 s_iIRDAVal  = MIN_IR_DA_VAL;		//红外光DA值
static u16 s_iREDDAVal = MIN_RED_DA_VAL;	//红光DA值
	
//JudgePlugIn判断插头是否插入
static u8 s_iPlugInCnt   = 0;				//插头插入计数
static u8 s_iPlugOffCnt  = 0;				//插头拔出计数
static u8 s_iPlugState   = 0;				//插头插入状态

//JudgeFingerIn判断手指是否放入
static u8 s_iUnstableCnt    = 0;		//手指处于不稳定状态计数
static u8 s_iStableCnt      = 0;		//手指处于稳定状态计数

//JudgeIfRoughAdj判断是否需要调光
static u8 s_iIRAdjFlag    = 0;	//重调红外光标志
static u8 s_iREDAdjFlag   = 0;	//重调红光标志
static u8 s_iIRAdjCnt     = 0;			//红外光粗调计数
static u8 s_iIRStableCnt  = 0;			//红外光稳定计数
static u8 s_iREDAdjCnt    = 0;			//红光粗调计数
static u8 s_iREDStableCnt = 0;			//红光稳定计数

//JudgeIfResetAdj判断是否需要重调
static u8 s_iResetAdjCnt = 0;				//重调光计数

//JudgeIfFineAdj判断是否需要微调
static i8 s_iIRFineAdjFlag  = 0;		//红外光微调计数
static i8 s_iREDFineAdjFlag = 0;		//红光微调计数

static EnumSpo2Mode s_enumADJState = PLUG_OFF; //测量进度

static u16 s_iWaveBuf;	//血氧波形

static u16 s_arrIRCalData[DATA_CAL_NUM];		//红外光计算数据
static u16 s_arrREDCalData[DATA_CAL_NUM];		//红光计算数据
static u16 s_iIRCalCnt  = 0;		//红外光计算数据计数
static u16 s_iREDCalCnt = 0;		//红光计算数据计数
static u64 s_iIRTime[2];				//红外光采样开始和结束时间
static u64 s_iREDTime[2];				//红光采样开始和结束时间

static u16 s_arrIRMaxValue[PEAK_NUM][2];		//红外光数据最高点
static u16 s_arrIRMinValue[PEAK_NUM][2];		//红外光数据最低点
static u8  s_iIRMaxPointNum;								//红外光数据最高点个数
static u8  s_iIRMinPointNum;								//红外光数据最低点个数
static u16 s_arrREDMaxValue[PEAK_NUM][2];	  //红光数据最高点
static u16 s_arrREDMinValue[PEAK_NUM][2]; 	//红光数据最低点
static u8  s_iREDMaxPointNum;								//红光数据最高点个数
static u8  s_iREDMinPointNum;								//红光数据最低点个数

static u8  s_iCalSPO2Flag = 0;	//计算血氧的标志
static StructCalSPO2 s_structureCalSPO2;		//存放计算时需要用到的数据
static StructSPO2Data s_structureSPO2BufData[DATA_BUFFERING_NUM];	//存放缓冲结果
static StructSPO2Data s_structureSPO2Data;	//最终结果

const static u16 c_arrR2SPO2[11]={560,600,630,650,680,710,740,760,790,820,860};		//	R值转为血氧百分比的参数

//SendDataByBT通过蓝牙发送数据
//static StructPackType s_structureBTSendPack; 	//数据包
//static u8 s_iIRBTFlag  = 0;		//蓝牙发送红外光标志

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static  void  ConfigSPO2GPIO(void);   	//配置SPO2的GPIO
static  u8    JudgePlugIn(u16 IRSum,u16 REDSum);			//判断插头是否插入
static  u8    JudgeFingerIn(u16 IRSum,u16 REDSum);		//判断手指是否放入
static  u8    JudgeIfRoughAdj(u16 IRSum,u16 REDSum);	//判断是否需要粗调光
static  u8    JudgeIfResetAdj(u16 IRSum,u16 REDSum);	//判断是否需要重调
static  u8    JudgeIfFineAdj(u16 IRSum,u16 REDSum);		//判断是否需要微调
static  void  ResetAdjust(void);				//复位调光
static  u16   Filter(u16 data,u8 flag);	//滤波
static  u8    FindPeaks(u8 flag);		    //寻找峰值
static  u8    HandleData(u8 flag);			//数据处理
static  void  CalData(void);				    //计算脉搏和血氧数值
/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ConfigSPOGPIO
* 函数功能：配置SPO的GPIO 
* 输入参数：void 
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  void  ConfigSPO2GPIO(void)
{
  //使能RCU相关时钟
  rcu_periph_clock_enable(RCU_GPIOD);  //使能GPIOA的时钟
  
  //gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);  //设置SPO2_IR_CS输出模式及速度
  //gpio_bit_set(GPIOB,GPIO_PIN_1);                                      //将SPO2_IR_CS默认状态设置为低电平
  gpio_mode_set(GPIOD,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_11);
  gpio_output_options_set(GPIOD,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
  gpio_bit_set(GPIOD,GPIO_PIN_11);
  
  //gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);  //设置SPO2_RED_CS输出模式及速度
  //gpio_bit_set(GPIOB,GPIO_PIN_2);                                   //将SPO2_RED_CS默认状态设置为低电平
  gpio_mode_set(GPIOD,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_10);
  gpio_output_options_set(GPIOD,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
  gpio_bit_set(GPIOD,GPIO_PIN_10);
	
  //gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  //设置SPO2_CAP输出模式及速度
  //gpio_bit_set(GPIOB,GPIO_PIN_0);                                   //将SPO2_CAP默认状态设置为熄灭
  gpio_mode_set(GPIOD,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_9);
  gpio_output_options_set(GPIOD,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
  gpio_bit_set(GPIOD,GPIO_PIN_9);
}

/*********************************************************************************************************
* 函数名称：JudgePlugIn
* 函数功能：判断插头是否插入
* 输入参数：void 
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8  JudgePlugIn(u16 IRSum,u16 REDSum)
{	
	//A/D数值小于插头插入A/D最小值，即插头插入
	if(IRSum <= PLUG_IN_MIN_VAL*DATA_ADJ_NUM && REDSum <= PLUG_IN_MIN_VAL*DATA_ADJ_NUM )
	{
		if(s_iPlugOffCnt > 0)
		{
			s_iPlugOffCnt = 0;
			s_iPlugInCnt  = 1;
			return s_iPlugState;
		}
		else if(s_iPlugInCnt < 10)
		{
			s_iPlugInCnt++;
			return s_iPlugState;
		}
		else 
		{
			s_iPlugState = 1;
			return s_iPlugState;
		}
	}
	//A/D数值大于插头插入A/D最小值，即插头拔出
	else
	{
		if(s_iPlugInCnt > 0)
		{
			s_iPlugInCnt  = 0;
			s_iPlugOffCnt = 1;
			return s_iPlugState;
		}
		else if(s_iPlugOffCnt < 10)
		{
			s_iPlugOffCnt++;
			return s_iPlugState;
		}
		else
		{
			s_iPlugState = 0;
			return s_iPlugState;
		}
	}
}

/*********************************************************************************************************
* 函数名称：JudgeFingerIn
* 函数功能：判断手指是否放入
* 输入参数：0：手指未放入；1：手指放入且处于稳定状态；2：手指放入但处于不稳定状态 
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8    JudgeFingerIn(u16 IRSum,u16 REDSum)
{	
	//手指插入
	if(IRSum > FIN_OFF_MAX_VAL*DATA_ADJ_NUM && REDSum > FIN_OFF_MAX_VAL*DATA_ADJ_NUM)
	{
    figer = 1;
    //SEGGER_RTT_printf(0, "Figer In\r\n");
		if(s_iUnstableCnt > 0)
		{
			s_iUnstableCnt = 0;
			s_iStableCnt   = 1;
			return 2;
		}
		else if(s_iStableCnt < 20)
		{
			s_iStableCnt++;
			return 2;
		}
		return 1;
	}
	//手指未插入
	else
	{
    figer = 0;
    //SEGGER_RTT_printf(0, "Figer Out\r\n");
		if(s_iStableCnt > 0)
		{
			s_iStableCnt = 0;
			s_iUnstableCnt = 1;
			return 2;
		}
		else if(s_iUnstableCnt < 20)
		{
			s_iUnstableCnt++;
			return 0;
		}
		return 0;
	}
}

/*********************************************************************************************************
* 函数名称：JudgeIfRoughAdj
* 函数功能：判断是否需要粗调光
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8  JudgeIfRoughAdj(u16 IRSum,u16 REDSum)
{
	u8 flag = 1;
	
	if(s_iIRAdjFlag != 1)
	{
		flag = 0;
		if(IRSum >= IR_ADJ_END_LINE*DATA_ADJ_NUM)
		{
			if(s_iIRAdjCnt < 3)
			{
				s_iIRAdjCnt++;
			}
			else
			{
				s_iIRAdjCnt = 0;
				s_iIRDAVal += ROUGH_ADJ_STEP;
			}
		}
		else
		{
			if(s_iIRStableCnt < 3)
			{
				s_iIRStableCnt++;
			}
			else
			{
				s_iIRStableCnt = 0;
				s_iIRAdjFlag   = 1;
			}
		}
	}
	
	if(s_iREDAdjFlag != 1)
	{
		flag = 0;
		if(REDSum >= RED_ADJ_END_LINE*DATA_ADJ_NUM)
		{
			if(s_iREDAdjCnt < 3)
			{
				s_iREDAdjCnt++;
			}
			else
			{
				s_iREDAdjCnt = 0;
				s_iREDDAVal += ROUGH_ADJ_STEP;
			}
		}
		else
		{
			if(s_iREDStableCnt < 3)
			{
				s_iREDStableCnt++;
			}
			else
			{
				s_iREDStableCnt = 0;
				s_iREDAdjFlag   = 1;
			}
		}
	}
	return flag;
}

/*********************************************************************************************************
* 函数名称：JudgeIfResetAdj
* 函数功能：判断是否需要重调光
* 输入参数：void
* 输出参数：void
* 返 回 值：1:需要重调光，0:不需要重调光
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8    JudgeIfResetAdj(u16 IRSum,u16 REDSum)
{	
	if(IRSum < IR_READJ_MAX_VAL*DATA_ADJ_NUM || REDSum < RED_READJ_MAX_VAL*DATA_ADJ_NUM 
		|| s_iIRDAVal > MAX_IR_DA_VAL || s_iREDDAVal > MAX_RED_DA_VAL || s_iIRDAVal <MIN_IR_DA_VAL ||s_iREDDAVal < MIN_RED_DA_VAL
		|| s_iIRFineAdjFlag > 50 || s_iIRFineAdjFlag < (-50) || s_iREDFineAdjFlag > 50 || s_iREDFineAdjFlag < (-50))
	{
		if(s_iResetAdjCnt < 3)
		{
			s_iResetAdjCnt++;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		s_iResetAdjCnt = 0;
	}
	return 0;
}
/*********************************************************************************************************
* 函数名称：JudgeIfFineAdj
* 函数功能：判断是否需要微调
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8  JudgeIfFineAdj(u16 IRSum,u16 REDSum)
{
	u8 flag = 0;
	
	if(s_iIRAdjFlag == 1)
	{
		if(IRSum > DATA_ADJ_NUM*(IR_ADJ_END_LINE+IR_FINE_ADJ_OFFSET))
		{
			s_iIRDAVal += FINE_ADJ_STEP;
			s_iIRFineAdjFlag--;
			flag = 1;
		}
		else if(IRSum < DATA_ADJ_NUM*(RED_ADJ_END_LINE-IR_FINE_ADJ_OFFSET))
		{
			s_iIRDAVal -= FINE_ADJ_STEP;
			s_iIRFineAdjFlag++;
			flag = 1;
		}
	}
	if(s_iREDAdjFlag == 1)
	{
		if(REDSum > DATA_ADJ_NUM*(RED_ADJ_END_LINE+RED_FINE_ADJ_OFFSET))
		{
			s_iREDDAVal += FINE_ADJ_STEP;
			s_iREDFineAdjFlag--;
			flag = 1;
		}
		else if(REDSum < DATA_ADJ_NUM*(RED_ADJ_END_LINE-RED_FINE_ADJ_OFFSET))
		{
			s_iREDDAVal -= FINE_ADJ_STEP;
			s_iREDFineAdjFlag++;
			flag = 1;
		}
	}
	return flag;
}

/*********************************************************************************************************
* 函数名称：ResetAdjust
* 函数功能：复位调光
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  void  ResetAdjust(void)
{
	u8 i;
	
	s_iIRAdjFlag  = 0;			//重调红外光标志
	s_iREDAdjFlag = 0;			//重调红光标志

	s_iIRDAVal  = MIN_IR_DA_VAL;	//红外光DA值
	s_iREDDAVal = MIN_RED_DA_VAL;	//红光DA值
/*
	//JudgePlugIn判断插头是否插入
  s_iPlugInCnt   = 0;			//插头插入计数
  s_iPlugOffCnt  = 0;			//插头拔出计数
  s_iPlugState   = 0;			//插头插入状态

	//JudgeFingerIn判断手指是否放入
  s_iUnstableCnt = 0;		  //手指处于不稳定状态计数
  s_iStableCnt   = 0;		  //手指处于稳定状态计数
*/
  //JudgeIfRoughAdj判断是否需要调光
  s_iIRAdjCnt     = 0;			//红外光粗调计数
  s_iIRStableCnt  = 0;			//红外光稳定计数
  s_iREDAdjCnt    = 0;			//红光粗调计数
  s_iREDStableCnt = 0;			//红光稳定计数
	
	//JudgeIfResetAdj判断是否需要重调
	s_iResetAdjCnt = 0;				//重调光计数
	
	//JudgeIfFineAdj判断是否需要微调
	s_iIRFineAdjFlag  = 0;		//红外光微调计数
	s_iREDFineAdjFlag = 0;		//红光微调计数

	s_iREDCalCnt = 0;					//红光计算数据计数
	s_iIRCalCnt  = 0;					//红外光计算数据计数
	
	s_iCalSPO2Flag = 0;
	for(i=0;i<DATA_BUFFERING_NUM;i++)
	{
		s_structureSPO2BufData[i].PI = 0;
		s_structureSPO2BufData[i].PulseRate = 0;
		s_structureSPO2BufData[i].SPO2 = 0;
		s_structureSPO2BufData[i].R = 0;
	}
	
	s_structureSPO2Data.PI = 0;
	s_structureSPO2Data.PulseRate = 0;
	s_structureSPO2Data.R = 0;
	s_structureSPO2Data.SPO2 = 0;
	
	//SendDataByBT通过蓝牙发送数据
	//s_iIRBTFlag  = 0;		//蓝牙发送红外光标志
}

/*********************************************************************************************************
* 函数名称：ResetAdjust
* 函数功能：滤波
* 输入参数：0:红光数据，1:红外光数据
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u16   Filter(u16 data,u8 flag)
{
	const static i32 a[5] = {10000,-23695,23140,-10547,1874};
	const static i32 b[5] = {48,193,289,193,48};
	static u16 s_arrIRFltInData[5],s_arrREDFltInData[5],s_arrIRFltOutData[5],s_arrREDFltOutData[5];
	u8 i;
	i32 buf;
	
	if(s_iIRAdjFlag == 0 || s_iREDAdjFlag == 0)
	{
		for(i=0;i<5;i++)
		{
			s_arrIRFltInData[i]   = 0;
			s_arrREDFltInData[i]  = 0;
			s_arrIRFltOutData[i]  = 0;
			s_arrREDFltOutData[i] = 0;
		}
		return 0xffff;
	}
	else
	{
		if(flag == RED_FLAG)
		{
			for(i=0;i<4;i++)
			{
				s_arrREDFltOutData[i] = s_arrREDFltOutData[i+1];
				s_arrREDFltInData[i]  = s_arrREDFltInData[i+1];
			}
			s_arrREDFltInData[4] = data*10;
			buf = (i32)((b[0]*s_arrREDFltInData[4] + b[1]*s_arrREDFltInData[3] + b[2]*s_arrREDFltInData[2] + b[3]*s_arrREDFltInData[1] + b[4]*s_arrREDFltInData[0] - a[1] * s_arrREDFltOutData[3] - a[2] * s_arrREDFltOutData[2] - a[3] * s_arrREDFltOutData[1] - a[4] * s_arrREDFltOutData[0])/10000);
			if(buf < 0 || buf >= 40960)
			{
				s_arrREDFltInData[4] = 0;
				s_arrREDFltOutData[4] = 0;
			}
			else
			{
				s_arrREDFltOutData[4] = (u16)buf;
				
			}
			return s_arrREDFltOutData[4];
		}
		else
		{
			for(i=0;i<4;i++)
			{
				s_arrIRFltOutData[i] = s_arrIRFltOutData[i+1];
				s_arrIRFltInData[i]  = s_arrIRFltInData[i+1];
			}
			s_arrIRFltInData[4] = data*10;
			buf = (u16)((b[0]*s_arrIRFltInData[4] + b[1]*s_arrIRFltInData[3] + b[2]*s_arrIRFltInData[2] + b[3]*s_arrIRFltInData[1] + b[4]*s_arrIRFltInData[0] - a[1] * s_arrIRFltOutData[3] - a[2] * s_arrIRFltOutData[2] - a[3] * s_arrIRFltOutData[1] - a[4] * s_arrIRFltOutData[0])/10000);
			if(buf < 0 || buf >= 40960)
			{
				s_arrIRFltInData[4] = 0;
				s_arrIRFltOutData[4] = 0;
			}
			else
			{
				s_arrIRFltOutData[4] = (u16)buf;
			}
			return s_arrIRFltOutData[4];
		}
	}
}

/*********************************************************************************************************
* 函数名称：FindPeaks
* 函数功能：寻找顶峰值并计算峰值间时间差
* 输入参数：0:红光数据，1:红外光数据
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8  FindPeaks(u8 flag)
{
	u16 i;
	u16 max,min;
	u16 maxStdLine;
	u16 minStdLine;
	u8  unstableCnt = 0;
	//printf("Find Peaks Start\r\n");
	//对红光数据寻找顶峰值
	if (flag == RED_FLAG)
	{
		s_iREDMaxPointNum = 0;
		s_iREDMinPointNum = 0;
		max = s_arrREDCalData[0];
		min = s_arrREDCalData[0];
		for(i=1;i<DATA_CAL_NUM;i++)
		{
			if(s_arrREDCalData[i]>(RED_ADJ_END_LINE+RED_PEAK_MAX_OFFSET)*10 || s_arrREDCalData[i]<(RED_ADJ_END_LINE-RED_PEAK_MAX_OFFSET)*10)
			{
				if(i==0)
				{
					s_arrREDCalData[0] = s_arrREDCalData[1];
					max = s_arrREDCalData[0];
					min = s_arrREDCalData[0];
				}
				else
				{
					s_arrREDCalData[i] = s_arrREDCalData[i-1];
				}
				unstableCnt++;
			}
			if(unstableCnt >= 10)
			{
				//printf("Find RED Peaks Unsuccess\r\n");
				return 0;
			}
			if(max < s_arrREDCalData[i])
			{
				max = s_arrREDCalData[i];
			}
			if(min > s_arrREDCalData[i])
			{
				min = s_arrREDCalData[i];
			}
		}
		minStdLine = (u16)((max - min)*0.4)+min;
		maxStdLine = (u16)((max - min)*0.6)+min;
		for(i=0;i<400 && s_arrREDCalData[i]<=minStdLine;i++);	//让i移到数据大于最低标准线之上
		while(i<400 && s_iREDMinPointNum<PEAK_NUM)
		{
			for(;i<400 && s_arrREDCalData[i]>=minStdLine;i++);		//让i移到数据小于最低标准线之下
			if(i>=400)
				break;
			s_arrREDMinValue[s_iREDMinPointNum][0] = i;
			s_arrREDMinValue[s_iREDMinPointNum][1] = s_arrREDCalData[i];
			for(;i<400 && s_arrREDCalData[i]<=minStdLine;i++)		//寻找最低点
			{
				if(s_arrREDMinValue[s_iREDMinPointNum][1] > s_arrREDCalData[i])
				{
					s_arrREDMinValue[s_iREDMinPointNum][0] = i;
					s_arrREDMinValue[s_iREDMinPointNum][1] = s_arrREDCalData[i];
				}
			}
			if(i<400)
				s_iREDMinPointNum++;
			else
				break;
		}
		//寻找峰值
		for(i=0;i<400 && s_arrREDCalData[i]>=maxStdLine;i++);	//让i移到数据大于最低标准线之上
		while(i<400 && s_iREDMaxPointNum<PEAK_NUM)
		{
			for(;i<400 && s_arrREDCalData[i]<=maxStdLine;i++);		//让i移到数据小于最低标准线之下
			if(i>=400)
				break;
			s_arrREDMaxValue[s_iREDMaxPointNum][0] = i;
			s_arrREDMaxValue[s_iREDMaxPointNum][1] = s_arrREDCalData[i];
			for(;i<400 && s_arrREDCalData[i]>=maxStdLine;i++)		//寻找最低点
			{
				if(s_arrREDMaxValue[s_iREDMaxPointNum][1] < s_arrREDCalData[i])
				{
					s_arrREDMaxValue[s_iREDMaxPointNum][0] = i;
					s_arrREDMaxValue[s_iREDMaxPointNum][1] = s_arrREDCalData[i];
				}
			}
			if(i<400)
				s_iREDMaxPointNum++;
			else
				break;
		}
/*
			printf("Red Max Data:\r\n");
			for(i=0;i<s_iREDMaxPointNum;i++)
			{
				printf("%d  %d\r\n",s_arrREDMaxValue[i][0],s_arrREDMaxValue[i][1]);
			}
			printf("Red Min Data:\r\n");
			for(i=0;i<s_iREDMinPointNum;i++)
			{
				printf("%d  %d\r\n",s_arrREDMinValue[i][0],s_arrREDMinValue[i][1]);
			}
		*/
	}
	else
	{
		s_iIRMaxPointNum = 0;
		s_iIRMinPointNum = 0;
		max = s_arrIRCalData[0];
		min = s_arrIRCalData[0];
		for(i=1;i<DATA_CAL_NUM;i++)
		{
			if(s_arrIRCalData[i]>(IR_ADJ_END_LINE+IR_PEAK_MAX_OFFSET)*10 || s_arrIRCalData[i]<(IR_ADJ_END_LINE-IR_PEAK_MAX_OFFSET)*10)
			{
				if(i==0)
				{
					s_arrIRCalData[0] = s_arrIRCalData[1];
					max = s_arrIRCalData[0];
					min = s_arrIRCalData[0];
				}
				else
				{
					s_arrIRCalData[i] = s_arrIRCalData[i-1];
				}
				unstableCnt++;
			}
			if(unstableCnt >= 10)
			{
				//printf("Find IR Peaks Unsuccess\r\n");
				return 0;
			}
			
			if(max < s_arrIRCalData[i])
			{
				max = s_arrIRCalData[i];
			}
			if(min > s_arrIRCalData[i])
			{
				min = s_arrIRCalData[i];
			}
		}
		minStdLine = (u16)((max - min)*0.4)+min;
		maxStdLine = (u16)((max - min)*0.6)+min;
		//寻找谷值
		for(i=0;i<400 && s_arrIRCalData[i]<=minStdLine;i++);	//让i移到数据大于最低标准线之上
		while(i<400 && s_iIRMinPointNum<PEAK_NUM)
		{
			for(;i<400 && s_arrIRCalData[i]>=minStdLine;i++);		//让i移到数据小于最低标准线之下
			if(i>=400)
				break;
			s_arrIRMinValue[s_iIRMinPointNum][0] = i;
			s_arrIRMinValue[s_iIRMinPointNum][1] = s_arrIRCalData[i];
			for(;i<400 && s_arrIRCalData[i]<=minStdLine;i++)		//寻找最低点
			{
				if(s_arrIRMinValue[s_iIRMinPointNum][1] > s_arrIRCalData[i])
				{
					s_arrIRMinValue[s_iIRMinPointNum][0] = i;
					s_arrIRMinValue[s_iIRMinPointNum][1] = s_arrIRCalData[i];
				}
			}
			if(i<400)
				s_iIRMinPointNum++;
			else
				break;
		}
		//寻找峰值
		for(i=0;i<400 && s_arrIRCalData[i]>=maxStdLine;i++);	//让i移到数据大于最低标准线之上
		while(i<400 && s_iIRMaxPointNum<PEAK_NUM)
		{
			for(;i<400 && s_arrIRCalData[i]<=maxStdLine;i++);		//让i移到数据小于最低标准线之下
			if(i>=400)
				break;
			s_arrIRMaxValue[s_iIRMaxPointNum][0] = i;
			s_arrIRMaxValue[s_iIRMaxPointNum][1] = s_arrIRCalData[i];
			for(;i<400 && s_arrIRCalData[i]>=maxStdLine;i++)		//寻找最低点
			{
				if(s_arrIRMaxValue[s_iIRMaxPointNum][1] < s_arrIRCalData[i])
				{
					s_arrIRMaxValue[s_iIRMaxPointNum][0] = i;
					s_arrIRMaxValue[s_iIRMaxPointNum][1] = s_arrIRCalData[i];
				}
			}
			if(i<400)
				s_iIRMaxPointNum++;
			else
				break;
		}
	}
//	printf("Find Peaks finish\r\n");
	return 1;
}

/*********************************************************************************************************
* 函数名称：HandleData
* 函数功能：数据处理
* 输入参数：0:红光数据，1:红外光数据
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  u8    HandleData(u8 flag)
{
	u8  i = 0;
	u16  j;
	u32 buf;
//	printf("Handle Data Start\r\n");
	if(FindPeaks(flag) == 0)
	{
//		printf("Find Peaks Unsuccess\r\n");
		return 0;
	}
	else
	{
//		printf("Find Peaks Success\r\n");
		if(flag == RED_FLAG)
		{
			//最高点或最低点小于三个判定为失败
			if(s_iREDMinPointNum < 3 || s_iREDMaxPointNum < 3)
			{/*
				printf("\r\nRED Point Under 3\r\n");
			  for(j=0;j<DATA_CAL_NUM;j++)
				{
					printf("%d\r\n",s_arrREDCalData[j]);
				}
				printf("MAX POINT:\r\n");
				for(j=0;j<s_iREDMaxPointNum;j++)
				{
					printf("%d  %d\r\n",s_arrREDMaxValue[j][0],s_arrREDMaxValue[j][1]);
				}
				printf("MIN POINT:\r\n");
				for(j=0;j<s_iREDMinPointNum;j++)
				{
					printf("%d  %d\r\n",s_arrREDMinValue[j][0],s_arrREDMinValue[j][1]);
				}*/
				return 0;
			}
			//去除多余的点
			if (s_arrREDMaxValue[0][0] < s_arrREDMinValue[0][0])
			{
				while (i < 10)
				{
					if (i < (s_iREDMaxPointNum - 1) && i < (s_iREDMinPointNum))
					{
						//printf("i=%d    ", i);
						if (s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i][0] && (s_arrREDMaxValue[i + 1][0] < s_arrREDMinValue[i + 1][0] || i == (s_iREDMinPointNum - 1)))
						{
							//printf("s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i][0] && s_arrREDMaxValue[i + 1][0] < s_arrREDMinValue[i + 1][0]\n");
							i++;
						}
						else if (s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i][0] && (s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i + 1][0] || i == (s_iREDMinPointNum - 1)))
						{
							//printf("s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i][0] && s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i + 1][0]\n");
							for (j = i; j < s_iREDMinPointNum; j++)
							{
								s_arrREDMinValue[j][0] = s_arrREDMinValue[j + 1][0];
								s_arrREDMinValue[j][1] = s_arrREDMinValue[j + 1][1];
							}
							s_iREDMinPointNum--;
						}
						else
						{
							//printf("s_arrREDMaxValue[%d + 1][0] < s_arrREDMinValue[%d][0]\n", i, i);
							for (j = i + 1; j < s_iREDMaxPointNum; j++)
							{
								s_arrREDMaxValue[j][0] = s_arrREDMaxValue[j + 1][0];
								s_arrREDMaxValue[j][1] = s_arrREDMaxValue[j + 1][1];
							}
							s_iREDMaxPointNum--;
						}
					}
					else if (i < (s_iREDMinPointNum - 1))
					{
						s_arrREDMinValue[i][0] = s_arrREDMinValue[i + 1][0];
						s_arrREDMinValue[i][0] = s_arrREDMinValue[i + 1][0];
						s_iREDMinPointNum = i;
						break;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				while (s_arrREDMinValue[1][0] < s_arrREDMaxValue[0][0])
				{
					for (j = 0; j < s_iREDMinPointNum-1; j++)
					{
						s_arrREDMinValue[j][0] = s_arrREDMinValue[j+1][0];
						s_arrREDMinValue[j][1] = s_arrREDMinValue[j+1][1];
					}
					s_iREDMinPointNum--;
				}
				i = 1;
				while (i < 10)
				{
					if (i < (s_iREDMaxPointNum) && i < (s_iREDMinPointNum))
					{
					//	printf("i=%d    ", i);
						if (s_arrREDMaxValue[i][0] > s_arrREDMinValue[i][0] && (s_arrREDMaxValue[i][0] < s_arrREDMinValue[i + 1][0] || i == (s_iREDMinPointNum - 1)))
						{
							//printf("s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i][0] && s_arrREDMaxValue[i + 1][0] < s_arrREDMinValue[i + 1][0]\n");
							i++;
						}
						else if (s_arrREDMaxValue[i][0] > s_arrREDMinValue[i][0] && (s_arrREDMaxValue[i][0] > s_arrREDMinValue[i + 1][0] || i == (s_iREDMinPointNum - 1)))
						{
							//printf("s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i][0] && s_arrREDMaxValue[i + 1][0] > s_arrREDMinValue[i + 1][0]\n");
							for (j = i; j < s_iREDMinPointNum; j++)
							{
								s_arrREDMinValue[j][0] = s_arrREDMinValue[j + 1][0];
								s_arrREDMinValue[j][1] = s_arrREDMinValue[j + 1][1];
							}
							s_iREDMinPointNum--;
						}
						else
						{
							//printf("s_arrREDMaxValue[%d + 1][0] < s_arrREDMinValue[%d][0]\n", i, i);
							for (j = i; j < s_iREDMaxPointNum; j++)
							{
								s_arrREDMaxValue[j][0] = s_arrREDMaxValue[j + 1][0];
								s_arrREDMaxValue[j][1] = s_arrREDMaxValue[j + 1][1];
							}
							s_iREDMaxPointNum--;
						}
					}
					else if (i < (s_iREDMinPointNum - 1))
					{
						s_arrREDMinValue[i][0] = s_arrREDMinValue[i + 1][0];
						s_arrREDMinValue[i][0] = s_arrREDMinValue[i + 1][0];
						s_iREDMinPointNum = i;
						break;
					}
					else
					{
						break;
					}
				}
			}
			/*printf("Red Max Data:\r\n");
			for(i=0;i<s_iREDMaxPointNum;i++)
			{
				printf("%d  %d\r\n",s_arrREDMaxValue[i][0],s_arrREDMaxValue[i][1]);
			}
			for(i=0;i<s_iREDMinPointNum;i++)
			{
				printf("%d  %d\r\n",s_arrREDMinValue[i][0],s_arrREDMinValue[i][1]);
			}*/
		}
		else
		{
			//最高点或最低点小于三个判定为失败
			if(s_iIRMinPointNum < 3 || s_iIRMaxPointNum < 3)
			{/*
				printf("\r\nIR Point Under 3:");
				for(j=0;j<DATA_CAL_NUM;j++)
				{
					printf("%d\r\n",s_arrIRCalData[j]);
				}
				printf("MAX POINT:\r\n");
				for(j=0;j<s_iIRMaxPointNum;j++)
				{
					printf("%d  %d\r\n",s_arrIRMaxValue[j][0],s_arrIRMaxValue[j][1]);
				}
				printf("MIN POINT:\r\n");
				for(j=0;j<s_iIRMinPointNum;j++)
				{
					printf("%d  %d\r\n",s_arrIRMinValue[j][0],s_arrIRMinValue[j][1]);
				}		*/
				return 0;
			}
			//去除多余的点
			if (s_arrIRMaxValue[0][0] < s_arrIRMinValue[0][0])
			{
				while (i < 10)
				{
					if (i < (s_iIRMaxPointNum - 1) && i < (s_iIRMinPointNum))
					{
						//printf("i=%d    ", i);
						if (s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i][0] && (s_arrIRMaxValue[i + 1][0] < s_arrIRMinValue[i + 1][0] || i == (s_iIRMinPointNum - 1)))
						{
							//printf("s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i][0] && s_arrIRMaxValue[i + 1][0] < s_arrIRMinValue[i + 1][0]\n");
							i++;
						}
						else if (s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i][0] && (s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i + 1][0] || i == (s_iIRMinPointNum - 1)))
						{
							//printf("s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i][0] && s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i + 1][0]\n");
							for (j = i; j < s_iIRMinPointNum; j++)
							{
								s_arrIRMinValue[j][0] = s_arrIRMinValue[j + 1][0];
								s_arrIRMinValue[j][1] = s_arrIRMinValue[j + 1][1];
							}
							s_iIRMinPointNum--;
						}
						else
						{
							//printf("s_arrIRMaxValue[%d + 1][0] < s_arrIRMinValue[%d][0]\n", i, i);
							for (j = i + 1; j < s_iIRMaxPointNum; j++)
							{
								s_arrIRMaxValue[j][0] = s_arrIRMaxValue[j + 1][0];
								s_arrIRMaxValue[j][1] = s_arrIRMaxValue[j + 1][1];
							}
							s_iIRMaxPointNum--;
						}
					}
					else if (i < (s_iIRMinPointNum - 1))
					{
						s_arrIRMinValue[i][0] = s_arrIRMinValue[i + 1][0];
						s_arrIRMinValue[i][0] = s_arrIRMinValue[i + 1][0];
						s_iIRMinPointNum = i;
						break;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				while (s_arrIRMinValue[1][0] < s_arrIRMaxValue[0][0])
				{
					for (j = 0; j < s_iIRMinPointNum-1; j++)
					{
						s_arrIRMinValue[j][0] = s_arrIRMinValue[j+1][0];
						s_arrIRMinValue[j][1] = s_arrIRMinValue[j+1][1];
					}
					s_iIRMinPointNum--;
				}
				i = 1;
				while (i < 10)
				{
					if (i < (s_iIRMaxPointNum) && i < (s_iIRMinPointNum))
					{
						//printf("i=%d    ", i);
						if (s_arrIRMaxValue[i][0] > s_arrIRMinValue[i][0] && (s_arrIRMaxValue[i][0] < s_arrIRMinValue[i + 1][0] || i == (s_iIRMinPointNum - 1)))
						{
							//printf("s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i][0] && s_arrIRMaxValue[i + 1][0] < s_arrIRMinValue[i + 1][0]\n");
							i++;
						}
						else if (s_arrIRMaxValue[i][0] > s_arrIRMinValue[i][0] && (s_arrIRMaxValue[i][0] > s_arrIRMinValue[i + 1][0] || i == (s_iIRMinPointNum - 1)))
						{
							//printf("s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i][0] && s_arrIRMaxValue[i + 1][0] > s_arrIRMinValue[i + 1][0]\n");
							for (j = i; j < s_iIRMinPointNum; j++)
							{
								s_arrIRMinValue[j][0] = s_arrIRMinValue[j + 1][0];
								s_arrIRMinValue[j][1] = s_arrIRMinValue[j + 1][1];
							}
							s_iIRMinPointNum--;
						}
						else
						{
							//printf("s_arrIRMaxValue[%d + 1][0] < s_arrIRMinValue[%d][0]\n", i, i);
							for (j = i; j < s_iIRMaxPointNum; j++)
							{
								s_arrIRMaxValue[j][0] = s_arrIRMaxValue[j + 1][0];
								s_arrIRMaxValue[j][1] = s_arrIRMaxValue[j + 1][1];
							}
							s_iIRMaxPointNum--;
						}
					}
					else if (i < (s_iIRMinPointNum - 1))
					{
						s_arrIRMinValue[i][0] = s_arrIRMinValue[i + 1][0];
						s_arrIRMinValue[i][0] = s_arrIRMinValue[i + 1][0];
						s_iIRMinPointNum = i;
						break;
					}
					else
					{
						break;
					}
				}
			}
	/*		printf("Ir Max Data:\r\n");
			for(i=0;i<s_iIRMaxPointNum;i++)
			{
				printf("%d  %d\r\n",s_arrIRMaxValue[i][0],s_arrIRMaxValue[i][1]);
			}
			for(i=0;i<s_iIRMinPointNum;i++)
			{
				printf("%d  %d\r\n",s_arrIRMinValue[i][0],s_arrIRMinValue[i][1]);
			}*/
		}
	}
	if(flag == IR_FLAG)
	{
		if(s_iIRMinPointNum < 2 || s_iIRMaxPointNum < 2)
		{
			return 0;
		}
		else
		{
			buf = 0;
			for(i=0;i<s_iIRMaxPointNum;i++)
			{
				buf = buf + s_arrIRMaxValue[i][1];
			}
			s_structureCalSPO2.IirMax = (u16)(buf/s_iIRMaxPointNum);
			buf = 0;
			for(i=0;i<s_iIRMinPointNum;i++)
			{
				buf = buf + s_arrIRMinValue[i][1];
			}
			s_structureCalSPO2.IirMin = (u16)(buf/s_iIRMinPointNum);
			s_structureCalSPO2.IRTime = (u16)((s_iIRTime[1] - s_iIRTime[0])*(s_arrIRMaxValue[s_iIRMaxPointNum-1][0]-s_arrIRMaxValue[0][0])/DATA_CAL_NUM/(s_iIRMaxPointNum-1));
			s_structureCalSPO2.IRTime += (u16)((s_iIRTime[1] - s_iIRTime[0])*(s_arrIRMinValue[s_iIRMinPointNum-1][0]-s_arrIRMinValue[0][0])/DATA_CAL_NUM/(s_iIRMinPointNum-1));
			s_structureCalSPO2.IRTime = s_structureCalSPO2.IRTime>>1;
			if(s_iCalSPO2Flag == 0)
			{
				s_iCalSPO2Flag = 1;
			}
			else if(s_iCalSPO2Flag == 2)
			{
				CalData();
				s_iCalSPO2Flag = 0;
			}
		}
	}
	else
	{
		if(s_iREDMinPointNum < 2 || s_iREDMaxPointNum < 2)
		{
			return 0;
		}
		else
		{
			buf = 0;
			for(i=0;i<s_iREDMaxPointNum;i++)
			{
				buf = buf + s_arrREDMaxValue[i][1];
			}
			s_structureCalSPO2.IredMax = (u16)(buf/s_iREDMaxPointNum);
			buf = 0;
			for(i=0;i<s_iREDMinPointNum;i++)
			{
				buf = buf + s_arrREDMinValue[i][1];
			}
			s_structureCalSPO2.IredMin = (u16)(buf/s_iREDMinPointNum);
			s_structureCalSPO2.REDTime = (u16)((s_iREDTime[1] - s_iREDTime[0])*(s_arrREDMaxValue[s_iREDMaxPointNum-1][0]-s_arrREDMaxValue[0][0])/DATA_CAL_NUM/(s_iREDMaxPointNum-1));
			s_structureCalSPO2.REDTime += (u16)((s_iREDTime[1] - s_iREDTime[0])*(s_arrREDMinValue[s_iREDMinPointNum-1][0]-s_arrREDMinValue[0][0])/DATA_CAL_NUM/(s_iREDMinPointNum-1));
			s_structureCalSPO2.REDTime = s_structureCalSPO2.REDTime>>1;
			if(s_iCalSPO2Flag == 0)
			{
				s_iCalSPO2Flag = 2;
			}
			else if(s_iCalSPO2Flag == 1)
			{
				CalData();
				s_iCalSPO2Flag = 0;
			}
		}
	}
	/*
	if(flag == IR_FLAG)
	{
		printf("\r\nIR Data\r\n");
		for(i=0;i<400;i++)
		{
			printf("%d\r\n",s_arrIRCalData[i]);
		}
		printf("\r\n\r\n");
		for(i=0;i<s_iIRMinPointNum;i++)
		{
			printf("%d %d\r\n",s_arrIRMinValue[i][0],s_arrIRMinValue[i][1]);
		}
		for(i=0;i<s_iIRMaxPointNum;i++)
		{
			printf("%d %d\r\n",s_arrIRMaxValue[i][0],s_arrIRMaxValue[i][1]);
		}
	}
	else
	{
		printf("\r\nRED Data\r\n");
		for(i=0;i<400;i++)
		{
			printf("%d\r\n",s_arrREDCalData[i]);
		}
		printf("\r\n\r\n");
		for(i=0;i<s_iREDMinPointNum;i++)
		{
			printf("%d %d\r\n",s_arrREDMinValue[i][0],s_arrREDMinValue[i][1]);
		}
	}
	*/
//	printf("Handle Data Finish\r\n");
}

/*********************************************************************************************************
* 函数名称：CalData
* 函数功能：数据处理
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CalData(void)
{
	u8 i;
	u16 buf,j;
//	printf("Cal Data Start\r\n");
	
	buf = 60000/((s_structureCalSPO2.IRTime+s_structureCalSPO2.REDTime)>>1);
  /*
	if(buf < 40 || buf > 160)	//判断脉搏是否小于40或大于160
	{
//		printf("pulse rate under 40 or over 160");
		/*
		printf("\r\nRED Point:\r\n");
		for(j=0;j<DATA_CAL_NUM;j++)
		{
			printf("%d\r\n",s_arrREDCalData[j]);
		}
		printf("MAX POINT:\r\n");
		for(j=0;j<s_iREDMaxPointNum;j++)
		{
			printf("%d  %d\r\n",s_arrREDMaxValue[j][0],s_arrREDMaxValue[j][1]);
		}
		printf("MIN POINT:\r\n");
		for(j=0;j<s_iREDMinPointNum;j++)
		{
			printf("%d  %d\r\n",s_arrREDMinValue[j][0],s_arrREDMinValue[j][1]);
		}
		
		printf("\r\nIR Point:\r\n");
		for(j=0;j<DATA_CAL_NUM;j++)
		{
			printf("%d\r\n",s_arrIRCalData[j]);
		}
		printf("MAX POINT:\r\n");
		for(j=0;j<s_iIRMaxPointNum;j++)
		{
			printf("%d  %d\r\n",s_arrIRMaxValue[j][0],s_arrIRMaxValue[j][1]);
		}
		printf("MIN POINT:\r\n");
		for(j=0;j<s_iIRMinPointNum;j++)
		{
			printf("%d  %d\r\n",s_arrIRMinValue[j][0],s_arrIRMinValue[j][1]);
		}
		
		s_structureSPO2Data.PI = 0;
		s_structureSPO2Data.R  = 0;
		s_structureSPO2Data.SPO2 = 0;
		s_structureSPO2Data.PulseRate = 0;
		return;
	}*/
	for(i=0;i<DATA_BUFFERING_NUM-1;i++)
	{
		s_structureSPO2BufData[i].PI = s_structureSPO2BufData[i+1].PI;
		s_structureSPO2BufData[i].PulseRate = s_structureSPO2BufData[i+1].PulseRate;
		s_structureSPO2BufData[i].SPO2 =s_structureSPO2BufData[i+1].SPO2;
		s_structureSPO2BufData[i].R = s_structureSPO2BufData[i+1].R;
	}
	s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PulseRate = buf;
	s_structureSPO2BufData[DATA_BUFFERING_NUM-1].R = (s_structureCalSPO2.IredMax-s_structureCalSPO2.IredMin)*1000/(s_structureCalSPO2.IirMax-s_structureCalSPO2.IirMin);
	s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PI = (s_structureCalSPO2.IirMax-s_structureCalSPO2.IirMin)*1000/((s_structureCalSPO2.IirMax+s_structureCalSPO2.IirMin)>>1);
	for(i=0;i<11;i++)
	{
		if(s_structureSPO2BufData[DATA_BUFFERING_NUM-1].R<=c_arrR2SPO2[i])
		{
			break;
		}
	}
	s_structureSPO2BufData[DATA_BUFFERING_NUM-1].SPO2 = 100-i;
	s_structureSPO2Data.PI = 0;
	s_structureSPO2Data.R  = 0;
	s_structureSPO2Data.SPO2 = 0;
	s_structureSPO2Data.PulseRate = 0;
	for(i=0;i<DATA_BUFFERING_NUM && s_structureSPO2BufData[DATA_BUFFERING_NUM-1-i].PulseRate != 0;i++);	//判断有多少个有效数据
	switch (i)
	{
		case 1:
			s_structureSPO2Data.PI = s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PI;
			s_structureSPO2Data.R = s_structureSPO2BufData[DATA_BUFFERING_NUM-1].R;
			s_structureSPO2Data.SPO2 = s_structureSPO2BufData[DATA_BUFFERING_NUM-1].SPO2; 
			s_structureSPO2Data.PulseRate = s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PulseRate;
		break;
		case 2:
			s_structureSPO2Data.PI = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PI>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].PI>>1);
			s_structureSPO2Data.R = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].R>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].R>>1);
			s_structureSPO2Data.SPO2 = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].SPO2>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].SPO2>>1); 
			s_structureSPO2Data.PulseRate = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PulseRate>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].PulseRate>>1);
		break;
		default:
			s_structureSPO2Data.PI = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PI>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].PI>>2) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-3].PI>>2);
			s_structureSPO2Data.R = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].R>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].R>>2) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-3].R>>2);
			s_structureSPO2Data.SPO2 = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].SPO2>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].SPO2>>2) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-3].SPO2>>2); 
			s_structureSPO2Data.PulseRate = (s_structureSPO2BufData[DATA_BUFFERING_NUM-1].PulseRate>>1) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-2].PulseRate>>2) + (s_structureSPO2BufData[DATA_BUFFERING_NUM-3].PulseRate>>2);
		break;
	}
	if(s_structureSPO2Data.SPO2<92)
		s_structureSPO2Data.SPO2 += 2;
	else if(s_structureSPO2Data.SPO2<95)
		s_structureSPO2Data.SPO2++;
	if(s_structureSPO2Data.PI<2)
		s_structureSPO2Data.PI += 5;
	else if(s_structureSPO2Data.PI<4)
		s_structureSPO2Data.PI += 4;
	else if(s_structureSPO2Data.PI<6)
		s_structureSPO2Data.PI += 3;
	else if(s_structureSPO2Data.PI<8)
		s_structureSPO2Data.PI += 2;
	else if(s_structureSPO2Data.PI<10)
		s_structureSPO2Data.PI += 1;
	if(s_structureSPO2Data.SPO2==100)
		s_structureSPO2Data.SPO2 = 99;
  SEGGER_RTT_printf(0, "HR：%3d  SpO2：%3d%%  PI：%2.1f%%  R：%d\r\n",s_structureSPO2Data.PulseRate,s_structureSPO2Data.SPO2,s_structureSPO2Data.PI/10.0,s_structureSPO2Data.R);
//	printf("脉率：%3d  血氧饱和度：%3d%%  PI：%2.1f%%  R：%d\r\n",s_structureSPO2Data.PulseRate,s_structureSPO2Data.SPO2,s_structureSPO2Data.PI/10.0,s_structureSPO2Data.R);
	SendDataByBT(SPO2_FLAG,0);
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitSPO
* 函数功能：初始化SPO模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitSPO2(void)
{
  ConfigSPO2GPIO(); //配置SPO的GPIO
	InitADC();        //初始化ADC模块
  InitDAC();        //初始化DAC模块
}

/*********************************************************************************************************
* 函数名称：SPO2AdjTask
* 函数功能：SPO2调光任务，每100ms调用一次
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void  SPO2AdjTask(void)
{
	u16 IRDataSum = 0;
	u16 REDDataSum = 0;
	u8 i,flag;
	
	for(i=0;i<DATA_ADJ_NUM;i++)
	{
		IRDataSum += s_arrIRAdjData[i];
		REDDataSum += s_arrREDAdjData[i];
	}
	
	if(!JudgePlugIn(IRDataSum,REDDataSum))
	{
//		printf("插头未插入\r\n");
    //SEGGER_RTT_printf(0, "OFF LINE\r\n");
    online = 0;
		s_enumADJState = PLUG_OFF;
		SendDataByBT(SPO2_FLAG,1);
		return;
	}
	
	flag = JudgeFingerIn(IRDataSum,REDDataSum);
	if(flag == 2)
	{
//		printf("手指未稳定\r\n");
    online = 1;
    figer = 1;
    //SEGGER_RTT_printf(0, "FIGER UNSTABLE\r\n");
		s_enumADJState = FINGER_UNSTABLE;
		SendDataByBT(SPO2_FLAG,1);
		return;
	}
	else if(flag == 0)
	{
//		printf("手指未放入\r\n");
    online = 1;
    figer = 0;
    //SEGGER_RTT_printf(0, "NO FIGER\r\n");
		ResetAdjust();
		s_enumADJState = FINGER_OFF;
		SendDataByBT(SPO2_FLAG,1);
		return;
	}
	if(JudgeIfRoughAdj(IRDataSum,REDDataSum) == 0)
	{
    online = 1;
    figer = 1;
//		printf("粗调光中\r\n");
    //SEGGER_RTT_printf(0, "cu tiao guang\r\n");
		s_enumADJState = ADJUSTING;
		SendDataByBT(SPO2_FLAG,1);
		return;
	}
	
	if(JudgeIfResetAdj(IRDataSum,REDDataSum))
	{
    online = 1;
    figer = 1;
//		printf("复位调光\r\n");
    //SEGGER_RTT_printf(0, "reset tiao guang\r\n");
		ResetAdjust();
		s_enumADJState = ADJUSTING;
		SendDataByBT(SPO2_FLAG,1);
		return;
	}

	if(JudgeIfFineAdj(IRDataSum,REDDataSum))
	{
    online = 1;
    figer = 1;
//		printf("微调光\r\n");
    //SEGGER_RTT_printf(0, "light tiao guang\r\n");
	}
	s_enumADJState = FINE_ADJUST;
}

/*********************************************************************************************************
* 函数名称：SPO2Task
* 函数功能：SPO2任务，每1ms调用一次
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u16 SPO2Task(void)
{
	static u8 s_iCnt9 = 0;
	static u8 s_iREDCnt30 = 0, s_iIRCnt30 = 0;
	static u16 s_iDataNum = 0;
	static u16 buf;

	switch(s_iCnt9)
	{
		case(1):		//开启红灯
			CLR_IR;
			SET_RED;
			SET_CAP;
		  SetDAC(s_iREDDAVal);
      //SEGGER_RTT_printf(0, "red on\r\n");
		break;
		case(2):		//ADC采样
			s_arrREDAdjData[s_iDataNum] = GetADC();	
			buf = Filter(s_arrREDAdjData[s_iDataNum],RED_FLAG);
		//printf("%d %d",s_arrREDAdjData[s_iDataNum],buf);
    //SEGGER_RTT_printf(0, "RD - %d %d\r\n",s_arrREDAdjData[s_iDataNum],buf);
		break;		
		case(3):		//关灯
			CLR_IR;
			CLR_RED;
			CLR_CAP;		  
			if(s_iREDAdjFlag + s_iIRAdjFlag == 2 && s_iREDCnt30 >= 30)
			{
				s_arrREDCalData[s_iREDCalCnt++] = buf;
				if(s_iREDCalCnt == 1)
				{
					s_iREDTime[0] = GetSysTime();
				}
				else if(s_iREDCalCnt >= DATA_CAL_NUM)
				{
					s_iREDCalCnt = 0;
					s_iREDTime[1] = GetSysTime();
					HandleData(RED_FLAG);
				}		
			}
			else if(s_iREDAdjFlag + s_iIRAdjFlag == 2 && s_iREDCnt30 < 30)
			{
				s_iREDCnt30++;
			}
			else
			{
				s_iREDCnt30 = 0;
			}
		break;
		case(5):		//开启红外光
			SET_IR;
			CLR_RED;
			SET_CAP;
			SetDAC(s_iIRDAVal);
    //SEGGER_RTT_printf(0, "ir on\r\n");
		break;
		case(6):		//ADC采样
			s_arrIRAdjData[s_iDataNum++] = GetADC();
			buf = Filter(s_arrIRAdjData[s_iDataNum-1],IR_FLAG);
			//printf(" %d %d\r\n",s_arrIRAdjData[s_iDataNum-1],buf);
      //SEGGER_RTT_printf(0, "IR - %d %d\r\n",s_arrIRAdjData[s_iDataNum-1],buf);
		  s_iWaveBuf = buf;		
		break;
		case(7):		//关灯
			CLR_IR;
			CLR_RED;
			CLR_CAP;

			//printf("buf = %d\r\n",buf);
			if(s_iREDAdjFlag + s_iIRAdjFlag == 2 && s_iIRCnt30 >= 30)
			{
				SendDataByBT(IR_FLAG,buf);
				s_arrIRCalData[s_iIRCalCnt++] = buf;
				if(s_iIRCalCnt == 1)
				{
					s_iIRTime[0] = GetSysTime();
				}
				else if(s_iIRCalCnt >= DATA_CAL_NUM)
				{
					s_iIRCalCnt = 0;
					s_iIRTime[1] = GetSysTime();
					HandleData(IR_FLAG);
				}
			}
			else if(s_iREDAdjFlag + s_iIRAdjFlag == 2 && s_iIRCnt30 < 30)
			{
				s_iIRCnt30++;
			}
			else
			{
				s_iIRCnt30 = 0;
			}			
			if(s_iDataNum >= DATA_ADJ_NUM)
			{
				s_iDataNum = 0;
			}				
		break;
		default:
		break;
	}
	
	s_iCnt9++;
	if(s_iCnt9 >= 9)
	{
		s_iCnt9 = 0;
	}	
	//CalData();
	return buf;
}

/*********************************************************************************************************
* 函数名称：GetSPO2Data
* 函数功能：获取血氧等数据
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GetSPO2Data(StructSPO2Data* data)
{
	if(s_structureSPO2Data.PulseRate != 0)
	{
		data->PI = s_structureSPO2Data.PI;
		data->PulseRate = s_structureSPO2Data.PulseRate;
		data->R = s_structureSPO2Data.R;
		data->SPO2 = s_structureSPO2Data.SPO2;
	}
	else
	{
		data->PI = -1;
		data->PulseRate = -1;
		data->R = -1;
		data->SPO2 = -1;
	}
}

/*********************************************************************************************************
* 函数名称：GetSPO2State
* 函数功能：获取血氧测量状态
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
EnumSpo2Mode GetSPO2State(void)
{
	return s_enumADJState;
}

/*********************************************************************************************************
* 函数名称：GetSPO2Wave
* 函数功能：获取波形数据
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u16   GetSPO2Wave(void)
{
	return s_iWaveBuf;
}

int group = 0;
int min_d = 65535;
int max_d = 0;
int min = 0;
int max = 65535;
double needavg;
int retmin = 1000;
int retmax = 0;
u16   GetSPO2Wave_Prossed(void)
{
  group++;
  if(group>= 1000){
    group = 0;
    retmin = 1000;
    retmax = 0;
    if(needavg || (retmax-retmin) <=200){
      needavg = 0;
      min = min_d - 300;
      max = max_d + 300;
      min_d = 65535;
      max_d = 0;
    }
  }
  
  if(s_iWaveBuf < min_d && s_iWaveBuf>2000){
    min_d = s_iWaveBuf;
  }
  if(s_iWaveBuf > max_d && s_iWaveBuf < 60000){
    max_d = s_iWaveBuf;
  }
  
  if(s_iWaveBuf < min || s_iWaveBuf > max){
    needavg = 1;
  }
  
  int dat = s_iWaveBuf;
  int dec = max-min;
  dat = dat - min;
  if(dat < 0){
    return 0;
  }
  if(dat > dec){
    return 1000;
  }
  
  dat = 1000*dat/dec;
  
  if(dat < retmin){
    retmin = dat;
  }
  if(dat > retmax){
    retmax = dat;
  }
	return dat;
}

/*********************************************************************************************************
* 函数名称：SendDataByBT
* 函数功能：通过蓝牙发送数据
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void  SendDataByBT(u8 flag, u16 data)
{
    return ;
}
