#include "AppManager.h"
#include "SEGGER_RTT.h"
#include "pressure.h"


uint8_t nowapp = 0;
uint8_t nowpage = 0;
uint8_t destorysign = 0;
uint8_t selectapp = 0;


int appret = 3;
void App_Core(){
    //初始化识别
    if(gpio_input_bit_get(GPIOE,GPIO_PIN_10) == RESET){
      appret = MODE_A;
    }else if(gpio_input_bit_get(GPIOE,GPIO_PIN_11) == RESET){
      appret = MODE_B;
    }else if(gpio_input_bit_get(GPIOE,GPIO_PIN_12) == RESET){
      appret = MODE_C;
    }else if(gpio_input_bit_get(GPIOE,GPIO_PIN_13) == RESET){
      appret = MODE_D;
    }else{
      appret = MODE_A;
    }
    while(1){
      SEGGER_RTT_printf(0, "appret = %d\r\n",appret);
      switch(appret){
        case 3://12导联主
          appret = App_12lead_main(0);
          break;
        case 31://12导联10s查看
          appret = App_12lead_10s(0);
          break;
        case 31001://12导联10s翻页
          appret = App_12lead_func_viewpage(viewpage);
          break;
        
        
        
        case 1://监护主
          appret = App_monitor_main();
          break;
      }
    }
}

void keyHandler(int keynum) {
    if(nowapp == 3) { //12导联
        if(nowpage == 0) { //主界面
            switch(keynum) {
            case 1://冻结
                isiced = !isiced;
                break;
            case 2://分析
                break;
            case 3://10s
                start10s = 1;
                break;
            }
        } else if(nowpage == 1) {
            switch(keynum) {
            case 1://上页
                if(viewpage > 1) {
                  viewpage--;
                }
                break;
            case 2://下页
              if(viewpage < 6) {
                  viewpage++;
                }
                break;
            case 3://打印
                App_12lead_print10s();
                break;
            case 4:
                viewpage = 20;
                break;
            }
        }
    }else if(nowapp == 1){//监护仪
        if(nowpage == 0){
            switch(keynum){
                case 1://静音
                    break;
                case 2://快照
                    App_monitor_SnapShot();
                    break;
                case 3://冻结
                    isiced = !isiced;
                    break;
                case 4://记录
                    break;
                case 5://血压
                    testReset();
                    nibpsig = 1;
                    break;
                case 6://更多
                    break;
            }
        }
    }
}