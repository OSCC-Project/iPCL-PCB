#include "gd32f4xx.h"
#include "printer.h"
#include "bsp_rtc.h"

extern uint8_t nowapp,nowpage;

extern int newix;
extern int waveindexfxnew[800];

extern bool a4buffer[800][160];

extern rtc_parameter_struct   rtc_initpara_time;

int App_12lead_main(void *pra);
int App_12lead_10s(void* pra);

int App_12lead_func_viewpage(uint8_t page);

void App_12lead_print10s();