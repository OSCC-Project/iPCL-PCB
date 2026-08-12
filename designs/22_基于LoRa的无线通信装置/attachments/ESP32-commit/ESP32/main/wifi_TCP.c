/* Includes ------------------------------------------------------------------*/
#include "wifi_TCP.h"
/*const variables ------------------------------------------------------------*/
static const char *TAG = "WIFI_TCP";
/*Private variables ----------------------------------------------------------*/
// static bool WIFI_ready;
// static QueueHandle_t WIFI_ready_queue;
static SemaphoreHandle_t xWait_WIFI_send, xWait_WIFI_rev;
static uint8_t *debug_data;

/* Private function prototypes -----------------------------------------------*/
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
// static void tcp_server_task(void *pvParameters);
static void tcp_server_recv_task(void *pvParameters);
static int try_receive(const char *tag, const int sock, uint8_t *data, size_t max_len);
static void rev_retransmit(TCP_conf_t *TCP_conf, const int sock);
static void send_TCP_data_to_queen(int len, uint8_t *rx_buffer);
static void form_queen_send_TCP(const int sock);
// static void tcp_server_send_task(void *pvParameters);
// static void do_retransmit(TCP_conf_t *TCP_conf, const int sock);

/// @brief wifi init
/// @param  None
void wifi_init_softap(void)
{
  debug_data = (uint8_t *)malloc(1024);
  ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
  xWait_WIFI_send = xSemaphoreCreateBinary();
  xWait_WIFI_rev = xSemaphoreCreateBinary();
  // xSemaphoreGive(xWait_WIFI_send);
  // xSemaphoreGive(xWait_WIFI_rev);
  // WIFI_ready_queue = xQueueCreate(1, sizeof(bool));
  // WIFI_ready = 0;

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // esp_event_handler_instance_t instance_any_id;
  // esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL,
      // &instance_any_id));
      NULL));
  // ESP_ERROR_CHECK(esp_event_handler_instance_register(
  //     IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL,
  //     &instance_got_ip));

  wifi_config_t wifi_config = {
      .ap =
          {
              .ssid = WIFI_SSID,
              .ssid_len = strlen(WIFI_SSID),
              .password = WIFI_PASS,
              .channel = ESP_WIFI_CHANNEL,
              .max_connection = MAX_STA_CONN,
              .authmode = WIFI_AUTH_WPA3_PSK,
              .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
              // .pmf_cfg = {
              //     .required = true,
              // },
          },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
           WIFI_SSID, WIFI_PASS, ESP_WIFI_CHANNEL);

}

void TCP_task_init(void)
{
  ESP_LOGI(TAG, "TCP task init");
  TCP_conf_t *TCP_conf = calloc(1, sizeof(TCP_conf_t));
  if (!TCP_conf)
  {
    ESP_LOGE(TAG, "calloc memory for esp_fps failed");
    for (;;)
      ;
  }
  TCP_conf->rev_buffer2 = calloc(1, TCP_RX_BUF_SIZE * 2 + 2);
  if (!TCP_conf->rev_buffer2)
  {
    ESP_LOGE(TAG, "calloc memory for runtime rev_buffer failed");
    for (;;)
      ;
  }
  TCP_conf->send_buffer2 = calloc(1, TCP_TX_BUF_SIZE * 2 + 2);
  if (!TCP_conf->send_buffer2)
  {
    ESP_LOGE(TAG, "calloc memory for runtime rev_buffer failed");
    for (;;)
      ;
  }
  TCP_data_rev_queue = xQueueCreate(TCP_REV_QUEUE_LEN, sizeof(uint8_t *));

#ifdef CONFIG_IPV4
  TCP_conf->addr_family = AF_INET;
  xTaskCreate(tcp_server_recv_task, "tcp_server_recv", 4096, (void *)TCP_conf, 10, &TCP_conf->tsk_hdl_TCP_rev);
  // xTaskCreate(tcp_server_send_task, "tcp_server_send", 4096, (void *)TCP_conf, 10, &TCP_conf->tsk_hdl_TCP_send);
#endif
#ifdef CONFIG_EXAMPLE_IPV6
  TCP_conf->addr_family = AF_INET6;
  xTaskCreate(tcp_server_recv_task, "tcp_server_recv", 4096, (void *)TCP_conf, 10, &TCP_conf->tsk_hdl_TCP_rev);
  // xTaskCreate(tcp_server_send_task, "tcp_server_send", 4096, (void *)TCP_conf, 10, &TCP_conf->tsk_hdl_TCP_send);
#endif
}

/// @brief handler wifi event
/// @param arg
/// @param event_base
/// @param event_id
/// @param event_data
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{

  if (event_id == WIFI_EVENT_AP_STACONNECTED)
  {
    wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
             MAC2STR(event->mac), event->aid);
    // xSemaphoreGive(xWait_WIFI); // 用户连接WIFI，启动TCP
    xSemaphoreGive(xWait_WIFI_send);
    xSemaphoreGive(xWait_WIFI_rev);
    // bool WIFI_ready = 1;
    // if (xQueueSend(WIFI_ready_queue, &WIFI_ready, portMAX_DELAY) != pdPASS)
    // {
    //   ESP_LOGE(TAG, "Failed to send data to WIFI_ready_queue");
    // }
    // ESP_LOGI(TAG, "send data to WIFI_ready_queue");
  }
  else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
  {
    wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
             MAC2STR(event->mac), event->aid);
    // WIFI_ready = 0;
    // xSemaphoreTake(xWait_WIFI, (TickType_t)1 / portTICK_PERIOD_MS);
  }
}

static void tcp_server_recv_task(void *pvParameters)
{
  static const char *TAG = "TCP-recv_task";
  char addr_str[128];
  TCP_conf_t *TCP_conf = (TCP_conf_t *)pvParameters;
  int addr_family = (int)TCP_conf->addr_family;
  int ip_protocol = 0;
  int keepAlive = 1;
  int keepIdle = KEEPALIVE_IDLE;
  int keepInterval = KEEPALIVE_INTERVAL;
  int keepCount = KEEPALIVE_COUNT;
  // sockaddr_storage同时适用于 struct sockaddr_in和struct sockaddr_in6
  // 兼容IPv6设计
  struct sockaddr_storage dest_addr;

  ESP_LOGI(TAG, "begin TCP rev init");

Begin_REV_TCP:
  xSemaphoreTake(xWait_WIFI_rev, portMAX_DELAY);
  // bool WIFI_is_ready;
  // if (xQueueReceive(WIFI_ready_queue, &WIFI_is_ready, 0) != pdPASS)
  // {
  //   // ESP_LOGI(TAG, "WIFI ready");
  // }
  ESP_LOGI(TAG, "WIFI ready");
  // while (!WIFI_ready)
  // {
  // }
#ifdef CONFIG_IPV4
  if (addr_family == AF_INET)
  {
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY监听所有IP
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(TCP_PORT);
    ip_protocol = IPPROTO_IP;
  }
#endif
#ifdef CONFIG_EXAMPLE_IPV6
  if (addr_family == AF_INET6)
  {
    struct sockaddr_in6 *dest_addr_ip6 = (struct sockaddr_in6 *)&dest_addr;
    bzero(&dest_addr_ip6->sin6_addr.un, sizeof(dest_addr_ip6->sin6_addr.un));
    dest_addr_ip6->sin6_family = AF_INET6;
    dest_addr_ip6->sin6_port = htons(TCP_PORT);
    ip_protocol = IPPROTO_IPV6;
  }
#endif
  int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
  if (listen_sock < 0)
  {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    vTaskDelete(NULL);
    return;
  }
  int opt = 1;
  // SO_REUSEADDR 可重用IP地址和端口
  setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#if defined(CONFIG_IPV4) && defined(CONFIG_IPV6)
  // Note that by default IPV6 binds to both protocols, it is must be disabled
  // if both protocols used at the same time (used in CI)
  setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
#endif
  ESP_LOGI(TAG, "Socket created");
  int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err != 0)
  {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
    goto REV_CLEAN_UP;
  }
  ESP_LOGI(TAG, "Socket bound, port %d", TCP_PORT);
  err = listen(listen_sock, 1);
  if (err != 0)
  {
    ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
    goto REV_CLEAN_UP;
  }
  // 循环等待客户端连接
  while (1)
  {
    ESP_LOGI("TCP", "Waiting for client connection...");
    // 等待客户端连接
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t addr_len = sizeof(source_addr);
    int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
    if (sock < 0)
    {
      ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
      // break;
      continue; // 继续等待新的客户端连接
    }
    // Set tcp keepalive option（设置接收超时）
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
    // Convert ip address to string
#ifdef CONFIG_IPV4
    if (source_addr.ss_family == PF_INET)
    {
      inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
    }
#endif
#ifdef CONFIG_EXAMPLE_IPV6
    if (source_addr.ss_family == PF_INET6)
    {
      inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
    }
#endif
    ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

    // set the socket non-blocking
    int flags = fcntl(sock, F_GETFL);
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
      ESP_LOGE(TAG, "[sock=%d]: %s\n"
                    "error=%d: %s",
               sock, "Unable to set socket non blocking", errno, strerror(errno));
      goto REV_CLEAN_UP;
    }

    rev_retransmit(TCP_conf, sock);

    // 清理并关闭连接
    ESP_LOGI("TCP", "Cleaning up and closing connection");
    close(sock);
  }

REV_CLEAN_UP:
  ESP_LOGI(TAG, "CLEAN_UP");
  close(listen_sock);
  goto Begin_REV_TCP;
  vTaskDelete(NULL);
}

static int try_receive(const char *tag, const int sock, uint8_t *data, size_t max_len)
{
  int len = recv(sock, data, max_len, 0);
  if (len < 0)
  {
    if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK)
    {
      return 0; // Not an error
    }
    if (errno == ENOTCONN)
    {
      ESP_LOGW(tag, "[sock=%d]: Connection closed", sock);
      return -2; // Socket has been disconnected
    }
    ESP_LOGE(TAG, "[sock=%d]: %s\n"
                  "error=%d: %s",
             sock, "Error occurred during receiving", errno, strerror(errno));
    return -1;
  }

  return len;
}

static void rev_retransmit(TCP_conf_t *TCP_conf, const int sock)
{
  static const char *TAG = "TCP-rev_retransmit";
  uint8_t *rx_buffer = TCP_conf->rev_buffer2;
  

  // 接收数据并将数据放入队列
  while (1)
  {
    // int len = recv(sock, rx_buffer, TCP_RX_BUF_SIZE - 1, 0);
    int len = try_receive(TAG, sock, rx_buffer, TCP_RX_BUF_SIZE - 1);
    // if (len == 0)
    // {
    //   ESP_LOGI(TAG, "Client disconnected");
    //   break; // 客户端断开连接
    // }
    if (len < 0)
    {
      ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
      break; // 出现接收错误或超时
    }
    else if (len > 0)
    {
      send_TCP_data_to_queen(len, rx_buffer);
    }
    // len =0 无数据，尝试发送
    form_queen_send_TCP(sock);
  }
}

static void send_TCP_data_to_queen(int len, uint8_t *rx_buffer)
{ 
  uint8_t *data_to_send;
  uint16_t data_len;
  // 如果队列已满，清空最旧的数据
  if (uxQueueSpacesAvailable(TCP_data_rev_queue) == 0)
  {
    uint8_t *dummy_data;
    // 清空队列中的最旧数据
    if (xQueueReceive(TCP_data_rev_queue, &dummy_data, 0) == pdPASS)
    {
      free(dummy_data);
      dummy_data = NULL;
      ESP_LOGW(TAG, "TCP_data_rev_queue is full . Oldest data removed from the queue");
    }
  }

  // if (len >LoRa_DATA_MAX_SIZE){

  // }

  // 将接收到的数据放入队列

  // debug
  // rx_buffer[len] = 0;
  // ESP_LOGD(TAG, "Received %d bytes: %s", len, rx_buffer);

  // 申请数据大小 = 数据大小 + #(1 Byte) + 2 Byte len +
  // <校验和> (1 Byte) + 0A(换行1B)+ "\0"(数据结束 1B)+预留1B
  // = len + 7
  data_to_send = malloc(len + 7);
  *(data_to_send) = DATA_START_DELIMITER;
  data_len = (uint16_t)len + 3; // #<2B length><data>:1B+2B=3B
  *((uint16_t *)(data_to_send + 1)) = data_len;
  memcpy(data_to_send + 3, rx_buffer, len);

  // debug
  //  char_to_hex((char *)data, data_len, (char *)debug_data);
  //  dump_hex(debug_data, data, len + 4);
  //  ESP_LOGI(TAG, "【data:】%s", debug_data);
  //  ESP_LOGI(TAG, "【data:】%s【datalen:】%d", data, data_len);

  if (xQueueSend(TCP_data_rev_queue, &data_to_send, portMAX_DELAY) != pdPASS)
  {
    ESP_LOGE(TAG, "Failed to send data to queue");
    free(data_to_send);
    data_to_send = NULL;
  }
}

static void form_queen_send_TCP(const int sock)
{
  uint8_t *send_buf;
  uint16_t data_len;
  if (xQueueReceive(LoRa_data_rev_queue, &send_buf, (TickType_t)20) == pdPASS)
  {
    ESP_LOGI(TAG, "TCP Receive LoRa data");
    // 去除消息帧头 1B + 2B 长度

    data_len = *((uint16_t *)(send_buf + 1)) - 3;
    if (send(sock, send_buf + 3, data_len, 0) < 0)
    {
      ESP_LOGE("TCP", "Send failed");
    }
    // DEBUG
    //  send_buf[data_len] = '\0';
    //  ESP_LOGD(TAG, "TCP Send LoRa data:%s", send_buf + 3);

    free(send_buf);
    send_buf = NULL;
  }
}

