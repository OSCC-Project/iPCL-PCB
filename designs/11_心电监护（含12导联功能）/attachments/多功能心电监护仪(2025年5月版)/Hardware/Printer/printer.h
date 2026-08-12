#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"
#include "systick.h"
#include "SEGGER_RTT.h"
#include "stdbool.h"
#include "gd32f4xx.h"
#include <string.h>

extern bool dprintBuffer[1728];
extern bool a4buffer[800][160];
extern bool sidebar12[80][1400];

void editBuff(int place,bool value);
void initPrinter();
void paperStep();
void paperBack();
void paperStop();
void printLine();

void clearA4();
void A4_DrawString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t mode,uint8_t *p);//显示字符串
void Side_DrawString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t mode, uint8_t* p);

//缺纸检测 1-有纸 0-无纸
#define paperStatus (gpio_input_bit_get(GPIOC,GPIO_PIN_5) == RESET)?1:0

//STB控制
#define STB1_OFF gpio_bit_write(GPIOG,GPIO_PIN_4,SET)
#define STB2_OFF gpio_bit_write(GPIOG,GPIO_PIN_3,SET)
#define STB3_OFF gpio_bit_write(GPIOG,GPIO_PIN_15,SET)
#define STB4_OFF gpio_bit_write(GPIOG,GPIO_PIN_14,SET)
#define STB1_ON gpio_bit_write(GPIOG,GPIO_PIN_4,RESET)
#define STB2_ON gpio_bit_write(GPIOG,GPIO_PIN_3,RESET)
#define STB3_ON gpio_bit_write(GPIOG,GPIO_PIN_15,RESET)
#define STB4_ON gpio_bit_write(GPIOG,GPIO_PIN_14,RESET)
//LAT控制
#define LAT_OFF gpio_bit_write(GPIOG,GPIO_PIN_5,RESET)
#define LAT_ON gpio_bit_write(GPIOG,GPIO_PIN_5,SET)
//DATA控制
#define DI1_H gpio_bit_write(GPIOG,GPIO_PIN_13,SET)
#define DI2_H gpio_bit_write(GPIOB,GPIO_PIN_7,SET)
#define CLK_H gpio_bit_write(GPIOB,GPIO_PIN_6,SET)
#define DI1_L gpio_bit_write(GPIOG,GPIO_PIN_13,RESET)
#define DI2_L gpio_bit_write(GPIOB,GPIO_PIN_7,RESET)
#define CLK_L gpio_bit_write(GPIOB,GPIO_PIN_6,RESET)
//12V控制
#define PrintPower_ON gpio_bit_write(GPIOA,GPIO_PIN_15,SET)
#define PrintPower_OFF gpio_bit_write(GPIOA,GPIO_PIN_5,RESET)

#endif
