#include "pressure.h"
#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "systick.h"
#include "SEGGER_RTT.h"
#include "gd32f4xx_gpio.h"
#include "ecgjudge.h"
#include "nibpsolve.h"

extern low_pass_filter_t nbplow_pass_filter;
extern hight_pass_filter_t nbphight_pass_filter;

void initPressure() {
    rcu_periph_clock_enable(RCU_GPIOF);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    
    pressValue();
}

void testReset(){
    args = 0;
    overmemory = 0;
    ct = 0;
    Init_lowPass_alpha(&nbplow_pass_filter, 0.005f, 1.2);//2.5
    Init_hightPass_alpha(&nbphight_pass_filter, 0.005f, 0.1);
    gpio_bit_write(GPIOE, GPIO_PIN_1, SET);
    gpio_bit_write(GPIOE, GPIO_PIN_0, RESET);
}

float pressValue() {
    uint8_t status = 0;
    uint8_t press1 = 0;
    uint8_t press2 = 0;
    uint8_t press3 = 0;
    uint16_t dat = 0;
    float presscalc = 300;
    IIC_Start();
    IIC_Send_Byte(0xA0);
    if(IIC_Wait_Ack() == 1) {
        SEGGER_RTT_printf(0, "can't read pressure0xA0\r\n");
      IIC_Stop();
    }
    IIC_Send_Byte(0x00);
    if(IIC_Wait_Ack() == 1) {
        SEGGER_RTT_printf(0, "can't read pressure0x00\r\n");
      IIC_Stop();
    }
    IIC_Start();
    
    IIC_Send_Byte(0xA1);
    if(IIC_Wait_Ack() == 1) {
        SEGGER_RTT_printf(0, "can't read pressure0xA1\r\n");
      IIC_Stop();
    }
    press1 = IIC_Read_Byte();
    IIC_Send_Ack();
    press2 = IIC_Read_Byte();
    IIC_Send_Ack();
    press3 = IIC_Read_Byte();
    IIC_Send_Nack();
    IIC_Stop();
    dat = press1<<8;
    dat = dat | press2;
    presscalc = ((float)dat-2000)*75/100;
    //SEGGER_RTT_printf(0, "status = %x\r\n",status);
    //SEGGER_RTT_pdrintf(0, "pressureraw = %x\r\n",dat);
    //SEGGER_RTT_printf(0, "pressurecalced = %d mmHg\r\n",presscalc);
    return presscalc;
}

//IIC起始信号
void IIC_Start(void) {
    SDA_OUT();//设置SDA为输出模式
    SDA(1);
    SCL(1);
    delay_us(5);
    SDA(0);
    delay_us(5);
    SCL(0);
    delay_us(5);
}

//IIC停止信号
void IIC_Stop(void) {
    SDA_OUT();
    SCL(0);
    SDA(0);
    SCL(1);
    delay_us(5);
    SDA(1);
    delay_us(5);
}

//IIC发送应答
void IIC_Send_Ack(void) {
    SDA_OUT();
    SCL(0);
    SDA(1);
    SDA(0);
    SCL(1);
    delay_us(5);
    SCL(0);
    SDA(1);
}

//IIC发送非应答
void IIC_Send_Nack(void) {
    SDA_OUT();
    SCL(0);
    SDA(0);
    SDA(1);
    SCL(1);
    delay_us(5);
    SCL(0);
    SDA(0);
}

//IIC等待应答
//应答返回0  非应答返回1
unsigned char IIC_Wait_Ack(void) {
    char ack = 0;
    unsigned char ack_flag = 10;
    SCL(0);
    SDA(1);
    SDA_IN();
    delay_us(5);
    SCL(1);
    delay_us(5);

    while((SDA_GET() == 1)  && (ack_flag)) {
        ack_flag--;
        delay_us(5);
    }

    //非应答
    if(ack_flag <= 0) {
        IIC_Stop();
        return 1;
    } else { //应答
        SCL(0);
        SDA_OUT();
    }
    return ack;
}


//发送一个字节
void IIC_Send_Byte(uint8_t dat) {
    int i = 0;
    SDA_OUT();
    SCL(0);
    for(i = 0; i < 8; i++) {
        SDA((dat & 0x80) >> 7);
        delay_us(1);
        SCL(1);
        delay_us(5);
        SCL(0);
        delay_us(5);
        dat <<= 1;
    }
}

//接收一个字节
unsigned char IIC_Read_Byte(void) {
    unsigned char i, receive = 0;
    SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++) {
        SCL(0);
        delay_us(5);
        SCL(1);
        delay_us(5);
        receive <<= 1;
        if(SDA_GET()) {
            receive |= 1;
        }
    }
    SCL(0);
    return receive;
}
