/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #include "LoRa_parser.h"
#include "conf.h"
#include "tool.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_tls.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "inttypes.h"

#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
/*define ---------------------------------------------------------------------*/

#define TCP_REV_QUEUE_LEN 10
#define MAX_STA_CONN 2
#define ESP_WIFI_CHANNEL 8
#define TCP_PORT 8266
// TCP keep-alive idle time(s)
#define KEEPALIVE_IDLE 5
// TCP keep-alive interval time(s)
#define KEEPALIVE_INTERVAL 5
// TCP keep-alive packet retry send counts
#define KEEPALIVE_COUNT 3

#define WIFI_MAXIMUM_RETRY 5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK

/*Private struct ----------------------------------------------------------*/
typedef struct
{
  // uint8_t *rev_buffer;   /*!< Runtime buffer */
  uint8_t *rev_buffer2;  /*!< Runtime buffer */
  uint8_t *send_buffer2; /*!< Runtime buffer */

  int addr_family;
  TaskHandle_t tsk_hdl_TCP_rev; /*!< LoRa Parser taskhandle*/
  TaskHandle_t tsk_hdl_TCP_send; /*!< LoRa Parser taskhandle*/
  // bool WIFI_ready : 1, rev : 7;
} TCP_conf_t;
/*Public variables ----------------------------------------------------------*/
extern QueueHandle_t LoRa_data_rev_queue;
QueueHandle_t TCP_data_rev_queue;
// bool TCP_ready;


/* Public function prototypes -----------------------------------------------*/
void wifi_init_softap(void);
void TCP_task_init(void);
