#include "stdbool.h"
#include "gd32f4xx.h"
#include "12leadapp.h"
#include "monitor.h"
#include "gd32f4xx_gpio.h"

extern uint8_t nowapp,nowpage;
extern uint8_t destorysign;

extern bool isiced;
extern bool start10s;
extern uint8_t viewpage;
extern uint8_t selectapp;

extern bool nibpsig;

#define MODE_A 1
#define MODE_B 3
#define MODE_C 3
#define MODE_D 3

void App_Core();

void keyHandler(int keynum);
void confirmHandler();
void valueHandler(bool val);