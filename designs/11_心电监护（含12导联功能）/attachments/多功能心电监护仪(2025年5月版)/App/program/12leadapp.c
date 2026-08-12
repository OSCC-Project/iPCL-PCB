#include "12leadapp.h"
#include "12lead.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include "screen_basic.h"
#include "SEGGER_RTT.h"
#include "AppManager.h"
#include "ecgjudge.h"
#include <stdio.h>
#include "bsp_rtc.h"
#include "uart.h"

bool isiced = 0;
bool start10s;

bool is12inited = 0;


float pl1[2000], pl2[2000], pl3[2000], pavr[2000], pavl[2000], pavf[2000], pv1[2000], pv2[2000], pv3[2000], pv4[2000], pv5[2000], pv6[2000] = {0};

bool sidebar12[80][1400];


int point = 0;
int tpx = 0;
int bfl1, bfl2, bfl3, bfavr, bfavl, bfavf, bfv1, bfv2, bfv3, bfv4, bfv5, bfv6 = 0;
int dpxl1, dpxl2, dpxl3, dpxavr, dpxavl, dpxavf, dpxv1, dpxv2, dpxv3, dpxv4, dpxv5, dpxv6;
int hrc = 0;
int hrjd = 0;
int chr = 0;
int bbfl1, bbfl2, bbfl3;
char txt[] = "HR:";
char buffs[100];
uint8_t wirecolor = GREEN;
int App_12lead_main(void* pra) {
    SEGGER_RTT_printf(0, "12lead Task start\r\n");
    nowapp = 3;
    nowpage = 0;
    isiced = 0;
    start10s = 0;
    tpx = 0;
    point = 0;

    if(!is12inited) {
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
        is12inited = 1;
    }


    SEGGER_RTT_printf(0, "12lead Task Draw\r\n");

    LCD_Clear(1, BLACK);
    LCD_DrawRectangle(0, 0, 799, 479, GREEN);
    POINT_COLOR = YELLOW;
    LCD_ShowString(1, 2, 2, 480, 80, 24, 1, "12");
    LCD_ShowString(1, 2 + 24, 2, 480, 800, 241, 1, " !\"#$");
    LCD_DrawRectangle(1, 26, 798, 27, CYAN);
    //按键菜单
    LCD_ShowString(1, 14, 443, 480, 800, 241, 1, "45");//冻结
    LCD_DrawRectangle(11, 440, 11 + 53, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82, 443, 480, 800, 241, 1, "67"); //分析
    LCD_DrawRectangle(11 + 82, 440, 11 + 53 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82, 443, 480, 800, 24, 1, "10"); //10s
    LCD_ShowString(1, 14 + 82 + 82 + 24, 443, 480, 800, 241, 1, "+"); //10s
    LCD_DrawRectangle(11 + 82 + 82, 440, 11 + 53 + 82 + 82, 440 + 29, CYAN);

    POINT_COLOR = GREEN;
    LCD_ShowString(1, 2, 28, 480, 800, 16, 1, "I");
    LCD_ShowString(1, 400, 28, 480, 800, 16, 1, "V1");
    LCD_DrawRectangle(1, 430, 798, 431, CYAN); //12导联图形    上界28px    下界429px      单个导联使用64px
    LCD_ShowString(1, 2, 28 + 64, 480, 800, 16, 1, "II");
    LCD_ShowString(1, 400, 28 + 64, 480, 800, 16, 1, "V2");
    LCD_ShowString(1, 2, 28 + 64 + 64, 480, 800, 16, 1, "III");
    LCD_ShowString(1, 400, 28 + 64 + 64, 480, 800, 16, 1, "V3");
    LCD_ShowString(1, 2, 28 + 64 + 64 + 64, 480, 800, 16, 1, "aVR");
    LCD_ShowString(1, 400, 28 + 64 + 64 + 64, 480, 800, 16, 1, "V4");
    LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVL");
    LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V5");
    LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVF");
    LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V6"); //拟规定1mV为30px，等电位线为中点，向前清除波形2px


    //1px代表0.0333mV


    //double gl1,gl2,gl3,gavr,gavl,gavf,gv1,gv2,gv3,gv4,gv5,gv6 = 0;

    a12lead_GetmV();
    delay_1ms(5);
    a12lead_GetmV();
    delay_1ms(5);
    a12lead_GetmV();
    delay_1ms(5);
    a12lead_GetmV();
    delay_1ms(5);
    a12lead_GetmV();
    delay_1ms(5);
    //1px代表0.0333mV
    dpxl1 = fl1 / -0.0333;
    dpxl2 = fl2 / -0.0333;
    dpxl3 = fl3 / -0.0333;
    dpxavr = favr / -0.0333;
    dpxavl = favl / -0.0333;
    dpxavf = favf / -0.0333;
    dpxv1 = fv1 / -0.0333;
    dpxv2 = fv2 / -0.0333;
    dpxv3 = fv3 / -0.0333;
    dpxv4 = fv4 / -0.0333;
    dpxv5 = fv5 / -0.0333;
    dpxv6 = fv6 / -0.0333;
    if(28 + 32 + dpxl1 <= 28) {
        dpxl1 = 28 - 28 - 32;
    } else if(28 + 32 + dpxl1 >= 428) {
        dpxl1 = 428 - 28 - 32;
    }
    if(28 + 64 + 32 + dpxl2 <= 28) {
        dpxl2 = 28 - 28 - 32 - 64;
    } else if(28 + 64 + 32 + dpxl2 >= 428) {
        dpxl2 = 428 - 28 - 32 - 64;
    }
    if(28 + 64 + 64 + 32 + dpxl3 <= 28) {
        dpxl3 = 28 - 28 - 32 - 64 - 64;
    } else if(28 + 64 + 64 + 32 + dpxl3 >= 428) {
        dpxl3 = 428 - 28 - 32 - 64 - 64;
    }
    if(28 + 64 + 64 + 64 + 32 + dpxavr <= 28) {
        dpxavr = 28 - 28 - 32 - 64 - 64 - 64;
    } else if(28 + 64 + 64 + 64 + 32 + dpxavr >= 428) {
        dpxavr = 428 - 28 - 32 - 64 - 64 - 64;
    }
    if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl <= 28) {
        dpxavl = 28 - 28 - 32 - 64 - 64 - 64 - 64;
    } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl >= 428) {
        dpxavl = 428 - 28 - 32 - 64 - 64 - 64 - 64;
    }
    if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf <= 28) {
        dpxavf = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
    } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf >= 428) {
        dpxavf = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
    }


    if(28 + 32 + dpxv1 <= 28) {
        dpxv1 = 28 - 28 - 32;
    } else if(28 + 32 + dpxv1 >= 428) {
        dpxv1 = 428 - 28 - 32;
    }
    if(28 + 64 + 32 + dpxv2 <= 28) {
        dpxv2 = 28 - 28 - 32 - 64;
    } else if(28 + 64 + 32 + dpxv2 >= 428) {
        dpxv2 = 428 - 28 - 32 - 64;
    }
    if(28 + 64 + 64 + 32 + dpxv3 <= 28) {
        dpxv3 = 28 - 28 - 32 - 64 - 64;
    } else if(28 + 64 + 64 + 32 + dpxv3 >= 428) {
        dpxv3 = 428 - 28 - 32 - 64 - 64;
    }
    if(28 + 64 + 64 + 64 + 32 + dpxv4 <= 28) {
        dpxv4 = 28 - 28 - 32 - 64 - 64 - 64;
    } else if(28 + 64 + 64 + 64 + 32 + dpxv4 >= 428) {
        dpxv4 = 428 - 28 - 32 - 64 - 64 - 64;
    }
    if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 <= 28) {
        dpxv5 = 28 - 28 - 32 - 64 - 64 - 64 - 64;
    } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 >= 428) {
        dpxv5 = 428 - 28 - 32 - 64 - 64 - 64 - 64;
    }
    if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 <= 28) {
        dpxv6 = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
    } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 >= 428) {
        dpxv6 = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
    }
    //单个导联显示长度为397px
    if(tpx >= 396) {
        tpx = 0;
    }

    bfl1 = dpxl1;
    //II
    bfl2 = dpxl2;
    //III
    bfl3 = dpxl3;
    //aVR
    bfavr = dpxavr;
    //aVL
    bfavl = dpxavl;
    //aVF
    bfavf = dpxavf;
    //v1
    bfv1 = dpxv1;
    //v2
    bfv2 = dpxv2;
    //v3
    bfv3 = dpxv3;
    //v4
    bfv4 = dpxv4;
    //v5
    bfv5 = dpxv5;
    //v6
    bfv6 = dpxv6;
    while(1) {
        if(!isiced) {
            a12lead_GetmV();
						sprintf(buffs,"[$INF$]12T_%.3f_%.3f_%.3f_%.3f_%.3f_%.3f_%.3f_%.3f_E$\r\n",fl2,fl3,fv1,fv2,fv3,fv4,fv5,fv6);
						comString(buffs);

            //1px代表0.0333mV
            dpxl1 = fl1 / -0.0333;
            dpxl2 = fl2 / -0.0333;
            dpxl3 = fl3 / -0.0333;
            dpxavr = favr / -0.0333;
            dpxavl = favl / -0.0333;
            dpxavf = favf / -0.0333;
            dpxv1 = fv1 / -0.0333;
            dpxv2 = fv2 / -0.0333;
            dpxv3 = fv3 / -0.0333;
            dpxv4 = fv4 / -0.0333;
            dpxv5 = fv5 / -0.0333;
            dpxv6 = fv6 / -0.0333;
            if(28 + 32 + dpxl1 <= 28) {
                dpxl1 = 28 - 28 - 32;
            } else if(28 + 32 + dpxl1 >= 428) {
                dpxl1 = 428 - 28 - 32;
            }
            if(28 + 64 + 32 + dpxl2 <= 28) {
                dpxl2 = 28 - 28 - 32 - 64;
            } else if(28 + 64 + 32 + dpxl2 >= 428) {
                dpxl2 = 428 - 28 - 32 - 64;
            }
            if(28 + 64 + 64 + 32 + dpxl3 <= 28) {
                dpxl3 = 28 - 28 - 32 - 64 - 64;
            } else if(28 + 64 + 64 + 32 + dpxl3 >= 428) {
                dpxl3 = 428 - 28 - 32 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 32 + dpxavr <= 28) {
                dpxavr = 28 - 28 - 32 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 32 + dpxavr >= 428) {
                dpxavr = 428 - 28 - 32 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl <= 28) {
                dpxavl = 28 - 28 - 32 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl >= 428) {
                dpxavl = 428 - 28 - 32 - 64 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf <= 28) {
                dpxavf = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf >= 428) {
                dpxavf = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            }


            if(28 + 32 + dpxv1 <= 28) {
                dpxv1 = 28 - 28 - 32;
            } else if(28 + 32 + dpxv1 >= 428) {
                dpxv1 = 428 - 28 - 32;
            }
            if(28 + 64 + 32 + dpxv2 <= 28) {
                dpxv2 = 28 - 28 - 32 - 64;
            } else if(28 + 64 + 32 + dpxv2 >= 428) {
                dpxv2 = 428 - 28 - 32 - 64;
            }
            if(28 + 64 + 64 + 32 + dpxv3 <= 28) {
                dpxv3 = 28 - 28 - 32 - 64 - 64;
            } else if(28 + 64 + 64 + 32 + dpxv3 >= 428) {
                dpxv3 = 428 - 28 - 32 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 32 + dpxv4 <= 28) {
                dpxv4 = 28 - 28 - 32 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 32 + dpxv4 >= 428) {
                dpxv4 = 428 - 28 - 32 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 <= 28) {
                dpxv5 = 28 - 28 - 32 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 >= 428) {
                dpxv5 = 428 - 28 - 32 - 64 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 <= 28) {
                dpxv6 = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 >= 428) {
                dpxv6 = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            }
            //单个导联显示长度为397px
            if(tpx >= 396) {
                tpx = 0;
                LCD_DrawRectangle(tpx + 2, 28, tpx + 2 + 1, 429, BLACK);
                LCD_DrawRectangle(tpx + 400, 28, tpx + 400 + 1, 429, BLACK);
            }

            if(start10s) {
                wirecolor = WHITE;
            } else {
                wirecolor = GREEN;
            }
            //I
            LCD_DrawLine(2 + tpx - 1, 28 + 32 + bfl1, 2 + tpx, 28 + 32 + dpxl1, wirecolor);
            bfl1 = dpxl1;
            bbfl1 = bfl1;

            //II
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 32 + bfl2, 2 + tpx, 28 + 64 + 32 + dpxl2, wirecolor);
            bfl2 = dpxl2;
            bbfl2 = bfl2;

            //III
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 32 + bfl3, 2 + tpx, 28 + 64 + 64 + 32 + dpxl3, wirecolor);
            bfl3 = dpxl3;
            bbfl3 = bfl3;

            //aVR
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 32 + bfavr, 2 + tpx, 28 + 64 + 64 + 64 + 32 + dpxavr, wirecolor);
            bfavr = dpxavr;

            //aVL
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 32 + bfavl, 2 + tpx, 28 + 64 + 64 + 64 + 64 + 32 + dpxavl, wirecolor);
            bfavl = dpxavl;

            //aVF
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 64 + 32 + bfavf, 2 + tpx, 28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf, wirecolor);
            bfavf = dpxavf;



            //v1
            LCD_DrawLine(400 + tpx - 1, 28 + 32 + bfv1, 400 + tpx, 28 + 32 + dpxv1, wirecolor);
            bfv1 = dpxv1;

            //v2
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 32 + bfv2, 400 + tpx, 28 + 64 + 32 + dpxv2, wirecolor);
            bfv2 = dpxv2;

            //v3
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 32 + bfv3, 400 + tpx, 28 + 64 + 64 + 32 + dpxv3, wirecolor);
            bfv3 = dpxv3;

            //v4
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 32 + bfv4, 400 + tpx, 28 + 64 + 64 + 64 + 32 + dpxv4, wirecolor);
            bfv4 = dpxv4;

            //v5
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 32 + bfv5, 400 + tpx, 28 + 64 + 64 + 64 + 64 + 32 + dpxv5, wirecolor);
            bfv5 = dpxv5;

            //v6
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 64 + 32 + bfv6, 400 + tpx, 28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6, wirecolor);
            bfv6 = dpxv6;

            LCD_DrawRectangle(tpx + 2 + 1, 28, tpx + 2 + 2, 429, BLACK);
            LCD_DrawRectangle(tpx + 400 + 1, 28, tpx + 400 + 2, 429, BLACK);
            POINT_COLOR = GREEN;
            LCD_ShowString(1, 2, 28, 480, 800, 16, 1, "I");
            LCD_ShowString(1, 400, 28, 480, 800, 16, 1, "V1");
            LCD_DrawRectangle(1, 430, 798, 431, CYAN); //12导联图形    上界28px    下界429px      单个导联使用64px
            LCD_ShowString(1, 2, 28 + 64, 480, 800, 16, 1, "II");
            LCD_ShowString(1, 400, 28 + 64, 480, 800, 16, 1, "V2");
            LCD_ShowString(1, 2, 28 + 64 + 64, 480, 800, 16, 1, "III");
            LCD_ShowString(1, 400, 28 + 64 + 64, 480, 800, 16, 1, "V3");
            LCD_ShowString(1, 2, 28 + 64 + 64 + 64, 480, 800, 16, 1, "aVR");
            LCD_ShowString(1, 400, 28 + 64 + 64 + 64, 480, 800, 16, 1, "V4");
            LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVL");
            LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V5");
            LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVF");
            LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V6"); //拟规定1mV为30px，等电位线为中点，向前清除波形2px

            LCD_DrawRectangle(20, 45, 21, 45 + 30, GREEN);
            LCD_DrawRectangle(20, 45 + 64, 21, 45 + 30 + 64, GREEN);
            LCD_DrawRectangle(20, 45 + 64 + 64, 21, 45 + 30 + 64 + 64, GREEN);
            LCD_DrawRectangle(20, 45 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64, GREEN);
            LCD_DrawRectangle(20, 45 + 64 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64 + 64, GREEN);
            LCD_DrawRectangle(20, 45 + 64 + 64 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64 + 64 + 64, GREEN);


            tpx++;
            if(start10s) {
                pl1[point] = fl1 / 0.0125;
                pl2[point] = fl2 / 0.0125;
                pl3[point] = fl3 / 0.0125;
                pavr[point] = favr / 0.0125;
                pavl[point] = favl / 0.0125;
                pavf[point] = favf / 0.0125;
                pv1[point] = fv1 / 0.0125;
                pv2[point] = fv2 / 0.0125;
                pv3[point] = fv3 / 0.0125;
                pv4[point] = fv4 / 0.0125;
                pv5[point] = fv5 / 0.0125;
                pv6[point] = fv6 / 0.0125;
                point++;
                if(point >= 1999) {
                    start10s = 0;
                    point = 0;
                    solveecg10s();
                    clearA4();
                    rtc_current_time_get(&rtc_initpara_time);
                    //信息页
                    sprintf(buffs, "20%d-%d-%d %d:%d:%d", BcdToDecimal(rtc_initpara_time.year), BcdToDecimal(rtc_initpara_time.month), BcdToDecimal(rtc_initpara_time.date), BcdToDecimal(rtc_initpara_time.hour),  BcdToDecimal(rtc_initpara_time.minute), BcdToDecimal(rtc_initpara_time.second));
                    A4_DrawString(1, 1, 800, 120, 24, 1, buffs);
                    A4_DrawString(1, 40, 800, 120, 24, 1, "NAME:");
                    A4_DrawString(1, 80, 800, 120, 24, 1, "AGE:");
                    //概要
                    A4_DrawString(281, 1, 800, 120, 241, 1, "\"A"); //HR
                    sprintf(buffs, ": %d", led12_s_HR);
                    A4_DrawString(281 + 48, 1, 800, 120, 24, 1, buffs);

                    sprintf(buffs, "RR: %d ms", led12_s_RR);
                    A4_DrawString(281, 1 + 32, 800, 120, 24, 1, buffs); //RR

                    sprintf(buffs, ": %d deg", led12_s_QRS_deg);
                    A4_DrawString(281, 1 + 32 + 32, 800, 120, 24, 1, "QRS"); //QRS
                    A4_DrawString(281 + 36, 1 + 32 + 32, 800, 120, 241, 1, "#B"); //电轴
                    A4_DrawString(281 + 36 + 48, 1 + 32 + 32, 800, 120, 24, 1, buffs); //电轴

                    sprintf(buffs, "QRS: %d ms", led12_s_QRS);
                    A4_DrawString(281, 1 + 32 + 32 + 32, 800, 120, 24, 1, buffs); //RR

                    sprintf(buffs, "T: %d ms", led12_s_T);
                    A4_DrawString(281, 1 + 32 + 32 + 32 + 32, 800, 120, 24, 1, buffs); //RR
                    //概要：下一页
                    sprintf(buffs, "QT: %d ms", led12_s_QT);
                    A4_DrawString(481, 1, 800, 120, 24, 1, buffs); //QT

                    sprintf(buffs, "QTc: %d ms", led12_s_QTc);
                    A4_DrawString(481, 1 + 32, 800, 120, 24, 1, buffs); //QTc

                    sprintf(buffs, "RV5+SV1: %.3f mV", (led12_s_RV5 + led12_s_SV1));
                    A4_DrawString(481, 1 + 32 + 32, 800, 120, 24, 1, buffs); //RS

                    sprintf(buffs, "RV1+SV5: %.3f mV", (led12_s_RV1 + led12_s_SV5));
                    A4_DrawString(481, 1 + 32 + 32 + 32, 800, 120, 24, 1, buffs); //RS
                    
                    memset(&sidebar12,0,sizeof sidebar12);
                    
                    
                    //侧边栏
                    Side_DrawString(1,1,80,1400,24,1,"I");
                    Side_DrawString(1,1+120,80,1400,24,1,"II");
                    Side_DrawString(1,1+120+120,80,1400,24,1,"III");
                    Side_DrawString(1,1+120+120+120,80,1400,24,1,"aVR");
                    Side_DrawString(1,1+120+120+120+120,80,1400,24,1,"aVL");
                    Side_DrawString(1,1+120+120+120+120+120,80,1400,24,1,"aVF");
                    Side_DrawString(1,1+120+120+120+120+120+120,80,1400,24,1,"V1");
                    Side_DrawString(1,1+120+120+120+120+120+120+120,80,1400,24,1,"V2");
                    Side_DrawString(1,1+120+120+120+120+120+120+120+120,80,1400,24,1,"V3");
                    Side_DrawString(1,1+120+120+120+120+120+120+120+120+120,80,1400,24,1,"V4");
                    Side_DrawString(1,1+120+120+120+120+120+120+120+120+120+120,80,1400,24,1,"V5");
                    Side_DrawString(1,1+120+120+120+120+120+120+120+120+120+120+120,80,1400,24,1,"V6");
                    
                    //基准左
                    for(int f = 0;f < 16;f++){
                      sidebar12[f+32][80] = 1;
                      sidebar12[f+32][80+120] = 1;
                      sidebar12[f+32][80+240] = 1;
                      sidebar12[f+32][80+360] = 1;
                      sidebar12[f+32][80+480] = 1;
                      sidebar12[f+32][80+600] = 1;
                      sidebar12[f+32][80+720] = 1;
                      sidebar12[f+32][80+840] = 1;
                      sidebar12[f+32][80+960] = 1;
                      sidebar12[f+32][80+1080] = 1;
                      sidebar12[f+32][80+1200] = 1;
                      sidebar12[f+32][80+1319] = 1;
                    }
                    //基准右
                    for(int f = 0;f < 16;f++){
                      sidebar12[f+64][80] = 1;
                      sidebar12[f+64][80+120] = 1;
                      sidebar12[f+64][80+240] = 1;
                      sidebar12[f+64][80+360] = 1;
                      sidebar12[f+64][80+480] = 1;
                      sidebar12[f+64][80+600] = 1;
                      sidebar12[f+64][80+720] = 1;
                      sidebar12[f+64][80+840] = 1;
                      sidebar12[f+64][80+960] = 1;
                      sidebar12[f+64][80+1080] = 1;
                      sidebar12[f+64][80+1200] = 1;
                      sidebar12[f+64][80+1319] = 1;
                    }
                    //基准顶
                    for(int f = 0;f < 16;f++){
                      sidebar12[f+48][0] = 1;
                      sidebar12[f+48][120] = 1;
                      sidebar12[f+48][240] = 1;
                      sidebar12[f+48][360] = 1;
                      sidebar12[f+48][480] = 1;
                      sidebar12[f+48][600] = 1;
                      sidebar12[f+48][720] = 1;
                      sidebar12[f+48][840] = 1;
                      sidebar12[f+48][960] = 1;
                      sidebar12[f+48][1080] = 1;
                      sidebar12[f+48][1200] = 1;
                      sidebar12[f+48][1319] = 1;
                    }
                    //画线
                    for(int f = 0;f < 12;f++){
                      for(int k = 0; k <4;k++){
                        sidebar12[48][120*f+k*24] = 1;
                        sidebar12[48][120*f+k*24+1] = 1;
                        sidebar12[48][120*f+k*24+2] = 1;
                        sidebar12[48][120*f+k*24+3] = 1;
                        sidebar12[48][120*f+k*24+4] = 1;
                        sidebar12[48][120*f+k*24+5] = 1;
                        sidebar12[48][120*f+k*24+6] = 1;
                        sidebar12[48][120*f+k*24+7] = 1;
                      }
                    }
                    
                    for(int f = 0;f < 12;f++){
                      for(int k = 0;k <4;k++){
                        sidebar12[64][120*f+k*24] = 1;
                        sidebar12[64][120*f+k*24+1] = 1;
                        sidebar12[64][120*f+k*24+2] = 1;
                        sidebar12[64][120*f+k*24+3] = 1;
                        sidebar12[64][120*f+k*24+4] = 1;
                        sidebar12[64][120*f+k*24+5] = 1;
                        sidebar12[64][120*f+k*24+6] = 1;
                        sidebar12[64][120*f+k*24+7] = 1;
                      }
                    }
                    
                    return 31;
                }
            }

            hrjd = 0;
            if(dpxl1 >= bbfl1 + 5 || dpxl1 <= bbfl1 - 5) {
                hrjd++;
            }
            if(dpxl2 >= bbfl2 + 5 || dpxl2 <= bbfl1 - 5) {
                hrjd++;
            }
            if(dpxl3 >= bbfl3 + 5 || dpxl3 <= bbfl1 - 5) {
                hrjd++;
            }
            hrc += 5;
            if(hrjd >= 2 && hrc > 200) {
                chr = 60000 / hrc;
                hrc = 0;
                SEGGER_RTT_printf(0, "HR - %d\r\n", chr);
                LCD_SolidRectangle(720, 1, 798, 25, BLACK);
                sprintf(buffs, "%s%d", txt, chr);
                LCD_ShowString(1, 720, 2, 480, 80, 24, 1, buffs);
            }
            delay_us(4300);
        } else {
            delay_1ms(5);
        }

        //App销毁标识
        if(gpio_input_bit_get(GPIOE, GPIO_PIN_10) == RESET) {
            selectapp = MODE_A;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_11) == RESET) {
            selectapp = MODE_B;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_12) == RESET) {
            selectapp = MODE_C;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_13) == RESET) {
            selectapp = MODE_D;
        } else {
            //selectapp = MODE_A;
        }


        if(selectapp != nowapp) {
            destorysign++;
            //SEGGER_RTT_printf(0, "sel=%d now=%d\r\n", selectapp,nowapp);
            if(destorysign >= 250) {
                destorysign = 0;
                return selectapp;
            }
        } else {
            destorysign = 0;
        }

    }
}


uint8_t viewpage = 1;
int k = 0;
int App_12lead_10s(void* pra) {
    nowapp = 3;
    nowpage = 1;

    viewpage = 1;

    LCD_Clear(1, BLACK);
    LCD_DrawRectangle(0, 0, 799, 479, GREEN);
    POINT_COLOR = YELLOW;
    LCD_ShowString(1, 2, 2, 480, 80, 241, 1, "\"#$)*");
    LCD_DrawRectangle(1, 26, 798, 27, CYAN);
    //按键菜单
    LCD_ShowString(1, 14, 443, 480, 800, 241, 1, "<>");//上页
    LCD_DrawRectangle(11, 440, 11 + 53, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82, 443, 480, 800, 241, 1, "=>"); //下页
    LCD_DrawRectangle(11 + 82, 440, 11 + 53 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82, 443, 480, 800, 241, 1, "9:"); //打印
    LCD_DrawRectangle(11 + 82 + 82, 440, 11 + 53 + 82 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82 + 82, 443, 480, 800, 241, 1, "?@"); //返回
    LCD_DrawRectangle(11 + 82 + 82 + 82, 440, 11 + 53 + 82 + 82 + 82, 440 + 29, CYAN);

    POINT_COLOR = WHITE;
    LCD_ShowString(1, 2, 28, 480, 800, 16, 1, "I");
    LCD_ShowString(1, 400, 28, 480, 800, 16, 1, "V1");
    LCD_DrawRectangle(1, 430, 798, 431, CYAN); //12导联图形    上界28px    下界429px      单个导联使用64px
    LCD_ShowString(1, 2, 28 + 64, 480, 800, 16, 1, "II");
    LCD_ShowString(1, 400, 28 + 64, 480, 800, 16, 1, "V2");
    LCD_ShowString(1, 2, 28 + 64 + 64, 480, 800, 16, 1, "III");
    LCD_ShowString(1, 400, 28 + 64 + 64, 480, 800, 16, 1, "V3");
    LCD_ShowString(1, 2, 28 + 64 + 64 + 64, 480, 800, 16, 1, "aVR");
    LCD_ShowString(1, 400, 28 + 64 + 64 + 64, 480, 800, 16, 1, "V4");
    LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVL");
    LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V5");
    LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVF");
    LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V6"); //拟规定1mV为30px，等电位线为中点，向前清除波形2px

    LCD_DrawRectangle(20, 45, 21, 45 + 30, YELLOW);
    LCD_DrawRectangle(20, 45 + 64, 21, 45 + 30 + 64, YELLOW);
    LCD_DrawRectangle(20, 45 + 64 + 64, 21, 45 + 30 + 64 + 64, YELLOW);
    LCD_DrawRectangle(20, 45 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64, YELLOW);
    LCD_DrawRectangle(20, 45 + 64 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64 + 64, YELLOW);
    LCD_DrawRectangle(20, 45 + 64 + 64 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64 + 64 + 64, YELLOW);



    for(k = 0; k < 396; k++) {
        //1px代表0.0333mV
        tpx = k;
        dpxl1 = pl1[k] * 0.0125 / -0.0333;
        dpxl2 = pl2[k] * 0.0125 / -0.0333;
        dpxl3 = pl3[k] * 0.0125 / -0.0333;
        dpxavr = pavr[k] * 0.0125 / -0.0333;
        dpxavl = pavl[k] * 0.0125 / -0.0333;
        dpxavf = pavf[k] * 0.0125 / -0.0333;
        dpxv1 = pv1[k] * 0.0125 / -0.0333;
        dpxv2 = pv2[k] * 0.0125 / -0.0333;
        dpxv3 = pv3[k] * 0.0125 / -0.0333;
        dpxv4 = pv4[k] * 0.0125 / -0.0333;
        dpxv5 = pv5[k] * 0.0125 / -0.0333;
        dpxv6 = pv6[k] * 0.0125 / -0.0333;
        if(28 + 32 + dpxl1 <= 28) {
            dpxl1 = 28 - 28 - 32;
        } else if(28 + 32 + dpxl1 >= 428) {
            dpxl1 = 428 - 28 - 32;
        }
        if(28 + 64 + 32 + dpxl2 <= 28) {
            dpxl2 = 28 - 28 - 32 - 64;
        } else if(28 + 64 + 32 + dpxl2 >= 428) {
            dpxl2 = 428 - 28 - 32 - 64;
        }
        if(28 + 64 + 64 + 32 + dpxl3 <= 28) {
            dpxl3 = 28 - 28 - 32 - 64 - 64;
        } else if(28 + 64 + 64 + 32 + dpxl3 >= 428) {
            dpxl3 = 428 - 28 - 32 - 64 - 64;
        }
        if(28 + 64 + 64 + 64 + 32 + dpxavr <= 28) {
            dpxavr = 28 - 28 - 32 - 64 - 64 - 64;
        } else if(28 + 64 + 64 + 64 + 32 + dpxavr >= 428) {
            dpxavr = 428 - 28 - 32 - 64 - 64 - 64;
        }
        if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl <= 28) {
            dpxavl = 28 - 28 - 32 - 64 - 64 - 64 - 64;
        } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl >= 428) {
            dpxavl = 428 - 28 - 32 - 64 - 64 - 64 - 64;
        }
        if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf <= 28) {
            dpxavf = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
        } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf >= 428) {
            dpxavf = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
        }


        if(28 + 32 + dpxv1 <= 28) {
            dpxv1 = 28 - 28 - 32;
        } else if(28 + 32 + dpxv1 >= 428) {
            dpxv1 = 428 - 28 - 32;
        }
        if(28 + 64 + 32 + dpxv2 <= 28) {
            dpxv2 = 28 - 28 - 32 - 64;
        } else if(28 + 64 + 32 + dpxv2 >= 428) {
            dpxv2 = 428 - 28 - 32 - 64;
        }
        if(28 + 64 + 64 + 32 + dpxv3 <= 28) {
            dpxv3 = 28 - 28 - 32 - 64 - 64;
        } else if(28 + 64 + 64 + 32 + dpxv3 >= 428) {
            dpxv3 = 428 - 28 - 32 - 64 - 64;
        }
        if(28 + 64 + 64 + 64 + 32 + dpxv4 <= 28) {
            dpxv4 = 28 - 28 - 32 - 64 - 64 - 64;
        } else if(28 + 64 + 64 + 64 + 32 + dpxv4 >= 428) {
            dpxv4 = 428 - 28 - 32 - 64 - 64 - 64;
        }
        if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 <= 28) {
            dpxv5 = 28 - 28 - 32 - 64 - 64 - 64 - 64;
        } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 >= 428) {
            dpxv5 = 428 - 28 - 32 - 64 - 64 - 64 - 64;
        }
        if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 <= 28) {
            dpxv6 = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
        } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 >= 428) {
            dpxv6 = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
        }

        wirecolor = WHITE;
        //I
        LCD_DrawLine(2 + tpx - 1, 28 + 32 + bfl1, 2 + tpx, 28 + 32 + dpxl1, wirecolor);
        bfl1 = dpxl1;

        //II
        LCD_DrawLine(2 + tpx - 1, 28 + 64 + 32 + bfl2, 2 + tpx, 28 + 64 + 32 + dpxl2, wirecolor);
        bfl2 = dpxl2;

        //III
        LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 32 + bfl3, 2 + tpx, 28 + 64 + 64 + 32 + dpxl3, wirecolor);
        bfl3 = dpxl3;

        //aVR
        LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 32 + bfavr, 2 + tpx, 28 + 64 + 64 + 64 + 32 + dpxavr, wirecolor);
        bfavr = dpxavr;

        //aVL
        LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 32 + bfavl, 2 + tpx, 28 + 64 + 64 + 64 + 64 + 32 + dpxavl, wirecolor);
        bfavl = dpxavl;

        //aVF
        LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 64 + 32 + bfavf, 2 + tpx, 28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf, wirecolor);
        bfavf = dpxavf;



        //v1
        LCD_DrawLine(400 + tpx - 1, 28 + 32 + bfv1, 400 + tpx, 28 + 32 + dpxv1, wirecolor);
        bfv1 = dpxv1;

        //v2
        LCD_DrawLine(400 + tpx - 1, 28 + 64 + 32 + bfv2, 400 + tpx, 28 + 64 + 32 + dpxv2, wirecolor);
        bfv2 = dpxv2;

        //v3
        LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 32 + bfv3, 400 + tpx, 28 + 64 + 64 + 32 + dpxv3, wirecolor);
        bfv3 = dpxv3;

        //v4
        LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 32 + bfv4, 400 + tpx, 28 + 64 + 64 + 64 + 32 + dpxv4, wirecolor);
        bfv4 = dpxv4;

        //v5
        LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 32 + bfv5, 400 + tpx, 28 + 64 + 64 + 64 + 64 + 32 + dpxv5, wirecolor);
        bfv5 = dpxv5;

        //v6
        LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 64 + 32 + bfv6, 400 + tpx, 28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6, wirecolor);
        bfv6 = dpxv6;
    }


    for(int m = 0; m < newix; m++) {
        if(waveindexfxnew[m] * 5 != -1) {
            if(waveindexfxnew[m] * 5 / 5 < 396) {
                //SEGGER_RTT_printf(0, "Draw R px=1+%d\r\n", waveindex[m] / 5);
                LCD_DrawLine(1 + (waveindexfxnew[m] * 5 / 5), 28, 1 + (waveindexfxnew[m] * 5 / 5), 429, RED);
            } else {
                break;
            }
        }
    }

    while(1) {
        if(viewpage == 20) {
            return 3;
        }
        if(viewpage != 1) {
            return 31001;
        }
        delay_1ms(10);

        //App销毁标识
        if(gpio_input_bit_get(GPIOE, GPIO_PIN_10) == RESET) {
            selectapp = MODE_A;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_11) == RESET) {
            selectapp = MODE_B;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_12) == RESET) {
            selectapp = MODE_C;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_13) == RESET) {
            selectapp = MODE_D;
        } else {
            //selectapp = MODE_A;
        }


        if(selectapp != nowapp) {
            destorysign++;
            //SEGGER_RTT_printf(0, "sel=%d now=%d\r\n", selectapp,nowapp);
            if(destorysign >= 250) {
                destorysign = 0;
                return selectapp;
            }
        } else {
            destorysign = 0;
        }

    }
}

int App_12lead_func_viewpage(uint8_t page) {//1 - 6页 前5页图形 最后一页分析
    SEGGER_RTT_printf(0, "page=%d\r\n", page);
    nowapp = 3;
    nowpage = 1;

    viewpage = page;

    LCD_Clear(1, BLACK);
    LCD_DrawRectangle(0, 0, 799, 479, GREEN);
    POINT_COLOR = YELLOW;
    LCD_ShowString(1, 2, 2, 480, 80, 241, 1, "\"#$)*");
    LCD_DrawRectangle(1, 26, 798, 27, CYAN);
    //按键菜单
    LCD_ShowString(1, 14, 443, 480, 800, 241, 1, "<>");//上页
    LCD_DrawRectangle(11, 440, 11 + 53, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82, 443, 480, 800, 241, 1, "=>"); //下页
    LCD_DrawRectangle(11 + 82, 440, 11 + 53 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82, 443, 480, 800, 241, 1, "9:"); //打印
    LCD_DrawRectangle(11 + 82 + 82, 440, 11 + 53 + 82 + 82, 440 + 29, CYAN);
    LCD_DrawRectangle(1, 430, 798, 431, CYAN);

    LCD_ShowString(1, 14 + 82 + 82 + 82, 443, 480, 800, 241, 1, "?@"); //返回
    LCD_DrawRectangle(11 + 82 + 82 + 82, 440, 11 + 53 + 82 + 82 + 82, 440 + 29, CYAN);
    if(viewpage == 6) {

        LCD_ShowString(1, 2, 28, 480, 800, 161, 1, "\"#$67");
        sprintf(buffs, "HR: %d bpm(RR:%d ms)", led12_s_HR, led12_s_RR);
        LCD_ShowString(1, 2, 28 + 16, 480, 800, 16, 1, buffs);
        sprintf(buffs, "P: %d ms", led12_s_P);
        LCD_ShowString(1, 2, 28 + 32 + 16, 480, 800, 16, 1, buffs);
        sprintf(buffs, "QRS: %d ms (%d deg)", led12_s_QRS, led12_s_QRS_deg);
        LCD_ShowString(1, 2, 28 + 32 + 16 + 16, 480, 800, 16, 1, buffs);
        sprintf(buffs, "T: %d ms", led12_s_T);
        LCD_ShowString(1, 2, 28 + 32 + 16 + 16 + 16, 480, 800, 16, 1, buffs);
        sprintf(buffs, "QT/QTc: %d/%d ms", led12_s_QT, led12_s_QTc);
        LCD_ShowString(1, 2, 28 + 32 + 16 + 16 + 16 + 32, 480, 800, 16, 1, buffs);
        sprintf(buffs, "RV5+SV1: %f mV", (led12_s_RV5 + led12_s_SV1));
        LCD_ShowString(1, 2, 28 + 32 + 16 + 16 + 16 + 32 + 32, 480, 800, 16, 1, buffs);
        sprintf(buffs, "RV1+SV5: %f mV", (led12_s_RV1 + led12_s_SV5));
        LCD_ShowString(1, 2, 28 + 32 + 16 + 16 + 16 + 32 + 32 + 16, 480, 800, 16, 1, buffs);

    } else {
        POINT_COLOR = WHITE;
        LCD_ShowString(1, 2, 28, 480, 800, 16, 1, "I");
        LCD_ShowString(1, 400, 28, 480, 800, 16, 1, "V1");
        //12导联图形    上界28px    下界429px      单个导联使用64px
        LCD_ShowString(1, 2, 28 + 64, 480, 800, 16, 1, "II");
        LCD_ShowString(1, 400, 28 + 64, 480, 800, 16, 1, "V2");
        LCD_ShowString(1, 2, 28 + 64 + 64, 480, 800, 16, 1, "III");
        LCD_ShowString(1, 400, 28 + 64 + 64, 480, 800, 16, 1, "V3");
        LCD_ShowString(1, 2, 28 + 64 + 64 + 64, 480, 800, 16, 1, "aVR");
        LCD_ShowString(1, 400, 28 + 64 + 64 + 64, 480, 800, 16, 1, "V4");
        LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVL");
        LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V5");
        LCD_ShowString(1, 2, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "aVF");
        LCD_ShowString(1, 400, 28 + 64 + 64 + 64 + 64 + 64, 480, 800, 16, 1, "V6"); //拟规定1mV为30px，等电位线为中点，向前清除波形2px

        LCD_DrawRectangle(20, 45, 21, 45 + 30, YELLOW);
        LCD_DrawRectangle(20, 45 + 64, 21, 45 + 30 + 64, YELLOW);
        LCD_DrawRectangle(20, 45 + 64 + 64, 21, 45 + 30 + 64 + 64, YELLOW);
        LCD_DrawRectangle(20, 45 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64, YELLOW);
        LCD_DrawRectangle(20, 45 + 64 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64 + 64, YELLOW);
        LCD_DrawRectangle(20, 45 + 64 + 64 + 64 + 64 + 64, 21, 45 + 30 + 64 + 64 + 64 + 64 + 64, YELLOW);


        for(k = 0; k < 396; k++) {
            //1px代表0.0333mV
            tpx = k;
            dpxl1 = pl1[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxl2 = pl2[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxl3 = pl3[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxavr = pavr[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxavl = pavl[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxavf = pavf[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxv1 = pv1[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxv2 = pv2[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxv3 = pv3[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxv4 = pv4[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxv5 = pv5[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            dpxv6 = pv6[k + 396 * (page - 1)] * 0.0125 / -0.0333;
            if(28 + 32 + dpxl1 <= 28) {
                dpxl1 = 28 - 28 - 32;
            } else if(28 + 32 + dpxl1 >= 428) {
                dpxl1 = 428 - 28 - 32;
            }
            if(28 + 64 + 32 + dpxl2 <= 28) {
                dpxl2 = 28 - 28 - 32 - 64;
            } else if(28 + 64 + 32 + dpxl2 >= 428) {
                dpxl2 = 428 - 28 - 32 - 64;
            }
            if(28 + 64 + 64 + 32 + dpxl3 <= 28) {
                dpxl3 = 28 - 28 - 32 - 64 - 64;
            } else if(28 + 64 + 64 + 32 + dpxl3 >= 428) {
                dpxl3 = 428 - 28 - 32 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 32 + dpxavr <= 28) {
                dpxavr = 28 - 28 - 32 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 32 + dpxavr >= 428) {
                dpxavr = 428 - 28 - 32 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl <= 28) {
                dpxavl = 28 - 28 - 32 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxavl >= 428) {
                dpxavl = 428 - 28 - 32 - 64 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf <= 28) {
                dpxavf = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf >= 428) {
                dpxavf = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            }


            if(28 + 32 + dpxv1 <= 28) {
                dpxv1 = 28 - 28 - 32;
            } else if(28 + 32 + dpxv1 >= 428) {
                dpxv1 = 428 - 28 - 32;
            }
            if(28 + 64 + 32 + dpxv2 <= 28) {
                dpxv2 = 28 - 28 - 32 - 64;
            } else if(28 + 64 + 32 + dpxv2 >= 428) {
                dpxv2 = 428 - 28 - 32 - 64;
            }
            if(28 + 64 + 64 + 32 + dpxv3 <= 28) {
                dpxv3 = 28 - 28 - 32 - 64 - 64;
            } else if(28 + 64 + 64 + 32 + dpxv3 >= 428) {
                dpxv3 = 428 - 28 - 32 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 32 + dpxv4 <= 28) {
                dpxv4 = 28 - 28 - 32 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 32 + dpxv4 >= 428) {
                dpxv4 = 428 - 28 - 32 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 <= 28) {
                dpxv5 = 28 - 28 - 32 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 32 + dpxv5 >= 428) {
                dpxv5 = 428 - 28 - 32 - 64 - 64 - 64 - 64;
            }
            if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 <= 28) {
                dpxv6 = 28 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            } else if(28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6 >= 428) {
                dpxv6 = 428 - 28 - 32 - 64 - 64 - 64 - 64 - 64;
            }

            wirecolor = WHITE;
            //I
            LCD_DrawLine(2 + tpx - 1, 28 + 32 + bfl1, 2 + tpx, 28 + 32 + dpxl1, wirecolor);
            bfl1 = dpxl1;

            //II
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 32 + bfl2, 2 + tpx, 28 + 64 + 32 + dpxl2, wirecolor);
            bfl2 = dpxl2;

            //III
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 32 + bfl3, 2 + tpx, 28 + 64 + 64 + 32 + dpxl3, wirecolor);
            bfl3 = dpxl3;

            //aVR
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 32 + bfavr, 2 + tpx, 28 + 64 + 64 + 64 + 32 + dpxavr, wirecolor);
            bfavr = dpxavr;

            //aVL
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 32 + bfavl, 2 + tpx, 28 + 64 + 64 + 64 + 64 + 32 + dpxavl, wirecolor);
            bfavl = dpxavl;

            //aVF
            LCD_DrawLine(2 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 64 + 32 + bfavf, 2 + tpx, 28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxavf, wirecolor);
            bfavf = dpxavf;



            //v1
            LCD_DrawLine(400 + tpx - 1, 28 + 32 + bfv1, 400 + tpx, 28 + 32 + dpxv1, wirecolor);
            bfv1 = dpxv1;

            //v2
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 32 + bfv2, 400 + tpx, 28 + 64 + 32 + dpxv2, wirecolor);
            bfv2 = dpxv2;

            //v3
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 32 + bfv3, 400 + tpx, 28 + 64 + 64 + 32 + dpxv3, wirecolor);
            bfv3 = dpxv3;

            //v4
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 32 + bfv4, 400 + tpx, 28 + 64 + 64 + 64 + 32 + dpxv4, wirecolor);
            bfv4 = dpxv4;

            //v5
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 32 + bfv5, 400 + tpx, 28 + 64 + 64 + 64 + 64 + 32 + dpxv5, wirecolor);
            bfv5 = dpxv5;

            //v6
            LCD_DrawLine(400 + tpx - 1, 28 + 64 + 64 + 64 + 64 + 64 + 32 + bfv6, 400 + tpx, 28 + 64 + 64 + 64 + 64 + 64 + 32 + dpxv6, wirecolor);
            bfv6 = dpxv6;
        }

        for(int m = 0; m < newix; m++) {
            if(waveindexfxnew[m] * 5 != -1) {
                if(waveindexfxnew[m] * 5 / 5 < 396 * page) {
                    if(waveindexfxnew[m] * 5 / 5 > 396 * (page - 1)) {
                        //SEGGER_RTT_printf(0, "Draw R px=1+%d\r\n", waveindex[m] / 5);
                        LCD_DrawLine(1 + (waveindexfxnew[m] * 5 / 5) - 396 * (page - 1), 28, 1 + (waveindexfxnew[m] * 5 / 5) - 396 * (page - 1), 429, RED);
                    }

                } else {
                    break;
                }
            }
        }
    }

    while(1) {

        if(viewpage == 20) {
            return 3;
        }
        if(viewpage != page) {
            return 31001;
        }
        delay_1ms(10);

        //App销毁标识
        if(gpio_input_bit_get(GPIOE, GPIO_PIN_10) == RESET) {
            selectapp = MODE_A;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_11) == RESET) {
            selectapp = MODE_B;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_12) == RESET) {
            selectapp = MODE_C;
        } else if(gpio_input_bit_get(GPIOE, GPIO_PIN_13) == RESET) {
            selectapp = MODE_D;
        } else {
            //selectapp = MODE_A;
        }


        if(selectapp != nowapp) {
            destorysign++;
            //SEGGER_RTT_printf(0, "sel=%d now=%d\r\n", selectapp,nowapp);
            if(destorysign >= 250) {
                destorysign = 0;
                return selectapp;
            }
        } else {
            destorysign = 0;
        }

    }
}

extern bool steppaper;
void App_12lead_print10s() {
    //回纸
    for(int f = 0; f < 40; f++) {
        paperBack();
        delay_us(1400);
        paperBack();
        delay_us(1400);
    }
    paperStop();
    //装订线
    for(int f = 0; f < 80; f++) {
        paperStep();
        delay_us(1400);
        paperStep();
        delay_us(1400);
    }
    paperStop();
    delay_1ms(100);
    steppaper = 0;
    memset(&dprintBuffer, 0, sizeof dprintBuffer);
    for(int f = 0; f < 216; f++) {
        dprintBuffer[f * 8] = 1;
        //dprintBuffer[f * 8 + 1] = 1;
        //dprintBuffer[f * 8 + 2] = 1;
        //dprintBuffer[f * 8 + 3] = 1;
    }
    printLine();
    
    delay_1ms(10);
    memset(&dprintBuffer, 0, sizeof dprintBuffer);
    for(int f = 0; f < 216; f++) {
        //dprintBuffer[f * 8] = 1;
        dprintBuffer[f * 8 + 1] = 1;
        //dprintBuffer[f * 8 + 2] = 1;
        //dprintBuffer[f * 8 + 3] = 1;
    }
    printLine();
    
    delay_1ms(10);
    memset(&dprintBuffer, 0, sizeof dprintBuffer);
    for(int f = 0; f < 216; f++) {
        //dprintBuffer[f * 8] = 1;
        //dprintBuffer[f * 8 + 1] = 1;
        dprintBuffer[f * 8 + 2] = 1;
        //dprintBuffer[f * 8 + 3] = 1;
    }
    printLine();
    
    delay_1ms(10);
    memset(&dprintBuffer, 0, sizeof dprintBuffer);
    for(int f = 0; f < 216; f++) {
        //dprintBuffer[f * 8] = 1;
        //dprintBuffer[f * 8 + 1] = 1;
        //dprintBuffer[f * 8 + 2] = 1;
        dprintBuffer[f * 8 + 3] = 1;
    }
    steppaper = 1;
    printLine();
    
    paperStep();
    delay_1ms(1);
    paperStop();

    delay_1ms(100);
    paperStep();
    delay_1ms(1);
    paperStop();
    printLine();
    delay_1ms(5);
    for(int f = 0; f < 40; f++) {
        paperStep();
        delay_us(1400);
        paperStep();
        delay_us(1400);
    }
    paperStop();

    //侧边栏
    for(int f = 0; f < 80; f++) {
        memset(&dprintBuffer, 0, sizeof dprintBuffer);
        for(int k = 0; k < 160; k++) {
            dprintBuffer[1728 - k - 80] = a4buffer[f][k];
        }
        for(int k = 0; k < 1400; k++) {
            dprintBuffer[1728 - k - 160-80] = sidebar12[f][k];
        }
        paperStep();
        delay_1ms(1);
        paperStop();
        printLine();
    }

    //主+顶
    for(int i = 0; i < 800 - 80; i++) {
        memset(&dprintBuffer, 0, sizeof dprintBuffer);
        for(int k = 0; k < 160; k++) {
            dprintBuffer[1728 - k - 80] = a4buffer[i + 80][k];
        }
        int n = 0;
        if(i == 0) {
            n = 160 + 120 - pl1[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 2 - pl2[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 3 - pl3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 4 - pavr[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 5 - pavl[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 6 - pavf[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 7 - pv1[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 8 - pv2[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 9 - pv3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 10 - pv4[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 11 - pv5[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 12 - pv6[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
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
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl1[i]; k <= pl1[i - 1]; k++) {
                    n = 160 + 120 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //II
            if(pl2[i] > pl2[i - 1]) {
                for(int k = pl2[i - 1]; k <= pl2[i]; k++) {
                    n = 160 + 120 * 2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl2[i]; k <= pl2[i - 1]; k++) {
                    n = 160 + 120 * 2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //III
            if(pl3[i] > pl3[i - 1]) {
                for(int k = pl3[i - 1]; k <= pl3[i]; k++) {
                    n = 160 + 120 * 3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl3[i]; k <= pl3[i - 1]; k++) {
                    n = 160 + 120 * 3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //aVR
            if(pavr[i] > pavr[i - 1]) {
                for(int k = pavr[i - 1]; k <= pavr[i]; k++) {
                    n = 160 + 120 * 4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavr[i]; k <= pavr[i - 1]; k++) {
                    n = 160 + 120 * 4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //aVL
            if(pavl[i] > pavl[i - 1]) {
                for(int k = pavl[i - 1]; k <= pavl[i]; k++) {
                    n = 160 + 120 * 5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavl[i]; k <= pavl[i - 1]; k++) {
                    n = 160 + 120 * 5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //aVF
            if(pavf[i] > pavf[i - 1]) {
                for(int k = pavf[i - 1]; k <= pavf[i]; k++) {
                    n = 160 + 120 * 6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavf[i]; k <= pavf[i - 1]; k++) {
                    n = 160 + 120 * 6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }




            //v1
            if(pv1[i] > pv1[i - 1]) {
                for(int k = pv1[i - 1]; k <= pv1[i]; k++) {
                    n = 160 + 120 * 7 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv1[i]; k <= pv1[i - 1]; k++) {
                    n = 160 + 120 * 7 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v2
            if(pv2[i] > pv2[i - 1]) {
                for(int k = pv2[i - 1]; k <= pv2[i]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv2[i]; k <= pv2[i - 1]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v3
            if(pv3[i] > pv3[i - 1]) {
                for(int k = pv3[i - 1]; k <= pv3[i]; k++) {
                    n = 160 + 120 * 9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv3[i]; k <= pv3[i - 1]; k++) {
                    n = 160 + 120 * 9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v4
            if(pv4[i] > pv4[i - 1]) {
                for(int k = pv4[i - 1]; k <= pv4[i]; k++) {
                    n = 160 + 120 * 10 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv4[i]; k <= pv4[i - 1]; k++) {
                    n = 160 + 120 * 10 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v5
            if(pv5[i] > pv5[i - 1]) {
                for(int k = pv5[i - 1]; k <= pv5[i]; k++) {
                    n = 160 + 120 * 11 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv5[i]; k <= pv5[i - 1]; k++) {
                    n = 160 + 120 * 11 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v6
            if(pv6[i] > pv6[i - 1]) {
                for(int k = pv6[i - 1]; k <= pv6[i]; k++) {
                    n = 160 + 120 * 12 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv6[i]; k <= pv6[i - 1]; k++) {
                    n = 160 + 120 * 12 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
        }
        paperStep();
        delay_1ms(1);
        paperStop();
        printLine();
    }
    //纯主
    for(int i = 720; i < 2000; i++) {
        memset(&dprintBuffer, 0, sizeof dprintBuffer);
        int n = 0;
        if(i == 0) {
            n = 160 + 120 - pl1[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 2 - pl2[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 3 - pl3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 4 - pavr[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 5 - pavl[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 6 - pavf[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 7 - pv1[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 8 - pv2[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 9 - pv3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 10 - pv4[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 11 - pv5[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 12 - pv6[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
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
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl1[i]; k <= pl1[i - 1]; k++) {
                    n = 160 + 120 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //II
            if(pl2[i] > pl2[i - 1]) {
                for(int k = pl2[i - 1]; k <= pl2[i]; k++) {
                    n = 160 + 120 * 2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl2[i]; k <= pl2[i - 1]; k++) {
                    n = 160 + 120 * 2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //III
            if(pl3[i] > pl3[i - 1]) {
                for(int k = pl3[i - 1]; k <= pl3[i]; k++) {
                    n = 160 + 120 * 3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl3[i]; k <= pl3[i - 1]; k++) {
                    n = 160 + 120 * 3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //aVR
            if(pavr[i] > pavr[i - 1]) {
                for(int k = pavr[i - 1]; k <= pavr[i]; k++) {
                    n = 160 + 120 * 4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavr[i]; k <= pavr[i - 1]; k++) {
                    n = 160 + 120 * 4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //aVL
            if(pavl[i] > pavl[i - 1]) {
                for(int k = pavl[i - 1]; k <= pavl[i]; k++) {
                    n = 160 + 120 * 5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavl[i]; k <= pavl[i - 1]; k++) {
                    n = 160 + 120 * 5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //aVF
            if(pavf[i] > pavf[i - 1]) {
                for(int k = pavf[i - 1]; k <= pavf[i]; k++) {
                    n = 160 + 120 * 6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavf[i]; k <= pavf[i - 1]; k++) {
                    n = 160 + 120 * 6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }




            //v1
            if(pv1[i] > pv1[i - 1]) {
                for(int k = pv1[i - 1]; k <= pv1[i]; k++) {
                    n = 160 + 120 * 7 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv1[i]; k <= pv1[i - 1]; k++) {
                    n = 160 + 120 * 7 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v2
            if(pv2[i] > pv2[i - 1]) {
                for(int k = pv2[i - 1]; k <= pv2[i]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv2[i]; k <= pv2[i - 1]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v3
            if(pv3[i] > pv3[i - 1]) {
                for(int k = pv3[i - 1]; k <= pv3[i]; k++) {
                    n = 160 + 120 * 9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv3[i]; k <= pv3[i - 1]; k++) {
                    n = 160 + 120 * 9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v4
            if(pv4[i] > pv4[i - 1]) {
                for(int k = pv4[i - 1]; k <= pv4[i]; k++) {
                    n = 160 + 120 * 10 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv4[i]; k <= pv4[i - 1]; k++) {
                    n = 160 + 120 * 10 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v5
            if(pv5[i] > pv5[i - 1]) {
                for(int k = pv5[i - 1]; k <= pv5[i]; k++) {
                    n = 160 + 120 * 11 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv5[i]; k <= pv5[i - 1]; k++) {
                    n = 160 + 120 * 11 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //v6
            if(pv6[i] > pv6[i - 1]) {
                for(int k = pv6[i - 1]; k <= pv6[i]; k++) {
                    n = 160 + 120 * 12 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv6[i]; k <= pv6[i - 1]; k++) {
                    n = 160 + 120 * 12 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
        }
        paperStep();
        delay_1ms(1);
        paperStop();
        printLine();
    }
    delay_1ms(20);
    //尾部
    for(int f = 0; f < 200; f++) {
        paperStep();
        delay_us(1400);
        paperStep();
        delay_us(1400);
    }
    paperStop();
    //排纸
    for(int f = 0; f < 40; f++) {
        paperStep();
        delay_us(1400);
        paperStep();
        delay_us(1400);
    }
    paperStop();
}

/*
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    SEGGER_RTT_Init();
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "start run\r\n");

    LCD_Init();
    POINT_COLOR = RED;
    LCD_DrawRectangle(229-6,199,229+24*13,199+24+2,BLUE);
    LCD_ShowString(1,230,200,480,80,24,1,"Waiting for Hardware Init");
    init12Lead();
    LCD_Clear(1,WHITE);
    LCD_DrawRectangle(229-6,199,229+24*13,199+24+2,BLUE);
    LCD_ShowString(1,230,200,480,80,24,1,"Calling System Software");


  /*
    	//创建开始任务
    xTaskCreate((TaskFunction_t )App_12lead_main,            //任务函数
                (const char*    )"12Lead",          //任务名称
                (uint16_t       )1,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )1,       //任务优先级
                (TaskHandle_t*  )0);   //任务句柄
    vTaskStartScheduler();          //开启任务调度

  App_12lead_main(0);
*/