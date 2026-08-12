/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "esp_log.h"

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
/* Private function prototypes -----------------------------------------------*/
void char_to_hex(char* s, int l, char* d);

//弃用
void dump_hex(uint8_t *buff, const uint8_t *ptr, uint16_t buflen);

// void dump_hex(const uint8_t *ptr, uint16_t buflen);
