#include "respjudge.h"
#include "12leadapp.h"
#include "SEGGER_RTT.h"
#include "ecgjudge.h"

extern float pv2[2000];
int spresp[2000];
int bspr,bbspr,bbbspr,spr = 0;

int rdc = 0;
bool downsig = 0;

int resp_s_RR =  -1;

void judgeresp(){
  rdc = 0;
  resp_s_RR = 0;
  for(int f = 0;f < 2000;f++){
    spresp[f] = pv2[f];
  }
  
  low_pass_filter_t low_pass_filter = {0};
  Init_lowPass_alpha(&low_pass_filter, 0.005f, 4);
  for(int f = 0; f < 2000; f++) {
      spresp[f] = Low_pass_filter(&low_pass_filter, spresp[f]);
  }


  hight_pass_filter_t hight_pass_filter = {0};
  Init_hightPass_alpha(&hight_pass_filter, 0.005f, 0.2);
  for(int f = 0; f < 2000; f++) {
      spresp[f] = Hight_pass_filter(&hight_pass_filter, spresp[f]);
  }
  
  for(int f = 0;f < 2000;f++){
    if(f == 0){
    }else if(f == 1){
      bspr = spresp[0];
      spr = spresp[1];
      spresp[1] = (spr+bspr)/2/7;
    }else if(f == 2){
      bbspr = bspr;
      bspr = spr;
      spr = spresp[2];
      spresp[2] = (bbspr+bspr+spr)/3/7;
    }else{
      bbbspr = bbspr;
      bbspr = bspr;
      bspr = spr;
      spr = spresp[f];
      spresp[f] = (bbspr+bspr+spr+bbbspr)/4/7;
    }
  }
 
  //for(int f = 0;f < 2000;f++){
  //  SEGGER_RTT_printf(0,"%d \r\n",spresp[f]);
  //  delay_us(1000);
  //}
  //SEGGER_RTT_printf(0,"=======================\r\n");
  for(int f = 1;f < 2000;f++){
    if(spresp[f] < spresp[f - 1]){
      if(downsig == 0){
        downsig = 1;
        resp_s_RR++;
      }
    }else{
      if(spresp[f] == spresp[f - 1]){
      }else{
        downsig = 0;
      }
    }
  }
  
  resp_s_RR--;//重值矫正
  
  if(resp_s_RR <=2 ){//无效数据
    resp_s_RR = 0;
  }else{
    resp_s_RR *= 3;//数据量20s
  }
}