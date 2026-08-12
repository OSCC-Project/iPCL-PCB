// 全局配置文件

/* 单元测试（测试完注释） */
// #define APP_TEST //LoRa定时发送测试
// #define ECHO_TEST //回传测试程序

/* WIFI 配置 */
// 默认初始SSID

#define WIFI_SSID "esp32_iot"

// 默认初始WIFI密码
#define WIFI_PASS "12345678"
// 启用IPv4
#define CONFIG_IPV4

/* 数据协议配置 */
//  定义定界符，选择两个字节作为LoRa接收的定界符
#define DATA_START_DELIMITER 0x02 // 数据开始标志
#define CMD_START_DELIMITER 0x01  // 指令开始标志
#define END_DELIMITER 0x7F
// 转义所需字符
#define ESCAPE_BYTE 0x7E
#define ESCAPED_7E 0x7D
#define ESCAPED_7F 0x7E

/* 数据长度、buff长度配置 */
// LoRa 接收、发送缓存配置
#define LoRa_RX_BUF_SIZE (2048)
#define LoRa_TX_BUF_SIZE (2048)
#define LoRa_EVENT_QUEUE_SIZE (16)
// 接收数据最大长度
#define LoRa_DATA_MAX_SIZE (2048)

#define TCP_RX_BUF_SIZE (2048)
#define TCP_TX_BUF_SIZE (2048)