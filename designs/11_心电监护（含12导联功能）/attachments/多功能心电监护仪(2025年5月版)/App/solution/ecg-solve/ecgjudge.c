#include "ecgjudge.h"

/*******************************************************************************
 * @fn Init_lowPass_alpha
 * @brief 初始化低通滤波器滤波系数
 * @param filter 滤波器
 * @param ts 采用周期 单位s
 * @return fc 截至频率 单位hz
 ******************************************************************************/
float bkb;
void Init_lowPass_alpha(low_pass_filter_t* const filter, const float ts, const float fc) {
    bkb = 2 * M_PI * fc * ts;
    filter->ts = ts;
    filter->fc = fc;
    filter->lastYn = 0;
    filter->alpha = bkb / (bkb + 1);
}

/*******************************************************************************
 * @fn Low_pass_filter
 * @brief 低通滤波函数
 * @param data 采样数据
 * @return 滤波结果
 ******************************************************************************/
float tem;
float Low_pass_filter(low_pass_filter_t* const filter, const float data) {
    tem = filter->lastYn + (filter->alpha * (data - filter->lastYn));
    filter->lastYn = tem;
    return tem;

}


/*******************************************************************************
 * @fn Init_hightPass_alpha
 * @brief 初始高通滤波器滤波系数
 * @param filter 滤波器
 * @param ts 采用周期 单位s
 * @return fc 截至频率 单位hz
 ******************************************************************************/
void Init_hightPass_alpha(hight_pass_filter_t* const filter, const float ts, const float fc) {
    bkb = 2 * M_PI * fc * ts;
    filter->ts = ts;
    filter->fc = fc;
    filter->lastYn = 0;
    filter->lastXn = 0;
    filter->alpha = 1 / (1 + bkb);
}

/*******************************************************************************
 * @fn Hight_pass_filter
 * @brief 高通滤波函数
 * @param data 采样数据
 * @return 滤波结果
 ******************************************************************************/
float Hight_pass_filter(hight_pass_filter_t* const filter, const float data) {
    tem = ((filter->alpha) * (filter->lastYn)) + ((filter->alpha) * (data - (filter->lastXn)));
    filter->lastYn = tem;
    filter->lastXn = data;
    return tem;

}

int calcrange(int a, int b) {
    if(a == b) {
        return 0;
    }
    if(a > b) {
        return a - b;
    }
    if(a < b) {
        return b - a;
    }
}



//输入值
extern float pl1[2000], pl2[2000], pl3[2000], pavr[2000], pavl[2000], pavf[2000], pv1[2000], pv5[2000];
//处理中
float spl2[2000], spl3[2000];

//索引
int newix;
int waveindexfxnew[800];

//推算过程
int v1max, v1min, v5max, v5min = 0;
int v1maxsum, v1minsum, v5maxsum, v5minsum = 0;
int v1zero, v5zero = 0;

//分析结果
int led12_s_RR = 0;
int led12_s_QT = 0;
int led12_s_QTc = 0;

int led12_s_P = 0;
int led12_s_QRS = 0;
int led12_s_T = 0;

int led12_s_QRS_deg = 0;

float led12_s_RV1 = 0;
float led12_s_SV1 = 0;
float led12_s_RV5 = 0;
float led12_s_SV5 = 0;

int led12_s_HR = 0;

//滤波器和识别器配置参数
uint8_t ncfgc[16] = {0x54,0x4A,0x53,0x2D,0x48,0x4E,0x2D,0x34,0x31,0x30,0x2D,0x37,0x31,0x53,0x46,0x48};

void solveecg10s() {
    led12_s_RR = 0;
    led12_s_QT = 0;
    led12_s_QTc = 0;
    led12_s_P = 0;
    led12_s_QRS = 0;
    led12_s_T = 0;
    led12_s_QRS_deg = 0;
    led12_s_RV1 = 0;
    led12_s_SV1 = 0;
    led12_s_RV5 = 0;
    led12_s_SV5 = 0;
    led12_s_HR = 0;
  
    newix = 0;
    memset(&waveindexfxnew,0,sizeof waveindexfxnew);

    static double avg = 0;

    static int waveindex[900];
    static int waveindexfx[900];

    static int ixc = 0;
    static int ixc3 = 0;
    static bool wavenote = 0;
    static int bwix = 0;
    static int realrc = 0;
    static bool delsig = 0;

    static int bfi = 0;

    static int fxjd1, fxjd2, fxjd3 = 0;
    static int delta1, delta2, delta3 = 0;
    static int bdelta1, bdelta2, bdelta3 = 0;
    static float zero1, zero2, zero3 = 0;
    static int useqrs = 0;

    static int sum1, sum3 = 0;
    static int sumqrs = 0;
    static float degqrs = 0;

    static int tn = 0;
    static int tmin = 5000;
    static int tmax = -5000;
    static int usetqrs = 0;
    static int tminsum = 0;
    static int tmaxsum = 0;
    static bool istr = 0;
    static int maxt = 0;
    static int mint = 0;
    static int maxtsum, mintsum;
    static int stzero = 0;
    static int clnix[2000];
    static int clnsum = 0;
    
    //必要参数调试
    ncfgc[15] = 0b01010100;//0b00101100?
    

    //Henan Tian,J S for competition (CASTIC) 410-71
    //===============================
    //==QRS提取部分(II,III双重验证，但在后面)
    //===============================
    //II部分

    //初步滤波
    low_pass_filter_t low_pass_filter = {0};
    Init_lowPass_alpha(&low_pass_filter, 0.005f, 11);
    for(int f = 0; f < 2000; f++) {
        spl2[f] = Low_pass_filter(&low_pass_filter, pl2[f]);
    }


    hight_pass_filter_t hight_pass_filter = {0};
    Init_hightPass_alpha(&hight_pass_filter, 0.005f, 5);
    for(int f = 0; f < 2000; f++) {
        spl2[f] = Hight_pass_filter(&hight_pass_filter, spl2[f]);
    }

    //求均值
    for(int f = 0; f < 2000; f++) {
        avg += spl2[f];
        avg = avg / 2000;
    }

    //负向波形翻转
    //if(avg >= 0) { //主波向上
    //} else { //主波向下
    //    for(int f = 0; f < 2000; f++) {
    //        spl2[f] = -spl2[f];//一键翻转
    //        avg = -avg;
    //    }
    //}

    //反向波形有缘再见
    //for(int f = 0; f < 2000; f++) {
    //    spl2[f] -= avg;
    //    if(spl2[f] < 0) {
    //        spl2[f] = 0;
    //    }
    //}

    //去除矮小波形
    for(int f = 0; f < 2000; f++) {
        spl2[f] *= spl2[f];//^4
        spl2[f] /= 400;
        if(spl2[f] < 1) {
            spl2[f] = 0;
        }
    }


    //for(int f = 0; f < 2000; f++) {
    //SEGGER_RTT_printf(0, "%d\r\n", (int)(spl2[f] * 10));
    //delay_1ms(3);
    //}

    //===============================
    //==RR间期部分
    //===============================
    ixc = 0;
    ixc3 = 0;
    wavenote = 0;
    for(int f = 0; f < 2000; f++) {
        if(!wavenote) {
            if(spl2[f] > 1) {
                wavenote = 1;
                waveindex[ixc] = f * 5;
                //SEGGER_RTT_printf(0, "seek R t=%d\r\n", f*5);
                ixc++;
            }
        } else {
            if(spl2[f] < 1) {
                wavenote = 0;
            }
        }
    }

    /*

    wavenote = 0;
    for(int f = 0; f < 2000; f++) {
        if(!wavenote) {
            if(spl3[f] > 1) {
                wavenote = 1;
                waveindex3[ixc3] = f * 5;
                //SEGGER_RTT_printf(0, "seek R t=%d\r\n", f*5);
                ixc3++;
            }
        } else {
            if(spl3[f] < 1) {
                wavenote = 0;
            }
        }
    }*/


    if(ixc == 0) {
        //最后争取 宽限一半
        //Henan Tian,J S for competition (CASTIC) 410-71
        Init_lowPass_alpha(&low_pass_filter, 0.005f, 11);
        for(int f = 0; f < 2000; f++) {
            spl2[f] = Low_pass_filter(&low_pass_filter, pl2[f]);
        }

        Init_hightPass_alpha(&hight_pass_filter, 0.005f, 5);
        for(int f = 0; f < 2000; f++) {
            spl2[f] = Hight_pass_filter(&hight_pass_filter, spl2[f]);
        }

        for(int f = 0; f < 2000; f++) {
            spl2[f] *= spl2[f];//^4
            spl2[f] /= 80;
            if(spl2[f] < 1) {
                spl2[f] = 0;
            }
        }

        ixc = 0;
        ixc3 = 0;
        wavenote = 0;
        for(int f = 0; f < 2000; f++) {
            if(!wavenote) {
                if(spl2[f] > 1) {
                    wavenote = 1;
                    waveindex[ixc] = f * 5;
                    //SEGGER_RTT_printf(0, "seek R t=%d\r\n", f*5);
                    ixc++;
                }
            } else {
                if(spl2[f] < 1) {
                    wavenote = 0;
                }
            }
        }

        if(ixc == 0) {
            led12_s_HR = 0;
            led12_s_RR = 0;
            led12_s_QRS_deg = 360;
            return;
        }


    }

    //干扰值初步去除

    bwix = 0;
    for(int f = 0; f < ixc; f++) {
        //SEGGER_RTT_printf(0, "f=%d\r\n", f);
        if(f == 0) {
            bwix = waveindex[0];
        } else {
            //SEGGER_RTT_printf(0, "pre R f=%d\r\n", f);
            //if(waveindex[f] - 200 < bwix) {
            //    waveindex[f] = -1;
            //}
            //delsig = 0;
            //for(int j = 0; j < ixc3; j++) {
            //    if(waveindex[f] > waveindex3[j] - 50 && waveindex[f] < waveindex3[j] + 50) {
            //        delsig = 1;
            //        break;
            //    }
            //}
            //if(!delsig) {
            //    waveindex[f] = -1;
            //}

            bwix = waveindex[f];
        }
    }

    //for(int f = 0; f < ixc; f++) {
    //    SEGGER_RTT_printf(0, "Rwave t=%d\r\n", waveindex[f]);
    //}

    realrc = 0;
    //RR间期和心率开算
    /*
    for(int f = 0; f < ixc; f++) { //∑
        if(waveindex[f] != -1) {

            if(f != 0) {

                if(waveindex[f] > bwix) {
                    realrc++;
                    led12_s_RR += (waveindex[f] - bwix);
                }

            }
            bwix = waveindex[f];
        }

    }

    led12_s_RR /= realrc;
    led12_s_HR = 60000 / led12_s_RR;

    SEGGER_RTT_printf(0, "HR=%d RR=%d\r\n", led12_s_HR, led12_s_RR);*/

    //===============================================================
    //==补算RR，QT,QTc,与QRS,T时限,R根部,S位置与积分反三角几何法电轴
    //===============================================================

    //排除无效波形
    //整理数据
    bwix = 0;
    for(int m = 0; m < ixc; m++) {
        if(waveindex[m] != -1) {
            waveindexfx[bwix] = waveindex[m] / 5;
            bwix++;
        }
    }

    //for(int f = 0; f < ixc; f++) {
    //    SEGGER_RTT_printf(0, "Rwave t=%d\r\n", waveindexfx[f]);
    //}

    //排除平直无效型，前后3单位，双重验证，直接除全
    for(int m = 0; m < bwix; m++) {//II
        if(calcrange((int)pl2[waveindexfx[m] - 1], (int)pl2[waveindexfx[m]]) > 5) { //-1,0
            //SEGGER_RTT_printf(0, "II -1=%d 0=%d\r\n", (int)pl2[waveindexfx[m] - 1], (int)pl2[waveindexfx[m]]);
            continue;
        }
        if(calcrange((int)pl2[waveindexfx[m] - 2], (int)pl2[waveindexfx[m] - 1]) > 5) { //-2,-1
            //SEGGER_RTT_printf(0, "II -2=%d -1=%d\r\n", (int)pl2[waveindexfx[m] - 2], (int)pl2[waveindexfx[m] - 1]);
            continue;
        }
        if(calcrange((int)pl2[waveindexfx[m] - 3], (int)pl2[waveindexfx[m] - 2]) > 5) { //-3,-2
            //SEGGER_RTT_printf(0, "II -3=%d -2=%d\r\n", (int)pl2[waveindexfx[m] - 3], (int)pl2[waveindexfx[m] - 2]);
            continue;
        }

        if(calcrange((int)pl2[waveindexfx[m] + 0], (int)pl2[waveindexfx[m] + 1]) > 5) { //0,1
            //SEGGER_RTT_printf(0, "II 0=%d 1=%d\r\n", (int)pl2[waveindexfx[m] + 0], (int)pl2[waveindexfx[m] + 1]);
            continue;
        }
        if(calcrange((int)pl2[waveindexfx[m] + 1], (int)pl2[waveindexfx[m] + 2]) > 5) { //1,2
            //SEGGER_RTT_printf(0, "II 1=%d 2=%d\r\n", (int)pl2[waveindexfx[m] + 1], (int)pl2[waveindexfx[m] + 2]);
            continue;
        }
        if(calcrange((int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]) > 5) { //2,3
            //SEGGER_RTT_printf(0, "II 2=%d 3=%d\r\n", (int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]);
            continue;
        }

        if(calcrange((int)pl2[waveindexfx[m] - 2], (int)pl2[waveindexfx[m] + 0]) > 5) { //-2,1
            //SEGGER_RTT_printf(0, "II 2=%d 3=%d\r\n", (int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]);
            continue;
        }

        if(calcrange((int)pl2[waveindexfx[m] + 1], (int)pl2[waveindexfx[m] + 3]) > 5) { //1,3
            //SEGGER_RTT_printf(0, "II 2=%d 3=%d\r\n", (int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]);
            continue;
        }

        //SEGGER_RTT_printf(0, "Except II ix=%d range=%d\r\n", waveindexfx[m], calcrange((int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]));
        waveindexfx[m] = -1;
    }

    for(int m = 0; m < bwix; m++) {//III
        if(waveindexfx[m] != -1) {
            if(calcrange((int)pl3[waveindexfx[m] - 1], (int)pl3[waveindexfx[m]]) > 5) { //-1,0
                //SEGGER_RTT_printf(0, "III -1=%d 0=%d\r\n", (int)pl3[waveindexfx[m] - 1], (int)pl3[waveindexfx[m]]);
                continue;
            }
            if(calcrange((int)pl3[waveindexfx[m] - 2], (int)pl3[waveindexfx[m] - 1]) > 5) { //-2,-1
                //SEGGER_RTT_printf(0, "III -2=%d -1=%d\r\n", (int)pl3[waveindexfx[m] - 2], (int)pl3[waveindexfx[m] - 1]);
                continue;
            }
            if(calcrange((int)pl3[waveindexfx[m] - 3], (int)pl3[waveindexfx[m] - 2]) > 5) { //-3,-2
                //SEGGER_RTT_printf(0, "III -3=%d -2=%d\r\n", (int)pl3[waveindexfx[m] - 3], (int)pl3[waveindexfx[m] - 2]);
                //Henan Tian,J S for competition (CASTIC) 410-71
                continue;
            }

            if(calcrange((int)pl3[waveindexfx[m] + 0], (int)pl3[waveindexfx[m] + 1]) > 5) { //0,1
                //SEGGER_RTT_printf(0, "III 0=%d 1=%d\r\n", (int)pl3[waveindexfx[m] + 0], (int)pl3[waveindexfx[m] + 1]);
                continue;
            }
            if(calcrange((int)pl3[waveindexfx[m] + 1], (int)pl3[waveindexfx[m] + 2]) > 5) { //1,2
                //SEGGER_RTT_printf(0, "III 1=%d 2=%d\r\n", (int)pl3[waveindexfx[m] + 1], (int)pl3[waveindexfx[m] + 2]);
                continue;
            }
            if(calcrange((int)pl3[waveindexfx[m] + 2], (int)pl3[waveindexfx[m] + 3]) > 5) { //2,3
                //SEGGER_RTT_printf(0, "III 2=%d 3=%d\r\n", (int)pl3[waveindexfx[m] + 2], (int)pl3[waveindexfx[m] + 3]);
                continue;
            }

            if(calcrange((int)pl3[waveindexfx[m] - 2], (int)pl3[waveindexfx[m] + 0]) > 5) { //-2,1
                //SEGGER_RTT_printf(0, "II 2=%d 3=%d\r\n", (int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]);
                continue;
            }

            if(calcrange((int)pl3[waveindexfx[m] + 1], (int)pl3[waveindexfx[m] + 3]) > 5) { //1,3
                //SEGGER_RTT_printf(0, "II 2=%d 3=%d\r\n", (int)pl2[waveindexfx[m] + 2], (int)pl2[waveindexfx[m] + 3]);
                continue;
            }

            //SEGGER_RTT_printf(0, "Except III ix=%d range=%d\r\n", waveindexfx[m], calcrange((int)pl3[waveindexfx[m] + 2], (int)pl3[waveindexfx[m] + 3]));
            waveindexfx[m] = -1;
        }

    }

    //重新整理索引
    newix = 0;
    for(int m = 0; m < bwix; m++) {
        if(waveindexfx[m] != -1 && waveindexfx[m] != 0) {
            waveindexfxnew[newix] = waveindexfx[m];
            newix++;
        }
    }

    if(newix == 0) {
        led12_s_HR = 0;
        led12_s_RR = 0;
        led12_s_QRS_deg = 360;
        return;
    }

    realrc = 0;
    bwix = 0;
    //RR间期和心率开算
    for(int f = 0; f < newix; f++) { //∑
        if(waveindexfxnew[f] != -1) {

            if(f != 0) {

                if(waveindexfxnew[f] * 5 > bwix) {
                    realrc++;
                    led12_s_RR += (waveindexfxnew[f] * 5 - bwix);
                }

            }
            bwix = waveindexfxnew[f] * 5;
        }

    }

    led12_s_RR /= realrc;
    led12_s_HR = 60000 / led12_s_RR;

    //SEGGER_RTT_printf(0, "HR=%d RR=%d\r\n", led12_s_HR, led12_s_RR);

    //判断波形方向
    fxjd1 = 0;
    fxjd2 = 0;
    fxjd3 = 0;
    for(int m = 0; m < newix; m++) {
        if(pl1[waveindexfxnew[m] - 1] < pl1[waveindexfxnew[m] + 1]) {
            fxjd1++;
        } else {
            fxjd1--;
        }

        if(pl2[waveindexfxnew[m] - 1] < pl2[waveindexfxnew[m] + 1]) {
            fxjd2++;
        } else {
            fxjd2--;
        }

        if(pl3[waveindexfxnew[m] - 1] < pl3[waveindexfxnew[m] + 1]) {
            fxjd3++;
        } else {
            fxjd3--;
        }
    }

    //SEGGER_RTT_printf(0, "I=%d II=%d III=%d\r\n", fxjd1, fxjd2, fxjd3);
    //Henan Tian,J S for competition (CASTIC) 410-71

    //向左寻找QRS根部，零电位嗅探 变化值斜率绝对值与方向综合决策法 零点位要求平均值 斜率要从顶点跨越1单位，如 -2 0 +2跳过1 -1这样规避期间短变化
    //对于正向负向均可
    //0.无掉头直接平直 无Q波
    //`变化值方向掉头 要分类讨论:
    //1.变化值掉头，跨越斜率大 有Q波 但整体与p无关(分离)
    //2.变化值掉头，跨越斜率小 qrs紧跟着p
    //3.变化值斜率改变两次 有Q波且qrs紧跟着p 通常为SVT等，直接决策波谷为零点
    //对于2,3情况对p波寻找有意义，需要定义变化类型，方便p波寻找
    useqrs = 0;
    uint8_t dt = 0;
    for(int m = 0; m < newix; m++) {
        //SEGGER_RTT_printf(0, "Now QRS: %d\r\n", m);
        if(waveindexfxnew[m] < 100) {
            continue;
        }

        dt = 0;
        for(int k = waveindexfxnew[m]; k > 1; k--) {
            bdelta1 = delta1;
            bdelta2 = delta2;
            bdelta3 = delta3;
            delta1 = pl1[k] - pl1[k - 1];
            delta2 = pl2[k] - pl2[k - 1];
            delta3 = pl3[k] - pl3[k - 1];
            if(k != waveindexfxnew[m]) {
                if(abs(delta1) < 4 && abs(delta2) < 4 && abs(delta3) < 4) {//0.无掉头平直
                    if(abs(pl1[k - 1] - pl1[k - 2]) < 4 && abs(pl2[k - 1] - pl2[k - 2]) < 4 && abs(pl3[k - 1] - pl3[k - 2]) < 4) {
                        waveindexfxnew[m] = k - 1;
                        //SEGGER_RTT_printf(0, "Find Line\r\n");
                        zero1 += pl1[k - 2];
                        zero2 += pl2[k - 2];
                        zero3 += pl3[k - 2];
                        useqrs++;
                        break;
                    }
                }

                if(bdelta1 * delta1 < 0 || bdelta2 * delta2 < 0 || bdelta3 * delta3 < 0) { //识别掉头
                    if(dt >= 3) {
                        waveindexfxnew[m] = k;
                        zero1 += pl1[k];
                        zero2 += pl2[k];
                        zero3 += pl3[k];
                        useqrs++;
                        //SEGGER_RTT_printf(0, "Too many\r\n");
                        break;
                    }
                    dt++;
                    if(abs(pl1[k + 1] - pl1[k - 1]) < 3) { //判断斜率绝对值 bdelta为顶点
                        waveindexfxnew[m] = k - 2;
                        zero1 += pl1[k - 2];
                        zero2 += pl2[k - 2];
                        zero3 += pl3[k - 2];
                        useqrs++;
                        //SEGGER_RTT_printf(0, "Find p-QRS\r\n");
                        //Henan Tian,J S for competition (CASTIC) 410-71
                        break;
                    }

                }
            }
        }
    }

    zero1 /= useqrs;
    zero2 /= useqrs;
    zero3 /= useqrs;

    useqrs = 0;
    sumqrs = 0;
    sum1 = 0;
    sum3 = 0;
    degqrs = 0;
    //向后积分心电轴，寻找QRS时限 avg
    for(int m = 0; m < newix; m++) {
        if(waveindexfxnew[m] < 100) {
            continue;
        }

        dt = 0;
        useqrs++;

        for(int k = waveindexfxnew[m]; k < waveindexfxnew[m] + 50; k++) {
            bdelta1 = delta1;
            bdelta2 = delta2;
            bdelta3 = delta3;
            delta1 = pl1[k] - pl1[k - 1];
            delta2 = pl2[k] - pl2[k - 1];
            delta3 = pl3[k] - pl3[k - 1];
            if(abs(delta1) < 4 && abs(delta2) < 4 && abs(delta3) < 4 && k > (waveindexfxnew[m] + 4)) { //检测平直
                if(abs(pl1[k] - pl1[k + 2]) < 4 && abs(pl2[k] - pl2[k + 2]) < 4 && abs(pl3[k] - pl3[k + 2]) < 4) {
                    //SEGGER_RTT_printf(0, "QRS t=%d\r\n", 5 * (k - waveindexfxnew[m]));
                    sumqrs += 5 * (k - waveindexfxnew[m]);
                    break;
                }
            }

            if(abs(pl1[k] - pl1[k + 2]) < 4 && abs(pl1[k + 2] - pl1[k + 3]) < 4 && abs(pl3[k] - pl3[k + 2]) < 4 && abs(pl3[k + 2] - pl3[k + 3]) < 4 && k > (waveindexfxnew[m] + 6)) { //延续性斜率检测
                sumqrs += 5 * (k - waveindexfxnew[m]);
                //SEGGER_RTT_printf(0, "QRS t=%d\r\n", 5 * (k - waveindexfxnew[m]));
                break;
            }
            sum1 += pl1[k] - zero1;
            sum3 += pl3[k] - zero3;


        }

    }
    //解算心电轴与qrs时限
    led12_s_QRS = sumqrs / useqrs;
    //sum1 /= useqrs;
    //sum3 /= useqrs;
    //心电轴分类讨论
    if(sum1 == 0 && sum3 == 0) {
        led12_s_QRS_deg = 360;//不可能电轴
        //SEGGER_RTT_printf(0, "Imp\r\n");
    }

    if(sum1 > 0 && sum3 > 0) { //电轴不偏
        led12_s_QRS_deg = atan((float)(2 * sum3 + sum1) / 1.732 / sum1) * 180 / 3.14159265;
        //SEGGER_RTT_printf(0, "None\r\n");
    }

    if(sum1 < 0 && sum3 < 0) { //电轴不确定
        led12_s_QRS_deg = atan((float)(-2 * sum3 - sum1) / 1.732 / sum1) * 180 / 3.14159265 - 180;
        //SEGGER_RTT_printf(0, "Doubt\r\n");
    }

    if(sum1 > 0 && sum3 < 0) { //电轴左偏
        led12_s_QRS_deg = atan((float)(2 * sum3 - sum1) / 1.732 / sum1 * 180 / 3.14159265);
        //SEGGER_RTT_printf(0, "Left\r\n");
    }

    if(sum1 < 0 && sum3 > 0) { //电轴右偏
        led12_s_QRS_deg = 180 - atan((float)(-2 * sum3 - sum1) / 1.732 / sum1) * 180 / 3.14159265;
        //SEGGER_RTT_printf(0, "Right\r\n");
    }


    //通过II导联寻找T波 QT avg与QTc，推出T波时限(用avg算)
    //绿波
    low_pass_filter_t low_pass_filter2 = {0};
    Init_lowPass_alpha(&low_pass_filter2, 0.005f, 22.5);
    for(int f = 0; f < 2000; f++) {
        spl2[f] = Low_pass_filter(&low_pass_filter2, pl2[f]) * 2;
    }

    hight_pass_filter_t hight_pass_filter2 = {0};
    Init_hightPass_alpha(&hight_pass_filter2, 0.005f, 8);
    for(int f = 0; f < 2000; f++) {
        spl2[f] = Hight_pass_filter(&hight_pass_filter2, spl2[f]);
    }


    clnsum = 0;
    //清除可燃物(斜率>15的会导致极值识别立刻发生爆炸)
    for(int f = 0; f < 2000; f++) {
        if(f != 0) {
            if(abs(spl2[f] - spl2[f - 1]) > 13) {
                clnix[clnsum] = f;
                clnsum++;
            }
        }
    }

    for(int f = 0; f < clnsum; f++) {
        spl2[clnix[f]] = 0;
    }

    clnsum = 0;
    //继续清除40
    for(int f = 0; f < 2000; f++) {
        if(f != 0) {
            if(abs(spl2[f] - spl2[f - 1]) > 40) {
                clnix[clnsum] = f;
                clnsum++;
            }
        }
    }

    for(int f = 0; f < clnsum; f++) {
        spl2[clnix[f]] = 0;
    }

    //for(int f = 0;f < 2000;f++){
    //  SEGGER_RTT_printf(0, "%d\r\n", (int)spl2[f]);
    //  delay_1ms(2);
    //}

    tn = 0.56 * sqrt((float)led12_s_RR / 1000) * 1000 / 5;
    usetqrs = 0;
    maxt = 0;
    mint = 0;
    for(int f = 0; f < newix; f++) {
        if(waveindexfxnew[f] < 100 || waveindexfxnew[f] + tn > 1800) {
            continue;
        }
        usetqrs++;
        tmax = -5000;
        tmin = 5000;
        for(int k = 0; k < tn - 30; k++) { //f+5
            if(tmax < spl2[waveindexfxnew[f] + 30 + k]) {
                tmax = spl2[waveindexfxnew[f] + 30 + k];
                maxt = 30 + k;
            }

            if(tmin > spl2[waveindexfxnew[f] + 30 + k]) {
                tmin = spl2[waveindexfxnew[f] + 30 + k];
                mint = 30 + k; //xxxt 从Q到T波峰 需要创建追加函数
            }
        }
        tmaxsum += tmax;
        tminsum += tmin;
        maxtsum += maxt;
        mintsum += mint;
    }

    tmaxsum /= usetqrs;
    tminsum /= usetqrs;

    maxtsum /= usetqrs;
    mintsum /= usetqrs;

    if(abs(tmaxsum) > abs(tminsum)) {
        istr = 1;
        led12_s_QT = (maxtsum + (maxtsum / 11)) * 5;
    } else {
        istr = 0;
        led12_s_QT = (mintsum + (mintsum / 11)) * 5;
    }
    //Henan Tian,J S for competition (CASTIC) 410-71
    //SEGGER_RTT_printf(0, "Min %d,Max %d IsT_R=%x MaxTime=%d MinTime=%d\r\n", tminsum, tmaxsum, istr, maxtsum, mintsum);

    //解算QTc
    led12_s_QTc = (int)(((float)led12_s_QT / 1000) / sqrt((float)led12_s_RR / 1000) * 1000);
    //SEGGER_RTT_printf(0, "QTC = %d\r\n", (int)led12_s_QTc);

    //算出T
    led12_s_T = led12_s_QT - led12_s_QRS;


    //============================
    //==胸导联R波分析 avg
    //============================
    //最值法
    
    
    v1zero, v5zero = 0;
    v1maxsum, v1minsum, v5maxsum, v5minsum = 0;
    useqrs = 0;
    //SEGGER_RTT_printf(0, "BEFORE v1zero = %d v5zero = %d v1maxs = %d v1mins = %d v5maxs = %d v5mins = %d\r\n", v1zero, v5zero, (int)v1maxsum, (int)v1minsum, (int)v5maxsum, (int)v5minsum);
    alltozeros();
    //求出等电位
    for(int f = 0; f < newix; f++) {
        if(waveindexfxnew[f] < 100 || waveindexfxnew[f] + tn > 1800) {
            continue;
        }
        useqrs++;
        v1zero += pv1[waveindexfxnew[f] - 2];
        v5zero += pv5[waveindexfxnew[f] - 2];
    }
    v1zero /= useqrs;
    v5zero /= useqrs;

    //求与等电位相对最值
    useqrs = 0;

    for(int f = 0; f < newix; f++) {
        if(waveindexfxnew[f] < 100 || waveindexfxnew[f] + tn > 1800) {
            continue;
        }
        useqrs++;
        v1max, v5max = -5000;
        v1min, v5min = 5000;
        for(int k = waveindexfxnew[f] - 2; k < waveindexfxnew[f] + (led12_s_QRS / 5) + 6; k++) {
            //v1
            if(pv1[k] - v1zero < v1min) {
                v1min = pv1[k] - v1zero;
            }
            if(pv1[k] - v1zero > v1max) {
                v1max = pv1[k] - v1zero;
            }
            //v5
            if(pv5[k] - v5zero < v5min) {
                v5min = pv5[k] - v5zero;
            }
            if(pv5[k] - v5zero > v5max) {
                v5max = pv5[k] - v5zero;
            }
        }
        v1minsum += v1min;
        v1maxsum += v1max;
        v5minsum += v5min;
        v5maxsum += v5max;
        //SEGGER_RTT_printf(0,"V1Min add %d \r\n",(int) v1min);
        //SEGGER_RTT_printf(0,"V1Max add %d\r\n",(int) v1max);
    }

    v1minsum /= useqrs;
    v1maxsum /= useqrs;
    v5minsum /= useqrs;
    v5maxsum /= useqrs;

    //记录值转电压值
    led12_s_RV1 = abs(v1maxsum) * 0.0125;
    led12_s_SV1 = abs(v1minsum) * 0.0125;

    led12_s_RV5 = abs(v5maxsum) * 0.0125;
    led12_s_SV5 = abs(v5minsum) * 0.0125;

    //v1max, v1min, v5max, v5min = 0;
    v1maxsum, v1minsum, v5maxsum, v5minsum = 0;
    v1zero, v5zero = 0;

    //SEGGER_RTT_printf(0, "v1zero = %d v5zero = %d v1maxs = %d v1mins = %d v5maxs = %d v5mins = %d\r\n", v1zero, v5zero, (int)v1maxsum, (int)v1minsum, (int)v5maxsum, (int)v5minsum);

    //SEGGER_RTT_printf(0, "RV1 = %d SV1 = %d RV5 = %d SV5 = %d\r\n", (int)led12_s_RV1, (int)led12_s_SV1, (int)led12_s_RV5, (int)led12_s_SV5);
}

void alltozeros(){
  v1max = 0;
  v1min = 0;
  v5max = 0;
  v5min = 0;
  v1maxsum = 0;
  v1minsum = 0;
  v5maxsum = 0; 
  v5minsum = 0;
  v1zero = 0;
  v5zero = 0;
  //SEGGER_RTT_printf(0, "AFTER Force Zreo v1zero = %d v5zero = %d v1maxs = %d v1mins = %d v5maxs = %d v5mins = %d\r\n", v1zero, v5zero, (int)v1maxsum, (int)v1minsum, (int)v5maxsum, (int)v5minsum);
  //Henan Tian,J S for competition (CASTIC) 410-71
}