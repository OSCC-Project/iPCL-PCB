#include "gd32f4xx.h"

#define respmon_CS_L gpio_bit_write(GPIOD, GPIO_PIN_8, RESET)
#define respmon_CS_H gpio_bit_write(GPIOD, GPIO_PIN_8, SET)
#define respmon_RST_L gpio_bit_write(GPIOD, GPIO_PIN_5, RESET)
#define respmon_RST_H gpio_bit_write(GPIOD, GPIO_PIN_5, SET)
#define respmon_START_L gpio_bit_write(GPIOD, GPIO_PIN_4, RESET)
#define respmon_START_H gpio_bit_write(GPIOD, GPIO_PIN_4, SET)

void respmonInit();
uint8_t respmon_REG(unsigned char com, unsigned data);
uint8_t SPI_Transport_respmon(uint8_t com);
void getRespValue();