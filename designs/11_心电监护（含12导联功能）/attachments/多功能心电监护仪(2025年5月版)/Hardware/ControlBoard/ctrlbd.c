#include "ctrlbd.h"
#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"
#include "systick.h"
#include "SEGGER_RTT.h"
#include "stdbool.h"
#include "AppManager.h"
//int keyiost = 0;

//按键使用中断
int16_t Encoder_Count, B_level, Cnt;


void initKeyBoard() {
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_SYSCFG);
    //模式选择开关
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    //确认按钮
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    //行 高电平
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_14);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_bit_write(GPIOF, GPIO_PIN_13, SET);
    gpio_bit_write(GPIOF, GPIO_PIN_14, SET);
    gpio_bit_write(GPIOF, GPIO_PIN_15, SET);
    gpio_bit_write(GPIOG, GPIO_PIN_0, SET);
    //列 读取
    gpio_mode_set(GPIOG, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    //电位器
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    SEGGER_RTT_printf(0, "Key GPIO inited\r\n");

    nvic_irq_enable(EXTI4_IRQn, 3, 3);
    nvic_irq_enable(EXTI5_9_IRQn, 3, 3);
    nvic_irq_enable(EXTI10_15_IRQn, 3, 3);

    syscfg_exti_line_config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN4);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN9);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN14);

    exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(EXTI_9, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    //keyiost = 1;
}

bool scr = 1;
void EXTI4_IRQHandler() {
    if(exti_interrupt_flag_get(EXTI_4) == SET) { // 中断标志位为1，按键按下
      if(gpio_input_bit_get(GPIOC,GPIO_PIN_4) == 1){
        int keys = getKey();
        rowHIGH();
        switch(keys) {
        case 0:
            break;
        case 1:
            SEGGER_RTT_printf(0, "Key - A\r\n");
            break;
        case 2:
            SEGGER_RTT_printf(0, "Key - B\r\n");
            break;
        case 3:
            SEGGER_RTT_printf(0, "Key - C\r\n");
            break;
        case 4:
            SEGGER_RTT_printf(0, "Key - D\r\n");
            break;
        case 5:
            SEGGER_RTT_printf(0, "Key - E\r\n");
            break;
        case 6:
            SEGGER_RTT_printf(0, "Key - F\r\n");
            break;
        case 7:
            SEGGER_RTT_printf(0, "Key - ScreenToggle,scr=%d\r\n",scr);
              scr = !scr;
              gpio_bit_write(GPIOD,GPIO_PIN_13,!scr);
            break;
        case 8:
            SEGGER_RTT_printf(0, "Key - Charge\r\n");
            break;
        case 9:
            SEGGER_RTT_printf(0, "Key - Energy 50J\r\n");
            break;
        case 10:
            SEGGER_RTT_printf(0, "Key - Energy 150J\r\n");
            break;
        case 11:
            SEGGER_RTT_printf(0, "Key - Energy 200J\r\n");
            break;
        case 12:
            SEGGER_RTT_printf(0, "Key - Energy 3600J\r\n");
            break;
        case 13:
            SEGGER_RTT_printf(0, "Key - Confirm\r\n");
            break;
        }
        if((keys !=7 )&&(keys !=0)){
          keyHandler(keys);
        }
      }else{
        delay_1ms(10);
        int jk = getKey();
        rowHIGH();
        if(jk != 0){
          delay_1ms(10);
        }
        delay_1ms(10);
        
      }

        exti_interrupt_flag_clear(EXTI_4); // 清中断标志位
    }
}

void EXTI5_9_IRQHandler() {
    if(exti_interrupt_flag_get(EXTI_9) == SET) { // 中断标志位为1，按键按下
        //SEGGER_RTT_printf(0, "CFM\r\n");
        exti_interrupt_flag_clear(EXTI_9); // 清中断标志位
    }
}


bool stat = 0;
void EXTI10_15_IRQHandler() {
    if(exti_interrupt_flag_get(EXTI_14) == SET) { // 中断标志位为1，按键按下
        if(gpio_input_bit_get(GPIOE, GPIO_PIN_14) == 0 && Cnt == 0) { //A相下降沿触发第一次中断
            Cnt++;//计数值加一，表示已经触发了第一次中断
            B_level = 0; //读取B相电平，若为高电平则B_level置1，反之保持0
            if(gpio_input_bit_get(GPIOE, GPIO_PIN_15) == 1) {
                B_level = 1;
            }
        }
        if(gpio_input_bit_get(GPIOE, GPIO_PIN_14) == 1 && Cnt == 1) { //A相上升沿触发第二次中断
            Cnt = 0; //计数清零
            if(B_level == 1 && gpio_input_bit_get(GPIOE, GPIO_PIN_15) == 0) {
                Encoder_Count++;//正转
              //SEGGER_RTT_printf(0, "+\r\n");
            }
            if(B_level == 0 && gpio_input_bit_get(GPIOE, GPIO_PIN_15) == 1) {
                Encoder_Count--;//反转
              //SEGGER_RTT_printf(0, "-\r\n");
            }
        }

        exti_interrupt_flag_clear(EXTI_14); // 清中断标志位
    }
}

/* 0-监护 1-除颤 2-十二导联 3-系统*/
int getMode() {

}

/*0.无 1.A 2.B 3.C 4.D 5.E 6.F 7.SCR 8.充电 9. 50J 10. 150J 11. 200J 12. 360J 13.确认*/
int getKey() {
    rowHIGH();
    delay_1ms(5);
    if(gpio_input_bit_get(GPIOE, GPIO_PIN_7) == SET) { //B,E,50J,360J
        //SEGGER_RTT_printf(0, "BE53 trigger\r\n");
        lineHIGH();
        //delay_1ms(5);
        if(gpio_input_bit_get(GPIOF, GPIO_PIN_13) == SET) { //ABC
            return 2;
        } else if(gpio_input_bit_get(GPIOF, GPIO_PIN_14) == SET) { //DEF
            return 5;
        } else if(gpio_input_bit_get(GPIOF, GPIO_PIN_15) == SET) { //充电 50J 150J
            return 9;
        } else if(gpio_input_bit_get(GPIOG, GPIO_PIN_0) == SET) { //200J 360J SCR
            return 12;
        }
    } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_8) == SET) { //C,F,150J,SCR
        //SEGGER_RTT_printf(0, "CF1S trigger\r\n");
        lineHIGH();
        //delay_1ms(5);
        if(gpio_input_bit_get(GPIOF, GPIO_PIN_13) == SET) { //ABC
            return 3;
        } else if(gpio_input_bit_get(GPIOF, GPIO_PIN_14) == SET) { //DEF
            return 6;
        } else if(gpio_input_bit_get(GPIOF, GPIO_PIN_15) == SET) { //充电 50J 150J
            return 10;
        } else if(gpio_input_bit_get(GPIOG, GPIO_PIN_0) == SET) { //200J 360J SCR
            return 7;
        }
    } else if(gpio_input_bit_get(GPIOG, GPIO_PIN_1) == SET) { //A,D,充电,200J
        //SEGGER_RTT_printf(0, "ADCH2 trigger\r\n");
        lineHIGH();
        //delay_1ms(5);
        if(gpio_input_bit_get(GPIOF, GPIO_PIN_13) == SET) { //ABC
            return 1;
        } else if(gpio_input_bit_get(GPIOF, GPIO_PIN_14) == SET) { //DEF
            return 4;
        } else if(gpio_input_bit_get(GPIOF, GPIO_PIN_15) == SET) { //充电 50J 150J
            return 8;
        } else if(gpio_input_bit_get(GPIOG, GPIO_PIN_0) == SET) { //200J 360J SCR
            return 11;
        }
    }
    return 0;
}

//电位器读取 0-无状态 1-左转 2-右转
int getScoll() {
}

//行线高电平，列读取
void rowHIGH() {
    //if(keyiost == 1) {
    //    return;
    //}
    gpio_bit_write(GPIOG, GPIO_PIN_1, RESET);
    gpio_bit_write(GPIOE, GPIO_PIN_7, RESET);
    gpio_bit_write(GPIOE, GPIO_PIN_8, RESET);
    gpio_mode_set(GPIOG, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_bit_write(GPIOF, GPIO_PIN_13, SET);
    gpio_bit_write(GPIOF, GPIO_PIN_14, SET);
    gpio_bit_write(GPIOF, GPIO_PIN_15, SET);
    gpio_bit_write(GPIOG, GPIO_PIN_0, SET);
    //keyiost = 1;
}

//列线高电平，行线读取
void lineHIGH() {
    //if(keyiost == 0) {
    //    return;
    //}
    gpio_bit_write(GPIOF, GPIO_PIN_13, RESET);
    gpio_bit_write(GPIOF, GPIO_PIN_14, RESET);
    gpio_bit_write(GPIOF, GPIO_PIN_15, RESET);
    gpio_bit_write(GPIOG, GPIO_PIN_0, RESET);
    gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_mode_set(GPIOG, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_bit_write(GPIOG, GPIO_PIN_1, SET);
    gpio_bit_write(GPIOE, GPIO_PIN_7, SET);
    gpio_bit_write(GPIOE, GPIO_PIN_8, SET);
    //keyiost = 0;
}