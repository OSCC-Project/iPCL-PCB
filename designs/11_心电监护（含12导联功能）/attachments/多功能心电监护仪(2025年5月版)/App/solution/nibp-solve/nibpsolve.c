#include "nibpsolve.h"
#include "ecgjudge.h"

int nibp_s_H = -1;
int nibp_s_L = -1;

int args = 0;//从0开始
int ct = 0;
bool overmemory = 0;
uint16_t nibpvals[3200];//越界报袖带过松
int slnibp[3200];

int b, bb, bbb, bbbb, bbbbb;
int nb, nbb, nbbb, nbbbb, nbbbbb;


int pointidx[50];
int rise = 0;
bool dsig = 0;
bool rsig = 0;
int dcount = 0;
uint8_t points = 0;
int stp = 0;
int ava,avab = 0;
int psspidx[50];
int resultidx[70];
int idxmaxium = 0;
int maxfidx = 0;

static int hix = 0;
static int lix = 0;

low_pass_filter_t nbplow_pass_filter = {0};
hight_pass_filter_t nbphight_pass_filter = {0};
void solvenibp() {
    dsig = 0;
    if(overmemory) {
        int nibp_s_H = -1;
        int nibp_s_L = -1;
        return;
    }

    
    for(int f = 0; f < args; f++) {//去除左边竖线
        if(slnibp[f] > slnibp[f - 1]) {
            for(int k = 0; k <= f; k++) {
                slnibp[k] = slnibp[f + 1];
            }
            break;
        } else {
            
        }
    }
    
    for(int f = 0;f < 130;f++){
        if(slnibp[f] > -1){
            slnibp[f] = -1;
        }else{
            break;
        }
    }
    //for(int f = 1; f < args; f++) {
    //    slnibp[f] = slnibp[f]*slnibp[f]*(-1) / 2 / 8 / 2;//先测试/2
    //}
    
    //for(int f = 0; f < args; f++) {
    //    SEGGER_RTT_printf(0, "%d\r\n", slnibp[f]);
    //    delay_1ms(1);
    //}
    
    SEGGER_RTT_printf(0, "===============Here are Indexs=====\r\n");
    
     
    
    //以4作为阈值，给一个1的扰动宽限，取上升
    rise = 0;
    dsig = 0;
    rsig = 0;
    stp = 0;
    points = 0;
    //取最大值            
    idxmaxium = -11451;
    maxfidx = 0;
    dcount = 0;
    for(int f = 1;f < args;f++){
        if(slnibp[f - 1] <= slnibp[f]){//上升或者不变
            if(slnibp[f - 1] < slnibp[f]){//上升清除下降标识
                dsig = 0;
            }
            
            //注册起始点和最大值
            if(stp == -1){
                stp = f;
                idxmaxium = -11451;
                maxfidx = 0;
                dcount = 0;
            }
            
            //寻找最大值
            if(slnibp[f] > idxmaxium){
                idxmaxium = slnibp[f];
                maxfidx = f;
            }
            
            rise += slnibp[f] - slnibp[f - 1];//[那个取值应该取波峰，而不是起点或终点。已改好]
            if(!rsig && rise >= 10){//阈值达到，注入列表
                rsig = 1;
                //pointidx[points] = f;
                //points++;
            }
            
        }else{//检测到下降
            dcount++;
            if(slnibp[f - 1] - slnibp[f] >=2){//下降过多
                if(rsig){
                    pointidx[points] = maxfidx;
                    points++;
                }
                rise = 0;//重新计数
                rsig = 0;
                stp = -1;
                dcount = 0;
            }
            if(dsig){//持续下降
                if(rsig){
                    pointidx[points] = maxfidx;
                    points++;
                }
                rise = 0;//重新计数
                rsig = 0;
                stp = -1;
                dcount = 0;
            }else{
                dsig = 1;
                rise--;
            }
            
            if(dcount >= 4){//丢弃数据
                //不保存数据
                rise = 0;//重新计数
                rsig = 0;
                stp = -1;
                dcount = 0;
            }
        }
    }
    
    for(int f = 0;f < points;f++){
        SEGGER_RTT_printf(0, "%d\r\n", pointidx[f]);
        //delay_1ms(1);
    }
    
    //干扰排除
    //直接搜索 - 间隔1个搜索（两种结果后者优先，直接从后往前好） 宽限40的偏差。
    //最后取大的
    ava = 0;
    avab = 0;
    //直接搜索
    for(int f = 2;f < points;f++){//                                                                    符合逻辑全都错了，不是前一个与这一个，而是相差改变的数值，   有一个成立就有三个连贯
        if(abs((pointidx[f] - pointidx[f - 1]) - (pointidx[f - 1] - pointidx[f - 2])) <= 30){//符合
            //SEGGER_RTT_printf(0, "Plan A: available  i=%d and %d and %d\r\n", pointidx[f - 2], pointidx[f - 1], pointidx[f]);
            //delay_1ms(1);
            ava++;
        }else{
            if(ava >= 1){
                break;
            }else{
                ava = 0;
            }
        }
    }
    
    //按照points间隔搜索
    for(int f = 1;f < points - 1;f++){
        //删除点整理数据 rise已释放
        rise = 0;
        for(int k = 0;k < points;k++){
            if( k == f){
                
            }else{
                psspidx[rise] = pointidx[k];
                rise++;
            }
        }
        
        //继续对比
        avab = 0;
        for(int g = 2;g < rise;g++){
            if(abs((psspidx[g] - psspidx[g - 1]) - (psspidx[g - 1] - psspidx[g - 2])) <= 30){//符合
                //SEGGER_RTT_printf(0, "Plan B: available  i=%d and %d and %d\r\n", psspidx[g - 2], psspidx[g - 1], psspidx[g]);
                avab++;
                //delay_1ms(1);
            }else{
                if(avab >= 1){
                    break;
                }else{
                    avab = 0;
                }
            }
        }
        
        //给出结果
        resultidx[f] = avab;
    }
    
    //从后往前挑选间隔最大值
    idxmaxium = -2;
    maxfidx = 0;
    for(int f = points - 1;f > 0;f--){
        if(resultidx[f] > idxmaxium){
            idxmaxium = resultidx[f];
            maxfidx = f;
        }
    }
    
    //对比间隔与直接
    hix = -1;
    lix = -1;
    
    if(idxmaxium >= ava){//选间隔
        SEGGER_RTT_printf(0, "Plan B better idx=%d   use f =%d\r\n", idxmaxium,maxfidx);
        //删除点整理数据 rise已释放
        rise = 0;
        for(int k = 0;k < points;k++){
            if( k == maxfidx){
                
            }else{
                psspidx[rise] = pointidx[k];
                rise++;
            }
        }
        
        //继续对比
        avab = 0;
        for(int g = 2;g < rise;g++){
            if(abs((psspidx[g] - psspidx[g - 1]) - (psspidx[g - 1] - psspidx[g - 2])) <= 30){//符合
                //SEGGER_RTT_printf(0, "Plan B: available  i=%d and %d and %d\r\n", psspidx[g - 2], psspidx[g - 1], psspidx[g]);
                avab++;
                //delay_1ms(1);
                if(hix == -1){
                    hix = psspidx[g - 2];
                    //SEGGER_RTT_printf(0, "Hix = %d sy = %d\r\n", hix,(g-2));
                    //delay_1ms(1);
                }
                lix = psspidx[g];
            }else{
                if(avab >= 1){
                    break;
                }else{
                    avab = 0;
                }
            }
        }
        
    }else{//选直接
        ava = 0;//释放ava
        SEGGER_RTT_printf(0, "Plan A better idx=%d\r\n", ava);
        for(int f = 2;f < points;f++){//有一个成立就有三个连贯
            if(abs((pointidx[f] - pointidx[f - 1]) - (pointidx[f - 1] - pointidx[f - 2])) <= 30){//符合
                //SEGGER_RTT_printf(0, "Plan A: available  i=%d and %d and %d\r\n", pointidx[f - 2], pointidx[f - 1], pointidx[f]);
                if(hix == -1){
                    hix = pointidx[f - 2];
                    //SEGGER_RTT_printf(0, "Hix = %d\r\n", hix);
                    //delay_1ms(1);
                }
                lix = pointidx[f];
                ava++;
            }else{
                if(ava >= 1){
                    break;
                }else{
                    ava = 0;
                }
            }
        }
    }
    
    delay_1ms(1);
    nibp_s_H = nibpvals[hix] / 10;
    nibp_s_L = nibpvals[lix] / 10;
    if(hix == -1){
        nibp_s_H = -1;//袖带过松或未接
    }
    if(lix == -1){
        nibp_s_L = -1;
    }
    SEGGER_RTT_printf(0, "HP:%d mmHg  LP:%d mmHg    Hix=%d   Lix = %d\r\n",nibp_s_H,nibp_s_L,hix,lix);
    
    
   

}

void readnibp(float nibp) {
    if(ct <= 15) {
        ct++;
        return;
    }
    if(args >= 3200) {
        overmemory = 1;
    } else {
        nibpvals[args] = (uint16_t)(nibp * 10);
        slnibp[args] = Low_pass_filter(&nbplow_pass_filter, nibpvals[args]);
        //slnibp[args] = Hight_pass_filter(&nbphight_pass_filter,slnibp[args]);
        slnibp[args] = nibpvals[args] - slnibp[args];
        
        if(args == 0) {
            b = slnibp[args];
        } else if(args == 1) {
            bb = b;
            b = slnibp[args];
            slnibp[args] = (bb + b) / 2;
        } else if(args == 2) {
            bbb = bb;
            bb = b;
            b = slnibp[args];
            slnibp[args] = (bbb + bb + b) / 3;
        } else if(args == 3) {
            bbbb = bbb;
            bbb = bb;
            bb = b;
            b = slnibp[args];
            slnibp[args] = (bbbb + bbb + bb + b) / 4;
        } else {
            bbbbb = bbbb;
            bbbb = bbb;
            bbb = bb;
            bb = b;
            b = slnibp[args];
            slnibp[args] = (bbbbb + bbbb + bbb + bb + b) / 5;
        }

        if(args == 0) {
            nb = slnibp[args];
        } else if(args == 1) {
            nbb = nb;
            nb = slnibp[args];
            slnibp[args] = (nbb + nb) / 2;
        } else {
            nbbb = nbb;
            nbb = nb;
            nb = slnibp[args];
            slnibp[args] = (nbbb + nbb + nb) / 3;
        }

        slnibp[args] = (slnibp[args] + slnibp[args - 2]) / 2;
        slnibp[args] = (slnibp[args] + slnibp[args - 3]) / 2;
        //if(args != 0){
        //    slnibp[args - 1] = (slnibp[args] + slnibp[args - 2]) / 2;
        //}
        //SEGGER_RTT_printf(0, "%d\r\n",slnibp[args]);

    }

    args++;
}
