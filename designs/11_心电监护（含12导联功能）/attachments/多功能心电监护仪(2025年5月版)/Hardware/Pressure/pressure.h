#include "gd32f4xx.h"
#include "gd32f4xx_gpio.h"

#define SDA_OUT() gpio_mode_set(GPIOF,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_PIN_0)
#define SDA_IN() gpio_mode_set(GPIOF,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_0)
#define SDA_GET() gpio_input_bit_get(GPIOF,GPIO_PIN_0)

#define SDA(x) gpio_bit_write(GPIOF,GPIO_PIN_0,(x?SET:RESET))
#define SCL(x) gpio_bit_write(GPIOF,GPIO_PIN_1,(x?SET:RESET))

void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Ack(void);
void IIC_Send_Nack(void);
unsigned char IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t dat);
unsigned char IIC_Read_Byte(void);

float pressValue();
void initPressure();
void testReset();