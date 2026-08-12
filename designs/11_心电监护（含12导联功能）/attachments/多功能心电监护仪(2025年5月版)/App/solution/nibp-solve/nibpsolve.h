#include "gd32f4xx.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

extern int nibp_s_H;
extern int nibp_s_L;

extern int ct;
extern int args;
extern bool overmemory;
extern uint16_t nibpvals[3200];//越界报袖带过松

void solvenibp();
void readnibp(float nibp);