/* Includes ------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "tool.h"

#include "conf.h"
/*define ---------------------------------------------------------------------*/
// LoRa define


#define LoRa_REV_QUEUE_LEN 100
#define LoRa_UART_NUM UART_NUM_1
#define LoRa_TXD_Pin 23
#define LoRa_RXD_Pin 22
#define LoRa_M0_Pin GPIO_NUM_25
#define LoRa_M1_Pin GPIO_NUM_26
#define LoRa_AUX_Pin GPIO_NUM_34

/*Private struct ----------------------------------------------------------*/
typedef enum
{
  STATEMENT_UNKNOWN = 0, /*!< Unknown statement */
  STATEMENT_REPORT
} LoRa_data_cmd;

typedef struct
{
  LoRa_data_cmd cmd; // 指令
  char src[7];       // 源地址
  char dst[7];       // 目的地址

  char time[7]; // 时间
  char longitude[13];
  char latitude[13];

  char date1[10]; // 数据1 temperature
  char date2[10]; // 数据2 chlorophyll
  char date3[10]; // 数据3 turbidity
  char date4[10]; // 数据4 BatteryVoltage

} LoRa_data_REPORT;

typedef struct
{
  uint8_t *rev_buffer; /*!< Runtime buffer */
  uint8_t *rev_buffer2; /*!< Runtime buffer */
  uint8_t *send_buffer2; /*!< Runtime buffer */
  uint16_t pattern_tout_cycle;
  uint32_t event_queue_size;

  TaskHandle_t tsk_hdl_LoRa_rev;  /*!< LoRa Parser taskhandle*/
  TaskHandle_t tsk_hdl_LoRa_send; /*!< LoRa Parser taskhandle*/
  uart_port_t uart_port;          /*!< Uart port number */
  QueueHandle_t event_queue;      /*!< UART event queue handle */
} LoRa_conf_t;

/*Public variables ----------------------------------------------------------*/
extern QueueHandle_t TCP_data_rev_queue;
QueueHandle_t LoRa_data_rev_queue;
/* Public function prototypes -----------------------------------------------*/
void LoRa_Configuration(void);

// static void LoRa_rev_task(void* arg);
