#include "printer.h"
#include "screen_basic.h"

int step = 0;
int back = 0;
bool dprintBuffer[1728] = {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0};
bool a4buffer[800][160];//8dot为水平1mm 80为1cm A4取整数宽度30cm

bool steppaper = 1;

//#define nofast = 1

void editBuff(int place, bool value) {
    dprintBuffer[place] = value;
}

void initPrinter() {
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOA);
    //进纸电机
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    paperStop();
    //缺纸检测
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    //STB初始化
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    STB1_OFF;
    STB2_OFF;
    STB3_OFF;
    STB4_OFF;
    //12V
    //gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    //gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    //PrintPower_OFF;
    //LAT初始化
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    LAT_OFF;
    //数据IO初始化
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);//DI1
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);//DI2
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);//CLK
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    DI1_L;
    DI2_L;
    CLK_L;
}

/*
    for(int i = 0; i < 1728; i++) {
        bool in2 = dprintBuffer[i * 2];
        bool in22 = dprintBuffer[i * 2 + 1];
        bool in1 = dprintBuffer[865 + (i * 2)];
        bool in12 = dprintBuffer[865 + (i * 2) + 1];
        if(in1) {
            DI1_H;
            //SEGGER_RTT_printf(0,"DI1 = HIGH\r\n");
        } else {
            DI1_L;
            //SEGGER_RTT_printf(0,"DI1 = LOW\r\n");
        }
        if(in2) {
            DI2_H;
            //SEGGER_RTT_printf(0,"DI2 = HIGH\r\n");
        } else {
            DI2_L;
            //SEGGER_RTT_printf(0,"DI2 = LOW\r\n");
        }
        delay_us(1);
        CLK_H;
        delay_us(1);
        if(in12) {
            DI1_H;
            //SEGGER_RTT_printf(0,"DI12 = HIGH\r\n");
        } else {
            DI1_L;
            //SEGGER_RTT_printf(0,"DI12 = LOW\r\n");
        }
        if(in22) {
            DI2_H;
            //SEGGER_RTT_printf(0,"DI22 = HIGH\r\n");
        } else {
            DI2_L;
            //SEGGER_RTT_printf(0,"DI22 = LOW\r\n");
        }
        delay_us(1);
        CLK_L;
    }
*/
int duty = 1000;
int black = 0;
bool mode = 0;
bool dbl = 0;

void printLine() {
    paperStop();
    //paperStep();
    //LAT_OFF;
    //CLK_L;
    //delay_us(1);
    black = 0;
    mode = 0;
    dbl = 0;


    //每次循环给每个DI发送1帧数据，共两个数据输入引脚，每次发送2点数据，共发送432次，1728点
    for(int i = 0; i < 865; i++) {
        bool in2 = dprintBuffer[i];
        bool in1 = dprintBuffer[864 + i];
        if(in1) {
            black++;
            DI1_H;
            //SEGGER_RTT_printf(0,"DI1 = HIGH\r\n");
        } else {
            DI1_L;
            //SEGGER_RTT_printf(0,"DI1 = LOW\r\n");
        }
        if(in2) {
            DI2_H;
            black++;
            //SEGGER_RTT_printf(0,"DI2 = HIGH\r\n");
        } else {
            DI2_L;
            //SEGGER_RTT_printf(0,"DI2 = LOW\r\n");
        }

        CLK_H;
        delay_us(1);
        CLK_L;
        delay_us(1);
    }

    if(black <= 20) {
        duty = 130;
        mode = 1;
    } else if(black <= 35) {
        duty = 300;
        mode = 1;
    } else if(black <= 50) {
        duty = 620;
        mode = 1;
    } else if(black <= 100) {
        duty = 1800;
    } else if(black <= 150) {
        duty = 2500;
    } else if(black <= 200) {
        duty = 3600;
    } else if(black <= 300) {
        duty = 4500;
    } else if(black <= 400) {
        duty = 5000;
        dbl = 1;
    } else {
        duty = 6000;
        dbl = 1;
    }


    //锁存与加热
    LAT_OFF;
    delay_us(1);
    LAT_ON;
    delay_us(1);
    LAT_OFF;
    delay_us(1);
    delay_us(1);

#ifndef nofast
    if(mode) {
        delay_us(100);
        delay_us(duty);
        STB1_ON;
        STB2_ON;
        STB3_ON;
        STB4_ON;
        delay_us(1500);
        STB4_OFF;
        STB3_OFF;
        STB2_OFF;
        STB1_OFF;
        delay_us(duty);
        //SEGGER_RTT_printf(0,"HIGH SPEED MODE\r\n");
    } else {
#endif
        delay_us(5800);
        STB1_ON;
        delay_us(1700);
        STB1_OFF;
        delay_us(duty);
        STB2_ON;
        delay_us(1700);
        STB2_OFF;
        delay_us(duty);
        STB3_ON;
        delay_us(1700);
        STB3_OFF;
        delay_us(duty);
        STB4_ON;
        delay_us(1700);
        STB4_OFF;
#ifndef nofast
    }
#endif

    if(dbl) {
        delay_us(3000);
        STB1_ON;
        delay_us(1650);
        STB1_OFF;
        delay_us(duty + 200);
        STB2_ON;
        delay_us(1650);
        STB2_OFF;
        delay_us(duty + 200);
        STB3_ON;
        delay_us(1650);
        STB3_OFF;
        delay_us(duty + 200);
        STB4_ON;
        delay_us(1650);
        STB4_OFF;
    }

    delay_us(duty);
    LAT_ON;
    delay_us(1);
    LAT_OFF;
    delay_us(1);
    LAT_ON;
//走纸一步
    if(steppaper) {
        if(!mode && black<=500){
            delay_us(1500);
        }
        if(black >250){
            delay_us(3000);
        }
        paperStep();
        if(mode) {
            delay_us(1400);
        } else if(black <= 250) {
            delay_us(1800);
        } else {
            delay_us(2500);
        }

        paperStop();
    }

}

/*
    delay_us(8000);
    STB1_ON;
    delay_us(1800);
    STB1_OFF;
    delay_us(1000);
    STB2_ON;
    delay_us(1800);
    STB2_OFF;
    delay_us(1000);
    STB3_ON;
    delay_us(1800);
    STB3_OFF;
    delay_us(1000);
    STB4_ON;
    delay_us(1800);
    STB4_OFF;
*/

void paperBack() { //SS RS RR
    switch(step) {
    case 0:
        step++;
        gpio_bit_write(GPIOB, GPIO_PIN_1, RESET); //PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, RESET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    case 1:
        step++;
        gpio_bit_write(GPIOB, GPIO_PIN_1, SET);//PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, RESET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    case 2:
        step++;
        gpio_bit_write(GPIOB, GPIO_PIN_1, SET);//PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, SET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    case 3:
        step = 0;
        gpio_bit_write(GPIOB, GPIO_PIN_1, RESET);//PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, SET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    }
    //gpio_bit_write(GPIOB,GPIO_PIN_1,RESET);
    //gpio_bit_write(GPIOF,GPIO_PIN_11,RESET);
    //gpio_bit_write(GPIOB,GPIO_PIN_2,RESET);
    //gpio_bit_write(GPIOF,GPIO_PIN_12,SET);
    //SEGGER_RTT_printf(0, "%d\r\n",step);
    //delay_us(3000);
    //gpio_bit_write(GPIOB,GPIO_PIN_2,RESET);
    //gpio_bit_write(GPIOF,GPIO_PIN_12,RESET);
}

void paperStep() {
    switch(back) {
    case 3:
        back = 0;
        gpio_bit_write(GPIOB, GPIO_PIN_1, RESET); //PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, RESET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    case 2:
        back++;
        gpio_bit_write(GPIOB, GPIO_PIN_1, SET);//PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, RESET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    case 1:
        back++;
        gpio_bit_write(GPIOB, GPIO_PIN_1, SET);//PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, SET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    case 0:
        back++;
        gpio_bit_write(GPIOB, GPIO_PIN_1, RESET);//PHA
        gpio_bit_write(GPIOB, GPIO_PIN_2, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_11, SET); //PHB
        gpio_bit_write(GPIOF, GPIO_PIN_12, SET);
        break;
    }
}

void paperStop() {
    gpio_bit_write(GPIOB, GPIO_PIN_2, RESET);
    gpio_bit_write(GPIOF, GPIO_PIN_12, RESET);
}


//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void A4_DrawString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t mode, uint8_t* p) {
    uint8_t x0 = x;
    width += x;
    height += y;
    uint8_t nsize;
    if(size == 121) {
        nsize = 12;
    } else if(size == 161) {
        nsize = 16;
    } else if(size == 241) {
        nsize = 24;
    } else {
        nsize = size;
    }
    while((*p <= '~') && (*p >= ' ')) { //判断是不是非法字符!

        if(x >= width) {
            x = x0;
            y += nsize;
        }
        if(y >= height)break; //退出
        A4_DrawChar(x, y, *p, size, mode);
        if(size > 100) {
            x += nsize;
        } else {
            x += nsize / 2;
        }
        p++;
    }
}

void clearA4() {
    memset(&a4buffer, 0, sizeof a4buffer);
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void Side_DrawString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t mode, uint8_t* p) {
    uint8_t x0 = x;
    width += x;
    height += y;
    uint8_t nsize;
    if(size == 121) {
        nsize = 12;
    } else if(size == 161) {
        nsize = 16;
    } else if(size == 241) {
        nsize = 24;
    } else {
        nsize = size;
    }
    while((*p <= '~') && (*p >= ' ')) { //判断是不是非法字符!

        if(x >= width) {
            x = x0;
            y += nsize;
        }
        if(y >= height)break; //退出
        Side_DrawChar(x, y, *p, size, mode);
        if(size > 100) {
            x += nsize;
        } else {
            x += nsize / 2;
        }
        p++;
    }
}


/*
int main(void) {
    systick_config();//12导联打印10s 标准速度，标准电压
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    //SEGGER_RTT_ConfigUpBuffer(1, "JScope_I4I4I4I4", &buff[0], sizeof(buff), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    SEGGER_RTT_printf(0, "start run\r\n");
    //InitTimer();
    init12Lead();
    initPrinter();
    PrintPower_OFF;

     SEGGER_RTT_printf(0, "12lead Task start\r\n");
    ads12Lead_CS_L;
    ads12Lead_START_L;
    delay_1ms(10);
    SPI_Transport_12Lead(0x11);
    delay_1ms(300);
    uint8_t did = a12lead_REG(0x20 | 0x00, 0x00);
    SEGGER_RTT_printf(0, "id=%x\r\n", did);
    delay_us(100);
    a12lead_REG(0x40 | 0x01, 0b10000110);
  delay_us(100);
    a12lead_REG(0x40 | 0x02, 0b00110101);
  delay_us(100);
    a12lead_REG(0x40 | 0x03, 0b11001100);
  delay_us(100);
    //WCT
    a12lead_REG(0x40 | 0x18, 0b00001001); //0b00001001
  delay_us(100);
    a12lead_REG(0x40 | 0x19, 0b11011000); //0b11011000
  delay_us(100);
    //RLD
    a12lead_REG(0x40 | 0x0d, 0xff);
    delay_us(100);
    a12lead_REG(0x40 | 0x0e, 0xff);
    delay_us(100);


    a12lead_REG(0x40 | 0x05, 0b00000000);
    delay_us(100);

    SPI_Transport_12Lead(0x10);
    delay_us(100);
    SPI_Transport_12Lead(0x08);
    ads12Lead_START_H;
    delay_1ms(200);

    double bl2, bl3, bv1, bv2, bv3, bv4, bv5, bv6 = 800;
    int n, nk = 0;
    int point = 0;
    //uint8_t dat[27];
    SEGGER_RTT_printf(0, "Start record\r\n");
    while(1) {
        a12lead_GetmV();
      point++;
        if(point >= 200) {
            if(point >= (2000 + 200)) {
                PrintPower_ON;
              SEGGER_RTT_printf(0, "Start print\r\n");
                break;
            }
            pl1[point - 1-200] = fl1 / 0.0125;
            pl2[point - 1-200] = fl2 / 0.0125;
            pl3[point - 1-200] = fl3 / 0.0125;
            pavr[point - 1-200] = favr / 0.0125;
            pavl[point - 1-200] = favl / 0.0125;
            pavf[point - 1-200] = favf / 0.0125;
            pv1[point - 1-200] = fv1 / 0.0125;
            pv2[point - 1-200] = fv2 / 0.0125;
            pv3[point - 1-200] = fv3 / 0.0125;
            pv4[point - 1-200] = fv4 / 0.0125;
            pv5[point - 1-200] = fv5 / 0.0125;
            pv6[point - 1-200] = fv6 / 0.0125;
        }
        delay_1ms(5);
    }
    for(int i = 0; i < 80; i++) {
        paperStep();
        delay_us(1500);
        paperStep();
        delay_us(1500);
    }
    paperStop();
    for(int i = 1999; i > 0; i--) {
        memset(dprintBuffer, 0, sizeof dprintBuffer);
        //顶部空出20mm 160dot
        //I导联需要额外增加 120dot
        //每个等电位线相隔15mm 120dot
      SEGGER_RTT_printf(0, "Line: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",pl1[i],pl2[i],pl3[i],pavr[i],pavl[i],pavf[i],pv1[i],pv2[i],pv3[i],pv4[i],pv5[i],pv6[i]);
        int n = 0;
        if(i == 0) {
            n = 160 + 120 - pl1[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 2 - pl2[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 3 - pl3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 4 - pavr[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 5 - pavl[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 6 - pavf[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 7 - pv1[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 8 - pv2[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 9 - pv3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 10 - pv4[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 11 - pv5[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
            n = 160 + 120 * 12 - pv6[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[n] = 1;
        } else {
            //I
            if(pl1[i] > pl1[i - 1]) {
              for(int k = pl1[i - 1]; k <= pl1[i]; k++) {
                    n = 160 + 120 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pl1[i]; k <= pl1[i-1]; k++) {
                    n = 160 + 120 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //II
            if(pl2[i] > pl2[i - 1]) {
                for(int k = pl2[i - 1]; k <= pl2[i]; k++) {
                    n = 160 + 120*2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pl2[i]; k <= pl2[i-1]; k++) {
                    n = 160 + 120*2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //III
            if(pl3[i] > pl3[i - 1]) {
                for(int k = pl3[i - 1]; k <= pl3[i]; k++) {
                    n = 160 + 120*3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pl3[i]; k <= pl3[i-1]; k++) {
                    n = 160 + 120*3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //aVR
            if(pavr[i] > pavr[i - 1]) {
                for(int k = pavr[i - 1]; k <= pavr[i]; k++) {
                    n = 160 + 120*4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pavr[i]; k <= pavr[i-1]; k++) {
                    n = 160 + 120*4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //aVL
            if(pavl[i] > pavl[i - 1]) {
                for(int k = pavl[i - 1]; k <= pavl[i]; k++) {
                    n = 160 + 120*5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pavl[i]; k <= pavl[i-1]; k++) {
                    n = 160 + 120*5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //aVF
            if(pavf[i] > pavf[i - 1]) {
                for(int k = pavf[i - 1]; k <= pavf[i]; k++) {
                    n = 160 + 120*6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pavf[i]; k <= pavf[i-1]; k++) {
                    n = 160 + 120*6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }




            //v1
            if(pv1[i] > pv1[i - 1]) {
              for(int k = pv1[i - 1]; k <= pv1[i]; k++) {
                    n = 160 + 120*7 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pv1[i]; k <= pv1[i-1]; k++) {
                    n = 160 + 120*7 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //v2
            if(pv2[i] > pv2[i - 1]) {
                for(int k = pv2[i - 1]; k <= pv2[i]; k++) {
                    n = 160 + 120*8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pv2[i]; k <= pv2[i-1]; k++) {
                    n = 160 + 120*8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //v3
            if(pv3[i] > pv3[i - 1]) {
                for(int k = pv3[i - 1]; k <= pv3[i]; k++) {
                    n = 160 + 120*9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pv3[i]; k <= pv3[i-1]; k++) {
                    n = 160 + 120*9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //v4
            if(pv4[i] > pv4[i - 1]) {
                for(int k = pv4[i - 1]; k <= pv4[i]; k++) {
                    n = 160 + 120*10 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pv4[i]; k <= pv4[i-1]; k++) {
                    n = 160 + 120*10 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //v5
            if(pv5[i] > pv5[i - 1]) {
                for(int k = pv5[i - 1]; k <= pv5[i]; k++) {
                    n = 160 + 120*11 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pv5[i]; k <= pv5[i-1]; k++) {
                    n = 160 + 120*11 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
            //v6
            if(pv6[i] > pv6[i - 1]) {
                for(int k = pv6[i - 1]; k <= pv6[i]; k++) {
                    n = 160 + 120*12 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }else{
              for(int k = pv6[i]; k <= pv6[i-1]; k++) {
                    n = 160 + 120*12 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[n] = 1;
                }
            }
        }
        paperStep();
        delay_1ms(1);
        paperStop();
        printLine();
    }
    for(int i = 0; i < 400; i++) {
        paperStep();
        delay_1ms(2);
        paperStep();
        delay_1ms(2);
    }
    paperStop();
}
    */