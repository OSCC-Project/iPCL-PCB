/* Includes ------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "LoRa_parser.h"
// #include "wifi_TCP.h"
// #include "driver/gpio.h"
// #include "driver/uart.h"
// #include "esp_chip_info.h"
#include "esp_log.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
/*define ---------------------------------------------------------------------*/
extern void LoRa_Configuration(void);
extern void wifi_init_softap(void);
extern void TCP_task_init(void);


/*const variables ------------------------------------------------------------*/
static const char* TAG = "Base_station";
/*Private variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void app_main(void) {
  esp_log_level_set("*", ESP_LOG_INFO);
  // esp_log_level_set(TAG, ESP_LOG_DEBUG);
  // esp_log_level_set("LoRa", ESP_LOG_DEBUG);
  // esp_log_level_set("WIFI_mqtt", ESP_LOG_DEBUG);
  // esp_log_level_set("MQTT_send_task", ESP_LOG_DEBUG);
  //esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);

  ESP_LOGI(TAG, "[APP] Startup..");
  ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes",
           esp_get_free_heap_size());
  ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_init_softap();
  TCP_task_init();
  LoRa_Configuration();
  // xTaskCreate(LoRa_rev_task, "LoRa_rev_task", 2048, NULL, 12, NULL);
  //  idf.py menuconfig
  //  idf.py build
  //  idf.py -p COM3 flash
  //  idf.py -p COM3 monitor
  //idf.py monitor
}
