#include "gd32f4xx.h"
#include "gd32f4xx_usart.h"
#include "gd32f4xx_gpio.h"

void initCOM(){
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_10);
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_15);
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_10);
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_15);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_15);

	usart_deinit(USART0);
	usart_baudrate_set(USART0,2000000);
	usart_parity_config(USART0,USART_PM_NONE);
	usart_word_length_set(USART0,USART_WL_8BIT);
	usart_stop_bit_set(USART0,USART_STB_1BIT);
	
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
	usart_enable(USART0);
}

void comChar(uint8_t ucch)
{
    usart_data_transmit(USART0, (uint8_t)ucch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
}

void comString(uint8_t *ucstr)
{
      while(ucstr && *ucstr)
      {
        comChar(*ucstr++);
      }
}