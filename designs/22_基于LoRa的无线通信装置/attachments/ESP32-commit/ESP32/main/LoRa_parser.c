#include "LoRa_parser.h"

/*Public variables ----------------------------------------------------------*/
// QueueHandle_t LoRa_data_rev_queue;
/*const variables ------------------------------------------------------------*/
static const char *TAG = "LoRa";
/*Private variables ----------------------------------------------------------*/
static QueueHandle_t LoRa_uart_queue;

/* Private function prototypes -----------------------------------------------*/
static void LoRa_event_task(void *pvParameters);
static void LoRa_data_send_task(void *pvParameters);
static int escape_data(uint8_t *input, uint8_t *output, int input_len);
static int unescape_data(uint8_t *input, uint8_t *output, int input_len);
static uint8_t calculat_crc(const uint8_t *sentence, uint16_t len);
static void LoRa_handle_uart_pattern(LoRa_conf_t *LoRa_conf);
static esp_err_t LoRa_decode(LoRa_conf_t *LoRa_conf, size_t len);
static bool check_crc(const char *sentence);

static bool LoRa_send_ready;
/*Private struct ----------------------------------------------------------*/

/**
 * @brief GPS parser library runtime structure
 *
 */

/// @brief Config LoRa
/// @param None
void LoRa_Configuration(void)
{

  ESP_LOGI(TAG, "LoRa Configuration start");
  LoRa_send_ready = 0;
  LoRa_conf_t *LoRa_conf = calloc(1, sizeof(LoRa_conf_t));
  if (!LoRa_conf)
  {
    ESP_LOGE(TAG, "calloc memory for esp_fps failed");
    for (;;)
      ;
  }
  LoRa_conf->rev_buffer = calloc(1, LoRa_RX_BUF_SIZE);
  if (!LoRa_conf->rev_buffer)
  {
    ESP_LOGE(TAG, "calloc memory for runtime rev_buffer failed");
    for (;;)
      ;
  }
  LoRa_conf->rev_buffer2 = calloc(1, LoRa_RX_BUF_SIZE * 2 + 2);
  if (!LoRa_conf->rev_buffer2)
  {
    ESP_LOGE(TAG, "calloc memory for runtime rev_buffer2 failed");
    for (;;)
      ;
  }
  LoRa_conf->send_buffer2 = calloc(1, LoRa_TX_BUF_SIZE * 2 + 2);
  if (!LoRa_conf->send_buffer2)
  {
    ESP_LOGE(TAG, "calloc memory for runtime send_buffer2 failed");
    for (;;)
      ;
  }
  LoRa_conf->uart_port = LoRa_UART_NUM;
  LoRa_conf->event_queue_size = LoRa_EVENT_QUEUE_SIZE;
  LoRa_conf->event_queue = LoRa_uart_queue;
  LoRa_conf->pattern_tout_cycle = 15;

  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  ESP_ERROR_CHECK(uart_driver_install(LoRa_UART_NUM, LoRa_RX_BUF_SIZE * 2, 0,
                                      LoRa_conf->event_queue_size,
                                      &LoRa_conf->event_queue, 0));
  // ESP_ERROR_CHECK(uart_driver_install(LoRa_UART_NUM, LoRa_RX_BUF_SIZE * 2, 0,
  // 0,
  //                                     NULL, intr_alloc_flags));
  ESP_ERROR_CHECK(uart_param_config(LoRa_UART_NUM, &uart_config));
  ESP_ERROR_CHECK(
      uart_set_pin(LoRa_UART_NUM, LoRa_TXD_Pin, LoRa_RXD_Pin, -1, -1));

  /* Set pattern interrupt, used to detect the end of a line */
  uart_enable_pattern_det_baud_intr(LoRa_UART_NUM, END_DELIMITER, 1,
                                    LoRa_conf->pattern_tout_cycle, 0, 0);
  /* Set pattern queue size */
  uart_pattern_queue_reset(LoRa_UART_NUM, LoRa_conf->event_queue_size);
  uart_flush(LoRa_UART_NUM);
  // GPIO setup

  // set M0 M1
  //  zero-initialize the config structure.
  gpio_config_t io_conf = {};
  // disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  // set as output mode
  io_conf.mode = GPIO_MODE_OUTPUT;
  // bit mask of the pins that you want to set
  io_conf.pin_bit_mask = (1ULL << LoRa_M0_Pin) | (1ULL << LoRa_M1_Pin);
  // disable pull-down mode
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  // disable pull-up mode
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  // configure GPIO with the given settings
  gpio_config(&io_conf);

  // AUX
  io_conf.pin_bit_mask = LoRa_AUX_Pin;
  io_conf.mode = GPIO_MODE_INPUT;
  //...

  /************LoRa communication Configuration*************/
  ESP_LOGI(TAG, "LoRa communication Conf start");
  gpio_set_level(LoRa_M0_Pin, 1);
  gpio_set_level(LoRa_M1_Pin, 1);
  // portTICK_PERIOD_MS节拍 默认值为1毫秒。
  vTaskDelay(50 / portTICK_PERIOD_MS); // 延时大约50ms
  char config[] = {
      0xC0, // C0掉电保存,C2掉电不保存
      0xFF, // ADDH,模块高地址
      0xFF, // ADDL,低地址
      0X1C, // 7-6串口校验位,5-3TTL串口速率（bps）,2-0空中速率（bps）
      0x15, // 信道410MHz+CHAN *1MHz,410～441MHz
      0x44, // 7定点发送使能位,6IO驱动方式,5-3无线唤醒时间,2FEC
            // 开关,1-0发射功率,(44为透传，C4设置为定点)
  }; // start
  uart_write_bytes(LoRa_UART_NUM, (const char *)config, 6);
  ESP_LOGI(TAG, "LoRa config write_bytes");
  vTaskDelay(70 / portTICK_PERIOD_MS); // 延时大约70ms
  char reda_cmd[] = {0xC1, 0xC1, 0xC1};
  // 校验是否成功写入
  char read_config[6]; /*eg:C0 00 01 1A 17 C4 */
  uint8_t i, validate = 0;
  char *data = (char *)malloc(LoRa_RX_BUF_SIZE);
  while (validate == 0)
  {
    uart_write_bytes(LoRa_UART_NUM, (const char *)reda_cmd, 3);
    ESP_LOGI(TAG, "LoRa read_config write_bytes");
    vTaskDelay(70 / portTICK_PERIOD_MS); // 延时大约70ms
    int len = uart_read_bytes(LoRa_UART_NUM, data, (LoRa_RX_BUF_SIZE - 1),
                              20 / portTICK_PERIOD_MS);
    // char _temp[LoRa_RX_BUF_SIZE * 2 + 10];
    // memset(_temp, 0, sizeof(_temp));
    if (len)
    {
      /*read data*/
      ESP_LOGI(TAG, "uart read_config read_bytes");
      // char_to_hex(data, LoRa_RX_BUF_SIZE, _temp);
      // // ESP_LOGD(TAG, "uart read:%s", (char*)data);
      // ESP_LOGD(TAG, "uart read:%s(HEX)", _temp);
      for (i = 0; data[i] != 0xC0 && i + 6 != LoRa_RX_BUF_SIZE; ++i)
        ;
      // cp config into read_config
      char *_p = read_config;
      for (; i < 6; i++)
      {
        *_p++ = data[i];
      }
      ESP_LOGI(TAG, "uart config copy");

      // char_to_hex(read_config, 6, _temp);
      // //_temp[7] = '\0';
      // ESP_LOGD(TAG, "config read:%s(HEX)", (char*)_temp);

      // 校验设置是否写入,跳过指令（写指令为C2时，读取的数据仍以C0开头）
      if (strncmp(config + 1, read_config + 1, 5) == 0)
      {
        validate = 1;
        ESP_LOGI(TAG, "uart config validate = 1");
      }
      else
      {
        // 写入错误，重试
        ESP_LOGE(TAG, "uart config ERROR");
        uart_write_bytes(LoRa_UART_NUM, (const char *)config, 6);
        ESP_LOGI(TAG, "LoRa config write_bytes");
        vTaskDelay(70 / portTICK_PERIOD_MS); // 延时大约70ms
      }
    }
    else
    {
      // 没读到数据，跳过重读
      ESP_LOGE(TAG, "uart read len=0");
    }
  }
  free(data);
  data = NULL;

  gpio_set_level(LoRa_M0_Pin, 0);
  gpio_set_level(LoRa_M1_Pin, 0);
  vTaskDelay(5 / portTICK_PERIOD_MS); // 延时大约5ms

  // 创建解析数据队列
  LoRa_data_rev_queue = xQueueCreate(LoRa_REV_QUEUE_LEN, sizeof(uint8_t *));
  LoRa_send_ready = 1;
  ESP_LOGI(TAG, "LoRa_Configuration Done");

  // 创建任务：(任务函数，任务名，任务堆栈大小，任务参数，任务优先级，任务句柄)
  xTaskCreate(LoRa_event_task, "uart_event_task", 4096, LoRa_conf, 10,
              &LoRa_conf->tsk_hdl_LoRa_rev);
  xTaskCreate(LoRa_data_send_task, "LoRa_data_send_task", 4096, LoRa_conf, 10,
              &LoRa_conf->tsk_hdl_LoRa_send);
}

static void LoRa_data_send_task(void *pvParameters)
{
  LoRa_conf_t *LoRa_conf = (LoRa_conf_t *)pvParameters;
  static const char *TAG = "LoRa_data_send_task";
  uint8_t *d;
  uint16_t data_len;
  if (!TCP_data_rev_queue)
  {
    ESP_LOGE(TAG, "TCP_data_rev_queue NULL");
    for (;;)
      ;
  }

  for (;;)
  {
#ifdef APP_TEST
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    const uint8_t test_data[] = {
        DATA_START_DELIMITER,
        0,
        0,
        '1',
        '2',
        '3',
        '4',
        '5'};
    uint16_t len_ = sizeof(test_data);
    d = malloc(len_ + 7);
    memcpy(d, &test_data, len_);
    *((uint16_t *)(d + 1)) = len_;
    char_to_hex((char *)d, len_, (char *)LoRa_conf->send_buffer2);
    ESP_LOGI(TAG, "\n\nAPP test: size_t: %d ,data: %s", len_, (char *)LoRa_conf->send_buffer2);
#endif
#ifndef APP_TEST
    if (xQueueReceive(TCP_data_rev_queue, (void *)&d, (TickType_t)portMAX_DELAY))
#endif
    {
      ESP_LOGI(TAG, "Receive TCP data");
      // ESP_LOGI(TAG, "【calloc data &:】%p", d);
      // ESP_LOGI(TAG, "【calloc data &:】%p", &d);
      data_len = *((uint16_t *)(d + 1));
      while (!LoRa_send_ready)
        ; // 等待LoRa发送初始化

      // 加入校验和
      uint8_t crc = calculat_crc(d, data_len);
      *((d + data_len + 0)) = crc;
      //(加入CRC后数据：<DATA_START_DELIMITER(1B)><data_len(2B)><CRC(1B)>)
      // 数据总长为 data_len+1
      // 转义处理
      // uint8_t *escaped_data = (uint8_t *)malloc((data_len + 1) * 2 + 1); // 最坏情况是数据大小翻倍
      uint8_t *escaped_data = (uint8_t *)LoRa_conf->send_buffer2; // 防止反复申请内存导致内存碎片
      int escaped_len = escape_data(d, escaped_data, data_len + 1);
      ESP_LOGI(TAG, "before len: %d,escape len: %d", data_len + 1, escaped_len);
      *((escaped_data + escaped_len)) = END_DELIMITER; // 帧结束界定符
      // *((escaped_data + data_len + 2)) = '\0';          // 无需写入LoRa

      // char_to_hex((char *)escaped_data, escaped_len + 1, (char *)LoRa_conf->rev_buffer2);
      // ESP_LOGI(TAG, "send %d Byte to LoRa: %s", escaped_len + 1, (char *)LoRa_conf->rev_buffer2);
      //...
      // ESP_LOGI(TAG, "send %d Byte to LoRa:%s%#hhX%s", data_len, d + 3, crc, d + 3 + data_len + 2);

      // 写入escaped_len + 1 ，转义后字符串+帧结束界定符
      uart_write_bytes(LoRa_UART_NUM, (const char *)escaped_data, escaped_len + 1);
      // free(escaped_data);
      // escaped_data = NULL;
      free(d);
      d = NULL;
    }
  }

  vTaskDelete(NULL);
}

// 转义函数，将输入数据中的 0x7E 和 0x7F 转换为转义序列
static int escape_data(uint8_t *input, uint8_t *output, int input_len)
{
  // 转义规则 使用 ESCAPE_BYTE(假设为0x7E) 作为转义标识符，且对于数据中出现 END_DELIMITER 字节，
  // 将其转义为一个新的转义序列：0x7E 0x7E，避免出现任何的 END_DELIMITER
  // 数据中如果包含 0x7E 字节，也需要被转义为另一个新的序列：0x7E 0x7D ，从而避免与转义标识符发生冲突
  int output_len = 0;
  for (int i = 0; i < input_len; i++)
  {
    if (input[i] == ESCAPE_BYTE)
    {
      // 转义 0x7E 为 0x7E 0x7D
      output[output_len++] = ESCAPE_BYTE;
      output[output_len++] = ESCAPED_7E;
    }
    else if (input[i] == END_DELIMITER)
    {
      // 转义 0x7F 为 0x7E 0x7E
      output[output_len++] = ESCAPE_BYTE;
      output[output_len++] = ESCAPED_7F;
    }
    else
    {
      // 其他字节直接复制
      output[output_len++] = input[i];
    }
  }
  return output_len;
}

// 反转义函数，将输入数据中的转义序列恢复为原始数据
static int unescape_data(uint8_t *input, uint8_t *output, int input_len)
{
  int output_len = 0;
  for (int i = 0; i < input_len; i++)
  {
    if (input[i] == ESCAPE_BYTE && i + 1 < input_len)
    {
      if (input[i + 1] == ESCAPED_7E)
      {
        // 恢复为 0x7E
        output[output_len++] = ESCAPE_BYTE;
        i++; // 跳过转义字节
      }
      else if (input[i + 1] == ESCAPED_7F)
      {
        // 恢复为 0x7F
        output[output_len++] = END_DELIMITER;
        i++; // 跳过转义字节
      }
      else
      {
        // 未识别的转义序列，保留原字节
        output[output_len++] = input[i];
      }
    }
    else
    {
      // 非转义字节直接复制
      output[output_len++] = input[i];
    }
  }
  return output_len;
}

static void LoRa_event_task(void *pvParameters)
{
  LoRa_conf_t *LoRa_conf = (LoRa_conf_t *)pvParameters;
  uart_event_t event;
  // size_t buffered_size;
  //  uint8_t* dtmp = (uint8_t*)malloc(LoRa_RX_BUF_SIZE);
  //  char* _temp = (char*)malloc(LoRa_RX_BUF_SIZE * 2 + 2);
  // char rx_buffer[128];
  // size_t received_length = 0;
  for (;;)
  {
    //(TickType_t)pdMS_TO_TICKS(200)
    if (xQueueReceive(LoRa_conf->event_queue, (void *)&event,
                      (TickType_t)portMAX_DELAY))
    {
      // bzero(dtmp, LoRa_RX_BUF_SIZE);
      // ESP_LOGI(TAG, "uart[%d] event:", LoRa_UART_NUM);
      switch (event.type)
      {
      // Event of UART receving data
      case UART_DATA:
        ESP_LOGI(TAG, "rev loRa DATA");
        // char dtmp[128];
        // char dtmp2[128 * 4];
        // uart_read_bytes(LoRa_UART_NUM, dtmp, event.size, portMAX_DELAY);
        // char_to_hex(dtmp, event.size, dtmp2);
        // ESP_LOGD(TAG, "[loRa DATA]: %s", (char *)dtmp2);
        break;
      // Event of HW FIFO overflow detected
      case UART_FIFO_OVF:
        ESP_LOGW(TAG, "hw fifo overflow");
        uart_flush_input(LoRa_UART_NUM);
        xQueueReset(LoRa_uart_queue);
        break;
      // Event of UART ring buffer full
      case UART_BUFFER_FULL:
        ESP_LOGW(TAG, "ring buffer full");
        uart_flush_input(LoRa_UART_NUM);
        xQueueReset(LoRa_uart_queue);
        break;
      // Event of UART RX break detected
      case UART_BREAK:
        ESP_LOGI(TAG, "uart rx break");
        break;
      // Event of UART parity check error
      case UART_PARITY_ERR:
        ESP_LOGI(TAG, "uart parity error");
        break;
      // Event of UART frame error
      case UART_FRAME_ERR:
        ESP_LOGI(TAG, "uart frame error");
        break;
      // UART_PATTERN_DET
      case UART_PATTERN_DET:
        ESP_LOGI(TAG, "UART_PATTERN_DET");
        LoRa_send_ready = 0;
        LoRa_handle_uart_pattern(LoRa_conf);
        LoRa_send_ready = 1;
        break;
      // Others
      default:
        ESP_LOGI(TAG, "uart event type: %d", event.type);
        break;
      }
    }
  }
  //   free(dtmp);
  //   dtmp = NULL;
  vTaskDelete(NULL);
}

/**
 * @brief Handle when a pattern has been detected by uart
 *
 * @param LoRa_conf LoRa_conf_t type object
 */
static void LoRa_handle_uart_pattern(LoRa_conf_t *LoRa_conf)
{
  ESP_LOGI(TAG, "LoRa_handle_uart_pattern start");
  int pos = uart_pattern_pop_pos(LoRa_conf->uart_port);
  if (pos != -1)
  {
    /* read one line(include '\n') */
    int read_len = uart_read_bytes(LoRa_conf->uart_port, LoRa_conf->rev_buffer,
                                   pos + 1, 100 / portTICK_PERIOD_MS);
    /* make sure the line is a standard string */
    ESP_LOGD(TAG, "uart_read_bytes");

    // LoRa_conf->buffer[read_len] = '\0';
    // ESP_LOGD(TAG, "buffer 0 set");

    /* Send new line to handle */
    if (LoRa_decode(LoRa_conf, read_len + 1) != ESP_OK)
    {
      ESP_LOGW(TAG, "LoRa decode line failed");
    }
  }
  else
  {
    ESP_LOGW(TAG, "Pattern Queue Size too small");
    uart_flush_input(LoRa_conf->uart_port);
  }
}

/**
 * @brief Parse LoRa statements from receiver
 *
 * @param LoRa_conf LoRa_conf_t type object
 * @param len number of bytes to decode
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
static esp_err_t LoRa_decode(LoRa_conf_t *LoRa_conf, size_t len)
{
  ESP_LOGI(TAG, "LoRa_decode start");

  // 接收数据，进行反转义
  // uint8_t *unescaped_data = (uint8_t *)malloc(len); // 恢复数据的最大大小
  uint8_t *unescaped_data = (uint8_t *)LoRa_conf->rev_buffer2; // 使用固定buff防止产生内存碎片
  // int unescaped_len = unescape_data(LoRa_conf->rev_buffer, unescaped_data, len);
  unescape_data(LoRa_conf->rev_buffer, unescaped_data, len);
  // 处理反转义后的数据
  const char *d = (char *)unescaped_data;
  // ESP_LOGI(TAG, "Received unescaped data: %s", unescaped_data);

  // eg:$REPORT,010317,010117,144242,35.5617991N,120.10.07339E,25.35,72.11,42.6,*36
  if (check_crc(d) == 0)
  {
    // 校验和不一致
    ESP_LOGE(TAG, "The checksum is inconsistent");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "checksum checked!");
  // if (strncmp(d, DATA_START_DELIMITER, 1) == 0)
  if (*d == DATA_START_DELIMITER)
  {
    ESP_LOGI(TAG, "rev LoRa DATA_START_DELIMITER");
    uint16_t data_len = *((uint16_t *)(d + 1));
    uint8_t *data = calloc(data_len + 1, sizeof(uint8_t));
    if (!data)
    {
      ESP_LOGE(TAG, "LoRa_data calloc memory failed");
      for (;;)
        ;
    }
    // strlcpy((char *)data, (char *)d, data_len);]
    memcpy(data, d, data_len);

    // 如果队列已满，清空最旧的数据
    if (uxQueueSpacesAvailable(LoRa_data_rev_queue) == 0)
    {
      uint8_t *dummy_data;
      // 清空队列中的最旧数据
      if (xQueueReceive(LoRa_data_rev_queue, &dummy_data, 0) == pdPASS)
      {
        free(dummy_data);
        dummy_data = NULL;
        ESP_LOGW(TAG, "LoRa_data_rev_queue is full . Oldest data removed from the queue");
      }
    }
    
    #ifdef ECHO_TEST
    //回传测试
    if (xQueueSend(TCP_data_rev_queue, &data, portMAX_DELAY) != pdPASS)
    {
      ESP_LOGE(TAG, "Failed to send parsed data to queue");
    }
    
    #else
    if (xQueueSend(LoRa_data_rev_queue, &data, portMAX_DELAY) != pdPASS)
    {
      ESP_LOGE(TAG, "Failed to send parsed data to queue");
    }
    ESP_LOGI(TAG, "LoRa rev data send to queue");
    #endif
    // ESP_LOGD(TAG, "[rev data]: %s\n", data);

    // free(data);
    // data = NULL;
    return ESP_OK;
  }
  // else if (strncmp(d, CMD_START_DELIMITER, 1) == 0)
  else if (*d == CMD_START_DELIMITER)
  {
    // LoRa 模块内部指令(ACK、等)
    ESP_LOGI(TAG, "$REPORT cmd");
    return ESP_OK;
  }
  else
  {
    ESP_LOGE(TAG, "未知指令开头：%hhx", *d);
    return ESP_FAIL;
  }

  // free(unescaped_data);
  // unescaped_data = NULL;
  return ESP_FAIL;
}

/// @brief 计算所给数据CRC并检验与已有CRC是否相符
/// @param data
/// @param len
/// @param crc
/// @return crc
static bool check_crc(const char *sentence)
{
  uint8_t calculated_checksum = 0;
  uint8_t received_checksum = 0;
  uint16_t data_len = *((uint16_t *)(sentence + 1));

  // debug
  const char *d = sentence;
  uint8_t *debug_data = calloc((data_len + 7) * 2 + 1, sizeof(uint8_t));
  char_to_hex((char *)d, data_len + 3, (char *)debug_data);
  ESP_LOGD(TAG, "【dump data:】%s", debug_data);
  ESP_LOGD(TAG, "[loRa DATA]: %c<%02d>%s[%hhX]<%hhX>%hhX", *(d), data_len,
           (d + 3), *(d + data_len), *(d + data_len + 1), *(d + data_len + 2));
  free(debug_data);
  debug_data = NULL;
  received_checksum = (uint8_t)sentence[data_len];
  // 从第一个有效字符开始计算校验和，直到data_len
  for (int i = 0; i < data_len; ++i)
  {
    calculated_checksum ^= sentence[i];
  }
  ESP_LOGD(TAG,
           "[calculated_checksum] : %#hhX\n[received_checksum] : %#hhX",
           calculated_checksum, received_checksum);
  // 返回校验结果
  return (received_checksum == calculated_checksum) ? 1 : 0;
}

/// @brief 计算所给数据CRC,并返回
/// @param data
/// @param len
/// @param crc
/// @return crc
static uint8_t calculat_crc(const uint8_t *sentence, uint16_t len)
{
  uint8_t calculated_checksum = 0;
  // uint8_t i = 0;

  // 从第一个有效字符开始计算校验和，直到到达计算长度
  for (int i = 0; i < len; ++i)
  {
    calculated_checksum ^= sentence[i];
  }
  ESP_LOGD(TAG,
           "[calculated_checksum] : %#hhX",
           calculated_checksum);
  // 返回校验结果
  return calculated_checksum;
}
