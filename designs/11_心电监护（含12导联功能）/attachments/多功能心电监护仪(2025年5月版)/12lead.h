#include "gd32f4xx.h"

#define ads12Lead_CS_L gpio_bit_write(GPIOF, GPIO_PIN_6, RESET)
#define ads12Lead_CS_H gpio_bit_write(GPIOF, GPIO_PIN_6, SET)
#define ads12Lead_RST_L gpio_bit_write(GPIOF, GPIO_PIN_4, RESET)
#define ads12Lead_RST_H gpio_bit_write(GPIOF, GPIO_PIN_4, SET)
#define ads12Lead_START_L gpio_bit_write(GPIOF, GPIO_PIN_5, RESET)
#define ads12Lead_START_H gpio_bit_write(GPIOF, GPIO_PIN_5, SET)

void init12Lead();
uint8_t a12lead_REG(unsigned char com, unsigned data);
uint8_t SPI_Transport_12Lead(uint8_t com);
void a12lead_Read(unsigned char* data);
void a12lead_GetmV();

extern float fl1,fl2,fl3,favr,favl,favf,fv1,fv2,fv3,fv4,fv5,fv6;
