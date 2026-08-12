#include "gd32f4xx.h"

#define monecg_CS_L gpio_bit_write(GPIOE, GPIO_PIN_4, RESET)
#define monecg_CS_H gpio_bit_write(GPIOE, GPIO_PIN_4, SET)
#define monecg_RST_L gpio_bit_write(GPIOC, GPIO_PIN_13, RESET)
#define monecg_RST_H gpio_bit_write(GPIOC, GPIO_PIN_13, SET)
#define monecg_START_L gpio_bit_write(GPIOE, GPIO_PIN_3, RESET)
#define monecg_START_H gpio_bit_write(GPIOE, GPIO_PIN_3, SET)

void monecgInit();
uint8_t monecg_REG(unsigned char com, unsigned data);
uint8_t SPI_Transport_monecg(uint8_t com);

void monitor_GetmV();

extern float fl1,fl2,fl3,favr,favl,favf,fv1;