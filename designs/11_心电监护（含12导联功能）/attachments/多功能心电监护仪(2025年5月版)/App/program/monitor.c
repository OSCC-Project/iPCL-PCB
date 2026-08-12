#include "monitor.h"
#include "AppManager.h"
#include "ecg_monitor.h"
#include "resp_monitor.h"
#include "SPO2.h"
#include "bsp_rtc.h"
#include "screen_basic.h"
#include "12leadapp.h"
#include <stdio.h>
#include "12lead.h"
#include <string.h>
#include "ecgjudge.h"
#include "respjudge.h"
#include "nibpsolve.h"
#include "pressure.h"

bool ismoninited = 0;
extern int tpx, point;
extern bool isiced;
extern int dpxl1, dpxl2, dpxl3, dpxavr, dpxavl, dpxavf, dpxv1;
extern int bfl1, bfl2, bfl3, bfavr, bfavl, bfavf, bfv1;
int mssig = 0;
StructSPO2Data spd;
int spdat = 0;
int dpxspo2, bspo2;
int spo2, gbspo2, pi, bpi = 0;

bool drawresp = 0;
int dpxresp;
int bfres, dres = 0;
extern float fresp;

static uint8_t xuxian = 0;
extern bool figer, online;
extern int  tresp;
extern bool ecglead = 0;

extern char buffs[70];

extern float pl1[2000], pl2[2000], pl3[2000], pavr[2000], pavl[2000], pavf[2000], pv1[2000], pv2[2000], pv3[2000], pv4[2000];
extern bool steppaper;
int bs = 0;

int rpoint = 0;

bool nibpsig = 0;
bool nibpupdate = 0;
float nibp;
int gp = 0;

int App_monitor_main() {
    nowapp = 1;
    nowpage = 0;

    tpx = 0;
    point = 0;
    isiced = 0;
    mssig = 0;

    drawresp = 0;
    dpxresp = 0;
    xuxian = 0;
    rpoint = 0;

    nibpsig = 0;
    nibpupdate = 0;
    if(!ismoninited) {
        //监护前端初始化
        monecg_CS_L;
        monecg_START_L;
        delay_1ms(10);
        SPI_Transport_monecg(0x11);
        delay_1ms(300);
        uint8_t did = monecg_REG(0x20 | 0x00, 0x00);
        SEGGER_RTT_printf(0, "id=%x\r\n", did);
        delay_us(100);
        monecg_REG(0x40 | 0x01, 0b10000110);
        delay_us(100);
        monecg_REG(0x40 | 0x02, 0b00110101);
        delay_us(100);
        monecg_REG(0x40 | 0x03, 0b11001100);
        delay_us(100);
        //WCT
        monecg_REG(0x40 | 0x18, 0b00001001); //0b00001001
        delay_us(100);
        monecg_REG(0x40 | 0x19, 0b11011000); //0b11011000
        delay_us(100);
        //RLD
        monecg_REG(0x40 | 0x0d, 0b00000111);
        delay_us(100);
        monecg_REG(0x40 | 0x0e, 0b00000111);
        delay_us(100);


        //monecg_REG(0x40 | 0x05, 0b00000000);
        //delay_us(100);
        monecg_REG(0x40 | 0x08, 0b10000001);
        monecg_REG(0x40 | 0x09, 0b10000001);
        monecg_REG(0x40 | 0x0A, 0b10000001);
        monecg_REG(0x40 | 0x0B, 0b10000001);
        monecg_REG(0x40 | 0x0C, 0b10000001);
        delay_us(100);

        SPI_Transport_monecg(0x10);
        delay_us(100);
        SPI_Transport_monecg(0x08);
        monecg_START_H;
        delay_1ms(10);

        //呼吸前端初始化
        respmon_CS_L;
        respmon_START_L;
        delay_1ms(10);
        SPI_Transport_respmon(0x11);
        delay_1ms(100);
        did = respmon_REG(0x20 | 0x00, 0x00);
        SEGGER_RTT_printf(0, "Resp id=%x\r\n", did);

        respmon_REG(0x40 | 0x02, 0xA0);//b10101000
        delay_1ms(20);
        respmon_REG(0x40 | 0x01, 0x01);
        delay_us(100);
        //RESP
        respmon_REG(0x40 | 0x09, 0xC2); //0b00001001
        delay_us(100);
        respmon_REG(0x40 | 0x0A, 0x03); //0b11011000
        delay_us(100);

        //respmon_REG(0x40|0x03,       0XF0);
        //respmon_REG(0x40|0x04,     0X00);	//放大倍数6倍
        //respmon_REG(0x40|0x05,     0x00);
        //respmon_REG(0x40|0x06,   0x30);
        //respmon_REG(0x40|0x07,  0x3F);
        //respmon_REG(0x40|0x0B,       0x0C);

        SPI_Transport_respmon(0x10);
        delay_us(10);
        SPI_Transport_respmon(0x08);
        respmon_START_H;
        delay_1ms(100);
        ismoninited = 1;
    }

    //页面初始化
    SEGGER_RTT_printf(0, "12lead Task Draw\r\n");

    LCD_Clear(1, BLACK);

    LCD_SolidRectangle(0, 0, 799, 40, GRAY);

    POINT_COLOR = WHITE;
    LCD_ShowString(1, 640, 20, 800, 480, 16, 1, "2100-13-32 25:61:61");

    //案件菜单
    LCD_ShowString(1, 14, 443, 480, 800, 241, 1, "DE");//静音
    LCD_DrawRectangle(11, 440, 11 + 53, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82, 443, 480, 800, 241, 1, "FG"); //快照
    LCD_DrawRectangle(11 + 82, 440, 11 + 53 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82, 443, 480, 800, 241, 1, "45"); //冻结
    LCD_DrawRectangle(11 + 82 + 82, 440, 11 + 53 + 82 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82 + 82, 443, 480, 800, 241, 1, ")*"); //记录
    LCD_DrawRectangle(11 + 82 + 82 + 82, 440, 11 + 53 + 82 + 82 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82 + 82 + 82, 443, 480, 800, 241, 1, "HI"); //血压
    LCD_DrawRectangle(11 + 82 + 82 + 82 + 82, 440, 11 + 53 + 82 + 82 + 82 + 82, 440 + 29, CYAN);

    LCD_ShowString(1, 14 + 82 + 82 + 82 + 82 + 82, 443, 480, 800, 241, 1, "JK"); //更多
    LCD_DrawRectangle(11 + 82 + 82 + 82 + 82 + 82, 440, 11 + 53 + 82 + 82 + 82 + 82 + 82, 440 + 29, CYAN);

    //辅助线 2 2 2 1.5 1.5 在绘制里面也有
    //LCD_DrawLine(0, 40 + 90, 799, 40 + 90, RED);
    //LCD_DrawLine(0, 40 + 90 + 90, 799, 40 + 90 + 90, RED);
    //LCD_DrawLine(0, 40 + 90 + 90 + 90, 799, 40 + 90 + 90 + 90, RED);
    //LCD_DrawLine(0, 40 + 90 + 90 + 90 + 60, 799, 40 + 90 + 90 + 90 + 60, RED);
    //LCD_DrawLine(0, 40 + 90 + 90 + 90 + 60 + 60, 799, 40 + 90 + 90 + 90 + 60 + 60, RED);
    LCD_DrawLine(0, 40 + 90 + 90 + 90 + 60 + 60, 670, 40 + 90 + 90 + 90 + 60 + 60, GRAY);
    LCD_DrawLine(670, 41, 670, 479, GRAY);

    //NIBP
    LCD_ShowString(1, 2, 40 + 90 + 90 + 90 + 61, 800, 480, 16, 1, "NIBP");
    LCD_ShowString(1, 2, 40 + 90 + 90 + 90 + 61 + 17 + 4, 800, 480, 24, 1, "---/-- mmHg");



    //参数
    POINT_COLOR = CYAN;
    LCD_ShowString(1, 672, 40 + 90 + 90, 800, 480, 16, 1, "SpO2");
    LCD_ShowString(1, 672, 40 + 90 + 90 + 17, 800, 480, 48, 1, "-?-");
    LCD_ShowString(1, 672, 40 + 90 + 90 + 17 + 50, 800, 480, 16, 1, "PI: ---");

    POINT_COLOR = GREEN;
    LCD_ShowString(1, 672, 40, 800, 480, 16, 1, "HR");
    LCD_ShowString(1, 672, 40 + 17, 800, 480, 48, 1, "-?-");
    LCD_ShowString(1, 672, 40 + 17 + 50, 800, 480, 16, 1, "QTc:---");
    LCD_ShowString(1, 672 + 4 + 56, 40 + 17 + 50, 800, 480, 161, 1, "#B");
    LCD_ShowString(1, 672 + 4 + 56 + 32, 40 + 17 + 50, 800, 480, 16, 1, ":---");
    LCD_ShowString(1, 672, 40 + 17 + 50 + 18, 800, 480, 16, 1, "RV:-.--- mV");
    LCD_ShowString(1, 672, 40 + 17 + 50 + 18 + 18, 800, 480, 16, 1, "SV:-.--- mV");

    POINT_COLOR = YELLOW;
    LCD_ShowString(1, 672, 40 + 90 + 90 + 90, 800, 480, 16, 1, "RR");
    LCD_ShowString(1, 672, 40 + 90 + 90 + 90 + 17, 800, 480, 48, 1, "-?-");

    //命名
    POINT_COLOR = GREEN;
    LCD_ShowString(1, 2, 40, 800, 480, 16, 1, "II");
    LCD_ShowString(1, 2, 40 + 90, 800, 480, 16, 1, "V");
    POINT_COLOR = CYAN;
    LCD_ShowString(1, 2, 40 + 90 + 90, 800, 480, 16, 1, "Pluse");
    POINT_COLOR = YELLOW;
    LCD_ShowString(1, 2, 40 + 90 + 90 + 90, 800, 480, 16, 1, "RESP");
    LCD_DrawRectangle(20, 65, 21, 105, GREEN); //II
    LCD_DrawRectangle(20, 65 + 90, 21, 105 + 90, GREEN); //V

    while(1) {
        //II V Pluse为90px RESP为60px 电压基准1mV=40px 等电位线中点( 45px) 1px代表0.025mV
        //呼吸-100 至 +100 等电位线0为中点(30px) 共200个变化点 放在60视窗内 1 px= 3.3333point
        // V等电位线还要往上5px
        if(!isiced) {
            mssig++;
            if(mssig == 1) {
                SPO2Task();
            } else if(mssig == 2) {
                SPO2Task();
                SPO2AdjTask();
                GetSPO2Data(&spd);
                spdat = GetSPO2Wave_Prossed();
            } else if(mssig == 3) {
                SPO2Task();
            } else if(mssig == 4) {
                SPO2Task();
                SPO2AdjTask();
                GetSPO2Data(&spd);
                spdat = GetSPO2Wave_Prossed();
            } else if(mssig == 5) {
                SPO2Task();
                monitor_GetmV();

                if(point >= 2000) {
                    memmove(pl1, pl1 + 1, 7996);
                    memmove(pl2, pl2 + 1, 7996);
                    memmove(pl3, pl3 + 1, 7996);
                    memmove(pavr, pavr + 1, 7996);
                    memmove(pavl, pavl + 1, 7996);
                    memmove(pavf, pavf + 1, 7996);
                    memmove(pv1, pv1 + 1, 7996);
                    memmove(pv3, pv3 + 1, 7996);
                    pl1[1999] = fl1 / 0.0125;
                    pl2[1999] = fl2 / 0.0125;
                    pl3[1999] = fl3 / 0.0125;
                    pavr[1999] = favr / 0.0125;
                    pavl[1999] = favl / 0.0125;
                    pavf[1999] = favf / 0.0125;
                    pv1[1999] = fv1 / 0.0125;//Pluse2000放到120内
                    pv3[1999] = spdat / 8 - 80 - 120;
                } else {
                    pl1[point] = fl1 / 0.0125;
                    pl2[point] = fl2 / 0.0125;
                    pl3[point] = fl3 / 0.0125;
                    pavr[point] = favr / 0.0125;
                    pavl[point] = favl / 0.0125;
                    pavf[point] = favf / 0.0125;
                    pv1[point] = fv1 / 0.0125;
                    pv3[point] = spdat / 8 - 80 - 120;
                    point++;
                }

                if(nibpsig) {
                    nibp = pressValue();
                    if(gp == 1) {
                        readnibp(nibp);
                    }

                    if(nibp >= 260 && gp == 0) { //0 Set松 0 RESET紧
                        gp = 1;
                        gpio_bit_write(GPIOE, GPIO_PIN_1, RESET);
                        //delay_1ms(200);
                        gpio_bit_write(GPIOE, GPIO_PIN_0, SET);

                    }

                    if(nibp <= 36 && gp == 1) {
                        gpio_bit_write(GPIOE, GPIO_PIN_0, RESET);
                        gp = 0;
                        nibpsig = 0;
                        solvenibp();
                        nibpupdate = 1;
                    }
                }
            } else if(mssig == 6) {
                SPO2Task();
                SPO2AdjTask();
                GetSPO2Data(&spd);
                spdat = GetSPO2Wave_Prossed();
            } else if(mssig == 7) {
                SPO2Task();
            } else if(mssig == 8) {
                SPO2Task();
                SPO2AdjTask();
                GetSPO2Data(&spd);
                spdat = GetSPO2Wave_Prossed();
            } else if(mssig == 9) {
                SPO2Task();
            } else if(mssig == 10) {
                SPO2Task();
                SPO2AdjTask();
                GetSPO2Data(&spd);
                spdat = GetSPO2Wave_Prossed();

                monitor_GetmV();

                if(point >= 2000) {
                    memmove(pl1, pl1 + 1, 7996);
                    memmove(pl2, pl2 + 1, 7996);
                    memmove(pl3, pl3 + 1, 7996);
                    memmove(pavr, pavr + 1, 7996);
                    memmove(pavl, pavl + 1, 7996);
                    memmove(pavf, pavf + 1, 7996);
                    memmove(pv1, pv1 + 1, 7996);
                    memmove(pv3, pv3 + 1, 7996);
                    pl1[1999] = fl1 / 0.0125;
                    pl2[1999] = fl2 / 0.0125;
                    pl3[1999] = fl3 / 0.0125;
                    pavr[1999] = favr / 0.0125;
                    pavl[1999] = favl / 0.0125;
                    pavf[1999] = favf / 0.0125;
                    pv1[1999] = fv1 / 0.0125;//Pluse2000放到120内
                    pv3[1999] = spdat / 8 - 80 - 120;

                } else {
                    pl1[point] = fl1 / 0.0125;
                    pl2[point] = fl2 / 0.0125;
                    pl3[point] = fl3 / 0.0125;
                    pavr[point] = favr / 0.0125;
                    pavl[point] = favl / 0.0125;
                    pavf[point] = favf / 0.0125;
                    pv1[point] = fv1 / 0.0125;
                    pv3[point] = spdat / 8 - 80 - 120;
                    point++;
                }
                getRespValue();

                if(nibpsig) {
                    nibp = pressValue();
                    if(gp == 1) {
                        readnibp(nibp);
                    }

                    if(nibp >= 260 && gp == 0) { //0 Set松 0 RESET紧
                        gp = 1;
                        gpio_bit_write(GPIOE, GPIO_PIN_1, RESET);
                        //delay_1ms(200);
                        gpio_bit_write(GPIOE, GPIO_PIN_0, SET);

                    }

                    if(nibp <= 36 && gp == 1) {
                        gpio_bit_write(GPIOE, GPIO_PIN_0, RESET);
                        gp = 0;
                        nibpsig = 0;
                        solvenibp();
                        nibpupdate = 1;
                    }
                }

                xuxian++;
                if(xuxian >= 5) {
                    xuxian = 1;
                }
                //RESP临界tr>9700000为脱落
                if(tresp > 9700000 || tresp < 60000) {
                    ecglead = 0;
                } else {
                    ecglead = 1;
                }

                dpxl1 = fl1 / -0.025;
                dpxl2 = fl2 / -0.025;
                dpxl3 = fl3 / -0.025;
                dpxavr = favr / -0.025;
                dpxavl = favl / -0.025;
                dpxavf = favf / -0.025;
                dpxv1 = fv1 / -0.025;

                if(40 + 45 + dpxl2 <= 41) {
                    dpxl2 = 28 - 28 - 44;
                } else if(40 + 45 + dpxl2 >= 219) {
                    dpxl2 = 220 - 40 - 46;
                }

                if(40 + 45 + dpxv1 <= 41) {
                    dpxv1 = 28 - 28 - 44;
                } else if(40 + 45 + dpxv1 >= 219) {
                    dpxv1 = 220 - 40 - 46;
                }

                //单个导联显示长度为px
                if(tpx >= 669) {
                    tpx = 0;
                    LCD_DrawRectangle(tpx + 1, 41, tpx + 1 + 1, 369 - 60, BLACK);
                    LCD_DrawRectangle(tpx + 2, 41, tpx + 1 + 2, 369 - 60, BLACK);
                }



                if(ecglead) {
                    //I
                    LCD_DrawLine(2 + tpx - 1, 40 + 45 + bfl2 + 5, 2 + tpx, 40 + 45 + dpxl2 + 5, GREEN);

                    //bbfl1 = bfl1;

                    //II
                    LCD_DrawLine(2 + tpx - 1, 40 + 90 + 45 + bfv1 - 5, 2 + tpx, 40 + 90 + 45 + dpxv1 - 5, GREEN);

                    //bbfl2 = bfl2;

                    //实时分析
                    if(tpx % 130 == 0 && point >= 2000) {

                        solveecg10s();
                        if(led12_s_QTc > 999) {
                            led12_s_QTc = 999;
                        }
                        POINT_COLOR = GREEN;
                        LCD_SolidRectangle(672, 40 + 17, 672 + 75, 40 + 17 + 50, BLACK);
                        if(led12_s_HR > 999) {
                            LCD_ShowString(1, 672, 40 + 17, 800, 480, 48, 1, "999");
                        } else {
                            sprintf(buffs, "%d", led12_s_HR);
                            LCD_ShowString(1, 672, 40 + 17, 800, 480, 48, 1, buffs);
                            LCD_SolidRectangle(672, 40 + 17 + 50, 672 + 64 - 8, 40 + 17 + 50 + 18, BLACK);
                            sprintf(buffs, "QTc:%d", led12_s_QTc);
                            LCD_ShowString(1, 672, 40 + 17 + 50, 800, 480, 16, 1, buffs);
                            LCD_SolidRectangle(672 + 4 + 56 + 32, 40 + 17 + 50, 672 + 4 + 56 + 32 + 32 + 3, 40 + 17 + 50 + 18, BLACK);
                            sprintf(buffs, ":%d", led12_s_QRS_deg);
                            LCD_ShowString(1, 672 + 4 + 56 + 32, 40 + 17 + 50, 800, 480, 16, 1, buffs);

                            LCD_SolidRectangle(672, 40 + 17 + 50 + 18, 800, 40 + 17 + 50 + 18 + 18 + 18, BLACK);
                            sprintf(buffs, "RV:%.3f mV", led12_s_RV1);
                            LCD_ShowString(1, 672, 40 + 17 + 50 + 18, 800, 480, 16, 1, buffs);
                            sprintf(buffs, "SV:%.3f mV", led12_s_SV1);
                            LCD_ShowString(1, 672, 40 + 17 + 50 + 18 + 18, 800, 480, 16, 1, buffs);
                        }
                    }
                } else {
                    if(xuxian <= 2) {
                        LTDC_Draw_Point(1, 2 + tpx, 40 + 45, GREEN);
                        LTDC_Draw_Point(1, 2 + tpx, 40 + 90 + 45, GREEN);
                    }
                    if(tpx % 130 == 0 && point >= 2000) {
                        POINT_COLOR = GREEN;
                        LCD_SolidRectangle(672, 40 + 17 + 50, 672 + 64 - 8, 40 + 17 + 50 + 18, BLACK);
                        LCD_ShowString(1, 672, 40 + 17 + 50, 800, 480, 16, 1, "QTc:---");
                        LCD_SolidRectangle(672, 40 + 17, 672 + 75, 40 + 17 + 50, BLACK);
                        LCD_ShowString(1, 672, 40 + 17, 800, 480, 48, 1, "-?-");
                        LCD_SolidRectangle(672 + 4 + 56 + 32, 40 + 17 + 50, 672 + 4 + 56 + 32 + 32 + 3, 40 + 17 + 50 + 18, BLACK);
                        LCD_ShowString(1, 672 + 4 + 56 + 32, 40 + 17 + 50, 800, 480, 16, 1, ":---");
                        LCD_SolidRectangle(672, 40 + 17 + 50 + 18, 800, 40 + 17 + 50 + 18 + 18 + 18, BLACK);
                        LCD_ShowString(1, 672, 40 + 17 + 50 + 18, 800, 480, 16, 1, "RV:-.--- mV");
                        LCD_ShowString(1, 672, 40 + 17 + 50 + 18 + 18, 800, 480, 16, 1, "SV:-.--- mV");
                    }
                }
                bfl2 = dpxl2;
                bfv1 = dpxv1;


                //Pluse
                if(!online || !figer) {
                    if(xuxian <= 2) {
                        LTDC_Draw_Point(1, 2 + tpx, 40 + 90 + 90 + 45, CYAN);
                    }
                } else {
                    dpxspo2 = (spdat / -11.1111) + 45;
                    LCD_DrawLine(2 + tpx - 1, 40 + 90 + 90 + 45 + bspo2, 2 + tpx, 40 + 90 + 90 + 45 + dpxspo2, CYAN);

                }
                bspo2 = dpxspo2;

                //RESP
                drawresp = !drawresp;
                if(drawresp) {
                    if(dpxresp >= 669) {
                        dpxresp = 0;
                        LCD_DrawRectangle(1, 41 + 90 + 90 + 90, 1 + 1, 369, BLACK);
                        LCD_DrawRectangle(2, 41 + 90 + 90 + 90, 1 + 2, 369, BLACK);
                    }
                    if(fresp > 100) {
                        dres = 100 / 3.3333;
                    } else if(fresp < -100) {
                        dres = -100 / 3.3333;
                    } else {
                        dres = fresp / 3.3333;
                    }

                    if(ecglead) {
                        LCD_DrawLine(2 + dpxresp - 1, 40 + 90 + 90 + 90 + 30 + bfres, 2 + dpxresp, 40 + 90 + 90 + 90 + 30 + dres, YELLOW);
                    } else {
                        if(xuxian <= 1) {
                            LTDC_Draw_Point(1, 2 + dpxresp, 40 + 90 + 90 + 90 + 45, YELLOW);
                        }
                    }

                    bfres = dres;
                    LCD_DrawRectangle(dpxresp + 2 + 1, 40 + 90 + 90 + 90, dpxresp + 2 + 2, 370, BLACK);
                    dpxresp++;

                    if(rpoint >= 2000) {
                        memmove(pv2, pv2 + 1, 7996);
                        memmove(pv4, pv4 + 1, 7996);
                        pv2[1999] = fresp;
                        pv4[1999] = dres * 2 - 40 - 120; //100 -100 共200 放到
                    } else {
                        pv2[point] = fresp;
                        pv4[point] = dres * 2 - 40 - 120;
                        rpoint++;
                    }

                    //RESP分析
                    if(dpxresp % 200 == 0 && rpoint >= 2000) {
                        POINT_COLOR = YELLOW;
                        if(ecglead) {
                            judgeresp();
                            LCD_SolidRectangle(672, 40 + 90 + 90 + 90 + 17, 799, 40 + 90 + 90 + 90 + 17 + 50, BLACK);
                            sprintf(buffs, "%d", resp_s_RR);
                            LCD_ShowString(1, 672, 40 + 90 + 90 + 90 + 17, 800, 480, 48, 1, buffs);
                        } else {
                            LCD_SolidRectangle(672, 40 + 90 + 90 + 90 + 17, 799, 40 + 90 + 90 + 90 + 17 + 50, BLACK);
                            LCD_ShowString(1, 672, 40 + 90 + 90 + 90 + 17, 800, 480, 48, 1, "-?-");
                        }

                    }

                }

                //NIBP
                if(nibpupdate) {
                    nibpupdate = 0;
                    POINT_COLOR = WHITE;
                    LCD_SolidRectangle(2, 40 + 90 + 90 + 90 + 61 + 17 + 4, 2 + 180, 40 + 90 + 90 + 90 + 61 + 17 + 4 + 26, BLACK);
                    if(nibp_s_H == -1) {
                        sprintf(buffs, "--/");
                    } else {
                        sprintf(buffs, "%d/", nibp_s_H);
                    }

                    if(nibp_s_L == -1) {
                        sprintf(buffs, "%s-- mmHg", buffs);
                    } else {
                        sprintf(buffs, "%s%d mmHg", buffs, nibp_s_L);
                    }
                    LCD_ShowString(1, 2, 40 + 90 + 90 + 90 + 61 + 17 + 4, 800, 480, 24, 1, buffs);
                }

                //TIME
                rtc_current_time_get(&rtc_initpara_time);
                if(BcdToDecimal(rtc_initpara_time.second) != bs) {
                    bs = BcdToDecimal(rtc_initpara_time.second);
                    LCD_SolidRectangle(640, 20, 799, 20 + 16, GRAY);
                    POINT_COLOR = WHITE;
                    sprintf(buffs, "20%d-", BcdToDecimal(rtc_initpara_time.year)); //, BcdToDecimal(rtc_initpara_time.month), BcdToDecimal(rtc_initpara_time.date),BcdToDecimal(rtc_initpara_time.hour),  BcdToDecimal(rtc_initpara_time.minute), BcdToDecimal(rtc_initpara_time.second)
                    if(BcdToDecimal(rtc_initpara_time.month) < 10) {
                        sprintf(buffs, "%s0%d-", buffs, BcdToDecimal(rtc_initpara_time.month));
                    } else {
                        sprintf(buffs, "%s%d-", buffs, BcdToDecimal(rtc_initpara_time.month));
                    }
                    if(BcdToDecimal(rtc_initpara_time.date) < 10) {
                        sprintf(buffs, "%s0%d ", buffs, BcdToDecimal(rtc_initpara_time.date));
                    } else {
                        sprintf(buffs, "%s%d ", buffs, BcdToDecimal(rtc_initpara_time.date));
                    }
                    if(BcdToDecimal(rtc_initpara_time.hour) < 10) {
                        sprintf(buffs, "%s0%d:", buffs, BcdToDecimal(rtc_initpara_time.hour));
                    } else {
                        sprintf(buffs, "%s%d:", buffs, BcdToDecimal(rtc_initpara_time.hour));
                    }
                    if(BcdToDecimal(rtc_initpara_time.minute) < 10) {
                        sprintf(buffs, "%s0%d:", buffs, BcdToDecimal(rtc_initpara_time.minute));
                    } else {
                        sprintf(buffs, "%s%d:", buffs, BcdToDecimal(rtc_initpara_time.minute));
                    }
                    if(BcdToDecimal(rtc_initpara_time.second) < 10) {
                        sprintf(buffs, "%s0%d", buffs, BcdToDecimal(rtc_initpara_time.second));
                    } else {
                        sprintf(buffs, "%s%d", buffs, BcdToDecimal(rtc_initpara_time.second));
                    }

                    LCD_ShowString(1, 640, 20, 800, 480, 16, 1, buffs);
                }



                LCD_DrawRectangle(tpx + 2 + 1, 41, tpx + 2 + 2, 369 - 60, BLACK);
                POINT_COLOR = GREEN;
                LCD_ShowString(1, 2, 40, 800, 480, 16, 1, "II");
                LCD_ShowString(1, 2, 40 + 90, 800, 480, 16, 1, "V");
                POINT_COLOR = CYAN;
                LCD_ShowString(1, 2, 40 + 90 + 90, 800, 480, 16, 1, "Pluse");
                POINT_COLOR = YELLOW;
                LCD_ShowString(1, 2, 40 + 90 + 90 + 90, 800, 480, 16, 1, "RESP");
                LCD_DrawRectangle(20, 65, 21, 105, GREEN); //II
                LCD_DrawRectangle(20, 65 + 90, 21, 105 + 90, GREEN); //V
                LCD_DrawLine(670, 41, 670, 479, GRAY);//辅助线

                //SpO2绘制
                POINT_COLOR = CYAN;
                spo2 = spd.SPO2;
                pi = spd.PI;
                if(spo2 != gbspo2) {
                    LCD_SolidRectangle(672, 40 + 90 + 90 + 17, 672 + 75, 40 + 90 + 90 + 17 + 50, BLACK);
                    if(spo2 == -1) {
                        LCD_ShowString(1, 672, 40 + 90 + 90 + 17, 800, 480, 48, 1, "-?-");
                    } else {
                        sprintf(buffs, "%d", spo2);
                        LCD_ShowString(1, 672, 40 + 90 + 90 + 17, 800, 480, 48, 1, buffs);
                    }
                }
                if(pi != bpi) {
                    LCD_SolidRectangle(672, 40 + 90 + 90 + 17 + 50, 672 + 80, 40 + 90 + 90 + 17 + 50 + 18, BLACK);
                    if(pi == -1) {
                        LCD_ShowString(1, 672, 40 + 90 + 90 + 17 + 50, 800, 480, 16, 1, "PI: ---");
                    } else {
                        sprintf(buffs, "PI: %d%", pi);
                        LCD_ShowString(1, 672, 40 + 90 + 90 + 17 + 50, 800, 480, 16, 1, buffs);
                    }
                }
                gbspo2 = spo2;
                bpi = pi;


                tpx++;
                mssig = 0;
            }
        }

        delay_1ms(1);

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
            selectapp = MODE_A;
        }

        if(selectapp != nowapp) {
            destorysign++;
            if(destorysign >= 250) {
                destorysign = 0;
                return selectapp;
            }
        } else {
            destorysign = 0;
        }
    }
}

void App_monitor_SnapShot() {
    clearA4();
    rtc_current_time_get(&rtc_initpara_time);
    //信息页
    sprintf(buffs, "20%d-%d-%d %d:%d:%d", BcdToDecimal(rtc_initpara_time.year), BcdToDecimal(rtc_initpara_time.month), BcdToDecimal(rtc_initpara_time.date), BcdToDecimal(rtc_initpara_time.hour),  BcdToDecimal(rtc_initpara_time.minute), BcdToDecimal(rtc_initpara_time.second));
    A4_DrawString(1, 1, 800, 120, 24, 1, buffs);
    A4_DrawString(1, 40, 800, 120, 24, 1, "Monitor SNAPSHOT 7+2");
    A4_DrawString(1, 80, 800, 120, 24, 1, "NAME:");
    if(nibp_s_H == -1){
        A4_DrawString(1, 120, 800, 160, 24, 1, "NIBP:_____/_____ mmHg");
    }else{
        sprintf(buffs,"NIBP: %d/%d mmHg",nibp_s_H,nibp_s_L);
        A4_DrawString(1, 120, 800, 160, 24, 1, buffs);
    }
    
    //概要
    A4_DrawString(281, 1, 800, 120, 241, 1, "\"A"); //HR
    sprintf(buffs, ": %d", led12_s_HR);
    A4_DrawString(281 + 48, 1, 800, 120, 24, 1, buffs);

    sprintf(buffs, "SpO2: %d", spo2);
    A4_DrawString(281, 1 + 32, 800, 120, 24, 1, buffs); //SpO2

    sprintf(buffs, "PI: %d", pi);
    A4_DrawString(281, 1 + 32 + 32, 800, 120, 24, 1, buffs); //PI

    sprintf(buffs, "RespR: %d", resp_s_RR);
    A4_DrawString(281, 1 + 32 + 32 + 32, 800, 120, 24, 1, buffs); //RR

    sprintf(buffs, ": %d deg", led12_s_QRS_deg);
    A4_DrawString(281, 1 + 32 + 32 + 32 + 32, 800, 120, 24, 1, "QRS"); //QRS
    A4_DrawString(281 + 36, 1 + 32 + 32 + 32 + 32, 800, 120, 241, 1, "#B"); //电轴
    A4_DrawString(281 + 36 + 48, 1 + 32 + 32 + 32 + 32, 800, 120, 24, 1, buffs); //电轴


    //概要：下一页
    sprintf(buffs, "QT: %d ms", led12_s_QT);
    A4_DrawString(481, 1, 800, 120, 24, 1, buffs); //QT

    sprintf(buffs, "QTc: %d ms", led12_s_QTc);
    A4_DrawString(481, 1 + 32, 800, 120, 24, 1, buffs); //QTc

    sprintf(buffs, "RV: %.3f mV", (led12_s_RV1));
    A4_DrawString(481, 1 + 32 + 32, 800, 120, 24, 1, buffs); //RV

    sprintf(buffs, "SV: %.3f mV", (led12_s_SV1));
    A4_DrawString(481, 1 + 32 + 32 + 32, 800, 120, 24, 1, buffs); //SV

    memset(&sidebar12, 0, sizeof sidebar12);

    //侧边栏
    Side_DrawString(1, 1, 80, 1400, 24, 1, "I");
    Side_DrawString(1, 1 + 120, 80, 1400, 24, 1, "II");
    Side_DrawString(1, 1 + 120 + 120, 80, 1400, 24, 1, "III");
    Side_DrawString(1, 1 + 120 + 120 + 120, 80, 1400, 24, 1, "aVR");
    Side_DrawString(1, 1 + 120 + 120 + 120 + 120, 80, 1400, 24, 1, "aVL");
    Side_DrawString(1, 1 + 120 + 120 + 120 + 120 + 120, 80, 1400, 24, 1, "aVF");
    Side_DrawString(1, 1 + 120 + 120 + 120 + 120 + 120 + 120, 80, 1400, 24, 1, "V");
    Side_DrawString(1, 1 + 120 + 120 + 120 + 120 + 120 + 120 + 120+120 , 80, 1400, 24, 1, "Pluse");
    Side_DrawString(1, 1 + 120 + 120 + 120 + 120 + 120 + 120 + 120 + 120+ 120, 80, 1400, 24, 1, "Resp");

    //基准左
    for(int f = 0; f < 16; f++) {
        sidebar12[f + 32][80] = 1;
        sidebar12[f + 32][80 + 120] = 1;
        sidebar12[f + 32][80 + 240] = 1;
        sidebar12[f + 32][80 + 360] = 1;
        sidebar12[f + 32][80 + 480] = 1;
        sidebar12[f + 32][80 + 600] = 1;
        sidebar12[f + 32][80 + 720] = 1;
        //sidebar12[f + 32][80 + 840] = 1;
        //sidebar12[f + 32][80 + 960] = 1;
        //sidebar12[f + 32][80 + 1080] = 1;
        //sidebar12[f + 32][80 + 1200] = 1;
        //sidebar12[f + 32][80 + 1319] = 1;
    }
    //基准右
    for(int f = 0; f < 16; f++) {
        sidebar12[f + 64][80] = 1;
        sidebar12[f + 64][80 + 120] = 1;
        sidebar12[f + 64][80 + 240] = 1;
        sidebar12[f + 64][80 + 360] = 1;
        sidebar12[f + 64][80 + 480] = 1;
        sidebar12[f + 64][80 + 600] = 1;
        sidebar12[f + 64][80 + 720] = 1;
    }
    //基准顶
    for(int f = 0; f < 16; f++) {
        sidebar12[f + 48][0] = 1;
        sidebar12[f + 48][120] = 1;
        sidebar12[f + 48][240] = 1;
        sidebar12[f + 48][360] = 1;
        sidebar12[f + 48][480] = 1;
        sidebar12[f + 48][600] = 1;
        sidebar12[f + 48][720] = 1;
    }
    //画线
    for(int f = 0; f < 7; f++) {
        for(int k = 0; k < 4; k++) {
            sidebar12[48][120 * f + k * 24] = 1;
            sidebar12[48][120 * f + k * 24 + 1] = 1;
            sidebar12[48][120 * f + k * 24 + 2] = 1;
            sidebar12[48][120 * f + k * 24 + 3] = 1;
            sidebar12[48][120 * f + k * 24 + 4] = 1;
            sidebar12[48][120 * f + k * 24 + 5] = 1;
            sidebar12[48][120 * f + k * 24 + 6] = 1;
            sidebar12[48][120 * f + k * 24 + 7] = 1;
        }
    }

    for(int f = 0; f < 7; f++) {
        for(int k = 0; k < 4; k++) {
            sidebar12[64][120 * f + k * 24] = 1;
            sidebar12[64][120 * f + k * 24 + 1] = 1;
            sidebar12[64][120 * f + k * 24 + 2] = 1;
            sidebar12[64][120 * f + k * 24 + 3] = 1;
            sidebar12[64][120 * f + k * 24 + 4] = 1;
            sidebar12[64][120 * f + k * 24 + 5] = 1;
            sidebar12[64][120 * f + k * 24 + 6] = 1;
            sidebar12[64][120 * f + k * 24 + 7] = 1;
        }
    }


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
            dprintBuffer[1728 - k - 160 - 80] = sidebar12[f][k];
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
            n = 160 + 120 - pl1[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 2 - pl2[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 3 - pl3[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 4 - pavr[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 5 - pavl[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 6 - pavf[i] / 2;
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
            n = 160 + 120 * 8 - pv3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 9 - pv4[i];//RESP
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
        } else {
            //I
            if(pl1[i] > pl1[i - 1]) {
                for(int k = pl1[i - 1] / 2; k <= pl1[i] / 2; k++) {
                    n = 160 + 120 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl1[i] / 2; k <= pl1[i - 1] / 2; k++) {
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
                for(int k = pl2[i - 1] / 2; k <= pl2[i] / 2; k++) {
                    n = 160 + 120 * 2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl2[i] / 2; k <= pl2[i - 1] / 2; k++) {
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
                for(int k = pl3[i - 1] / 2; k <= pl3[i] / 2; k++) {
                    n = 160 + 120 * 3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl3[i] / 2; k <= pl3[i - 1] / 2; k++) {
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
                for(int k = pavr[i - 1] / 2; k <= pavr[i] / 2; k++) {
                    n = 160 + 120 * 4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavr[i] / 2; k <= pavr[i - 1] / 2; k++) {
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
                for(int k = pavl[i - 1] / 2; k <= pavl[i] / 2; k++) {
                    n = 160 + 120 * 5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavl[i] / 2; k <= pavl[i - 1] / 2; k++) {
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
                for(int k = pavf[i - 1] / 2; k <= pavf[i] / 2; k++) {
                    n = 160 + 120 * 6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavf[i] / 2; k <= pavf[i - 1] / 2; k++) {
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
            //Pluse
            if(pv3[i] > pv3[i - 1]) {
                for(int k = pv3[i - 1]; k <= pv3[i]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv3[i]; k <= pv3[i - 1]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //RESP
            if(pv4[i] > pv4[i - 1]) {
                for(int k = pv4[i - 1]; k <= pv4[i]; k++) {
                    n = 160 + 120 * 9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv4[i]; k <= pv4[i - 1]; k++) {
                    n = 160 + 120 * 9 - k;
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
            n = 160 + 120 - pl1[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 2 - pl2[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 3 - pl3[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 4 - pavr[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 5 - pavl[i] / 2;
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 6 - pavf[i] / 2;
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
            n = 160 + 120 * 8 - pv3[i];
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
            dprintBuffer[1728 - n] = 1;
            n = 160 + 120 * 9 - pv4[i];//RESP
            if(n > 1727) {
                n = 1727;
            } else if(n < 0) {
                n = 0;
            };
        } else {
            //I
            if(pl1[i] > pl1[i - 1]) {
                for(int k = pl1[i - 1] / 2; k <= pl1[i] / 2; k++) {
                    n = 160 + 120 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl1[i] / 2; k <= pl1[i - 1] / 2; k++) {
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
                for(int k = pl2[i - 1] / 2; k <= pl2[i] / 2; k++) {
                    n = 160 + 120 * 2 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl2[i] / 2; k <= pl2[i - 1] / 2; k++) {
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
                for(int k = pl3[i - 1] / 2; k <= pl3[i] / 2; k++) {
                    n = 160 + 120 * 3 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pl3[i] / 2; k <= pl3[i - 1] / 2; k++) {
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
                for(int k = pavr[i - 1] / 2; k <= pavr[i] / 2; k++) {
                    n = 160 + 120 * 4 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavr[i] / 2; k <= pavr[i - 1] / 2; k++) {
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
                for(int k = pavl[i - 1] / 2; k <= pavl[i] / 2; k++) {
                    n = 160 + 120 * 5 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavl[i] / 2; k <= pavl[i - 1] / 2; k++) {
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
                for(int k = pavf[i - 1] / 2; k <= pavf[i] / 2; k++) {
                    n = 160 + 120 * 6 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pavf[i] / 2; k <= pavf[i - 1] / 2; k++) {
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
            //Pluse
            if(pv3[i] > pv3[i - 1]) {
                for(int k = pv3[i - 1]; k <= pv3[i]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv3[i]; k <= pv3[i - 1]; k++) {
                    n = 160 + 120 * 8 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            }
            //RESP
            if(pv4[i] > pv4[i - 1]) {
                for(int k = pv4[i - 1]; k <= pv4[i]; k++) {
                    n = 160 + 120 * 9 - k;
                    if(n > 1727) {
                        n = 1727;
                    } else if(n < 0) {
                        n = 0;
                    };
                    dprintBuffer[1728 - n] = 1;
                }
            } else {
                for(int k = pv4[i]; k <= pv4[i - 1]; k++) {
                    n = 160 + 120 * 9 - k;
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