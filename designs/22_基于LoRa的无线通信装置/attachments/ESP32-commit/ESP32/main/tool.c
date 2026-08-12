#include "tool.h"

/// @brief 将字符串转为十六进制格式
/// @param s string : string need to change
/// @param l int : 字符串长度
/// @param d string : 输出结果 len = len(input)*2+1
void char_to_hex(char *s, int l, char *d)
{
  const char hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  *(d + 2 * l + 2) = '\0';
  while (l--)
  {
    *(d + 2 * l + 1) = hex_table[(*(s + l)) & 0x0f];
    *(d + 2 * l) = hex_table[(*(s + l)) >> 4];
  }
}

void dump_hex(uint8_t *p, const uint8_t *ptr, uint16_t buflen)
{
  unsigned char *buf = (unsigned char *)ptr;
  int i, j;
  const char *TAG = "";
  char *buff = (char *)p;

  for (i = 0; i < buflen; i += 16)
  {
  }
  // ESP_LOGI(TAG, "%s", *buff);
}