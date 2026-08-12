#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f4xx_gpio.h"
#include "12lead.h"
#include "ctrlbd.h"
#include "pressure.h"
#include "screen_basic.h"
#include "printer.h"
#include "SEGGER_RTT.h"
#include "SPO2.h"
#include "Timer.h"
#include "sdcard.h"
#include "diskio.h"
#include "ff.h"
#include "ecg_monitor.h"
#include "resp_monitor.h"
#include <String.h>
#include "12leadapp.h"
#include "AppManager.h"
#include "bsp_rtc.h"
#include "ecgjudge.h"
#include "nibpsolve.h"
#include "uart.h"
//#include "FreeRTOS.h"
//#include "task.h"


extern float nibp;
int val;



FATFS fs;
sd_card_info_struct sd_cardinfo;
void sd_fatfs_init(void);
void card_info_get(void);
uint8_t POINT[5] = {WHITE, BLUE, RED, YELLOW, GREEN};
uint8_t BACK[11] = {RED, YELLOW, BLUE, GREEN, WHITE, BLACK, CYAN, BROWN, LIGHTGREEN, PINK, ORANGE};
extern bool dprintBuffer[1728];


//字体编码为UTF-8 Font encode type = UTF-8
//文档观看预防针：该项目已弃用FreeRTOS，且部分表示非常奇葩，阅读会比（难）较（如）困（登）难（天），祝您好运
/*0.无 1.A 2.B 3.C 4.D 5.E 6.F 7.SCR 8.充电 9. 50J 10. 150J 11. 200J 12. 360J 13.确认*/
//8个点 = 1mm 10mm/mV 1mV需要 80点，0.1mV需要8个点，1个点对应着0.0125mV
//25mm/s速度，1s共需要走200点，一个点为0.005s，即为5ms 一页为10s，共需要2000点
//© C! Henan Tian,J S for competition (CASTIC) 410-71
//参赛作品，作者 河南省 Tian.J S 410-71 其余任何人不得将该代码用于参赛，以免扰乱比赛秩序，违反社会公德，破坏竞赛平衡，违反法律法规，侵犯个人知识产权。
int main(void) {
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();



    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "start run\r\n");

    initPrinter();
		initCOM();
		comString("[$LOG$]Booting...\r\n");

    LCD_Init();
		comString("[$LOG$]LCD - OK\r\n");

    POINT_COLOR = RED;
    LCD_DrawRectangle(229 - 6, 199, 229 + 24 * 13, 199 + 24 + 2, BLUE);
    LCD_ShowString(1, 230, 200, 480, 80, 24, 1, "Waiting for Hardware Init");

    LCD_ShowString(1, 2, 480 - 18 - 16, 480, 80, 16, 1, "Designed by HN Tian.J S CASTIC work. All rights reserved.");
    LCD_ShowString(1, 2, 480 - 18, 800, 800, 16, 1, "410-71 Any OTHERs Can NOT use this work for competition or graduate");
		comString("[$LOG$]LCD - LIGHT\r\n");
    initKeyBoard();
		comString("[$LOG$]KeyBoard - OK\r\n");
		
    rtc_config();
    InitTimer();
		comString("[$LOG$]RTC,Timer - OK\r\n");

    init12Lead();
		comString("[$LOG$]12Lead - OK\r\n");

    InitSPO2();
		comString("[$LOG$]SpO2 - OK\r\n");

    initPressure();
		comString("[$LOG$]Pressure - OK\r\n");
    monecgInit();
		comString("[$LOG$]MonitorECG - OK\r\n");
    respmonInit();
		comString("[$LOG$]RESP - OK\r\n");

		comString("[$LOG$]CLEAR SCREEN\r\n");
    LCD_Clear(1, WHITE);
    LCD_DrawRectangle(229 - 6, 199, 229 + 24 * 13, 199 + 24 + 2, BLUE);
    LCD_ShowString(1, 230, 200, 480, 80, 24, 1, "Calling System Software");
    LCD_ShowString(1, 2, 480 - 18 - 16, 480, 80, 16, 1, "Designed by HN Tian.J S CASTIC work. All rights reserved.");
    LCD_ShowString(1, 2, 480 - 18, 800, 800, 16, 1, "410-71 Any OTHERs Can NOT use this work for competition or graduate");
    //RtcTimeConfig(0x24,0x08,0x5,0x01,0x15,0x24,0x00);
		comString("[$LOG$]Calling Software\r\n");

    while(1) {
        App_Core();
				comString("[$ERR$]App Manager STOPPED\r\n");
        SEGGER_RTT_printf(0, "App Manager STOPED by ACCIDENT\r\n");
    }
}

/*
int main(void) {
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "start run\r\n");

    LCD_Init();

    while(1) {//3b 55 8b 93
        for(int i=0;i<0xFF;i++)
        {
            POINT_COLOR=BLUE;
            //BACK_COLOR=BACK[i];

            LCD_Clear(1,i);
            char txt[] ="The Color Code is 0x";
            char buffs[30];
            sprintf(buffs,"%s%X",txt,i);
            LCD_ShowString(1,30,50,480,80,24,1,buffs);
            //LTDC_Draw_Point(1,600,100,0x01);
            delay_1ms(400);
        }
        for(int i=0;i<12;i++)
        {
            POINT_COLOR=BLUE;
            BACK_COLOR=BACK[i];

            LCD_Clear(1,BACK_COLOR);
            LCD_ShowString(1,30,50,480,80,24,1,"Test Text 123...");
            //LTDC_Draw_Point(1,600,100,0x01);
            delay_1ms(700);
        }
    }
}
*/

void card_info_get(void) {
    uint8_t sd_spec, sd_spec3, sd_spec4, sd_security;
    uint32_t block_count, block_size;
    uint16_t temp_ccc;
    SEGGER_RTT_printf(0, "\r\n Card information:");
    sd_spec = (sd_scr[1] & 0x0F000000) >> 24;
    sd_spec3 = (sd_scr[1] & 0x00008000) >> 15;
    sd_spec4 = (sd_scr[1] & 0x00000400) >> 10;
    if(2 == sd_spec) {
        if(1 == sd_spec3) {
            if(1 == sd_spec4) {
                SEGGER_RTT_printf(0, "\r\n## Card version 4.xx ##");
            } else {
                SEGGER_RTT_printf(0, "\r\n## Card version 3.0x ##");
            }
        } else {
            SEGGER_RTT_printf(0, "\r\n## Card version 2.00 ##");
        }
    } else if(1 == sd_spec) {
        SEGGER_RTT_printf(0, "\r\n## Card version 1.10 ##");
    } else if(0 == sd_spec) {
        SEGGER_RTT_printf(0, "\r\n## Card version 1.0x ##");
    }

    sd_security = (sd_scr[1] & 0x00700000) >> 20;
    if(2 == sd_security) {
        SEGGER_RTT_printf(0, "\r\n## SDSC card ##");
    } else if(3 == sd_security) {
        SEGGER_RTT_printf(0, "\r\n## SDHC card ##");
    } else if(4 == sd_security) {
        SEGGER_RTT_printf(0, "\r\n## SDXC card ##");
    }

    block_count = (sd_cardinfo.card_csd.c_size + 1) * 1024;
    block_size = 512;
    SEGGER_RTT_printf(0, "\r\n## Device size is %dKB ##", sd_card_capacity_get());
    SEGGER_RTT_printf(0, "\r\n## Block size is %dB ##", block_size);
    SEGGER_RTT_printf(0, "\r\n## Block count is %d ##", block_count);

    if(sd_cardinfo.card_csd.read_bl_partial) {
        SEGGER_RTT_printf(0, "\r\n## Partial blocks for read allowed ##");
    }
    if(sd_cardinfo.card_csd.write_bl_partial) {
        SEGGER_RTT_printf(0, "\r\n## Partial blocks for write allowed ##");
    }
    temp_ccc = sd_cardinfo.card_csd.ccc;
    SEGGER_RTT_printf(0, "\r\n## CardCommandClasses is: %x ##", temp_ccc);
    if((SD_CCC_BLOCK_READ & temp_ccc) && (SD_CCC_BLOCK_WRITE & temp_ccc)) {
        SEGGER_RTT_printf(0, "\r\n## Block operation supported ##");
    }
    if(SD_CCC_ERASE & temp_ccc) {
        SEGGER_RTT_printf(0, "\r\n## Erase supported ##");
    }
    if(SD_CCC_WRITE_PROTECTION & temp_ccc) {
        SEGGER_RTT_printf(0, "\r\n## Write protection supported ##");
    }
    if(SD_CCC_LOCK_CARD & temp_ccc) {
        SEGGER_RTT_printf(0, "\r\n## Lock unlock supported ##");
    }
    if(SD_CCC_APPLICATION_SPECIFIC & temp_ccc) {
        SEGGER_RTT_printf(0, "\r\n## Application specific supported ##");
    }
    if(SD_CCC_IO_MODE & temp_ccc) {
        SEGGER_RTT_printf(0, "\r\n## I/O mode supported ##");
    }
    if(SD_CCC_SWITCH & temp_ccc) {
        SEGGER_RTT_printf(0, "\r\n## Switch function supported ##");
    }
}

void sd_fatfs_init(void) {
    uint16_t result = 0;
    result = f_mount(&fs, "0:", 1);
    if(result == FR_NO_FILESYSTEM) {
        SEGGER_RTT_printf(0, "Henan Tian,J S for competition (CASTIC) 410-71\r\n");
        while(1); //未启用DMA

    } else if(result != FR_OK) {
        SEGGER_RTT_printf(0, "File mount error for SD! Result:(%d)\r\n", result);
        SEGGER_RTT_printf(0, "Initilization error maybe!\r\n");
        while(1);

    } else {
        SEGGER_RTT_printf(0, "File system existed in SD!\r\n");
    }
}




int main1() {
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(SDIO_IRQn, 0, 0);

    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "start run\r\n");
    InitTimer();
  	sd_fatfs_init();
    card_info_get();
    while(1) {
    }
}


extern uint16_t slnibp[3200];
int main12() {
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();


    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "start run\r\n");
    SEGGER_RTT_printf(0, "=============================================================\r\n");
    SEGGER_RTT_printf(0, "===========           Start New Test             ============\r\n");
    SEGGER_RTT_printf(0, "=============================================================\r\n");
    InitTimer();

    initPrinter();

    initPressure();
    delay_1ms(3000);

    testReset();
    //delay_1ms(15000);
    //gpio_bit_write(GPIOE, GPIO_PIN_1, RESET);
    //gpio_bit_write(GPIOE, GPIO_PIN_0, SET);
    int  p = 0;
    int before = 500;
    int bb, bbb, bbbb = 500;
    long long n = 0;
    int xds = 0;
    int ms = 0;

    while(1) {
        //pressValue();
        nibp = pressValue();
        if(p == 1) {
            readnibp(nibp);
        }

        ///bbb = bb;
        //bb = before;
        //before = nibp;
        if(nibp >= 240 && p == 0) { //0 Set松 0 RESET紧
            p = 1;
            gpio_bit_write(GPIOE, GPIO_PIN_1, RESET);
            delay_1ms(200);
            gpio_bit_write(GPIOE, GPIO_PIN_0, SET);
            ms = 0;

        }
        if(nibp <= 36 && p == 1) {
            gpio_bit_write(GPIOE, GPIO_PIN_0, RESET);
            solvenibp();
            p = 2;
        }
        val = 10 * slnibp[args];

        //SEGGER_RTT_printf(0, "%d\r\n",val);
        delay_1ms(5);
    }
}

