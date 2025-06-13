#ifndef _TASK_WIFI_H_
#define _TASK_WIFI_H_

#include "task_config.h"

#define PARAM_SIZE      64          // 控制命令参数最大长度

typedef enum {
    WIFI_CMD_CONNECT = 0,           // 连接 WiFi
    WIFI_CMD_DISCONNECT,            // 断开 WiFi
    WIFI_CMD_SCAN,                  // 扫描 WiFi
    WIFI_CMD_GET_STATUS,            // 获取 WiFi 状态
} WIFI_CMD;

typedef struct {
    TaskHandle_t htask;             // 任务句柄
    QueueHandle_t cmd_queue;        // 控制命令队列
    bool is_connected;              // 是否已连接 WiFi
} WifiHandle;

typedef struct {
    uint8_t cmd;                    // 控制命令
    uint8_t *param[PARAM_SIZE];     // 控制命令参数数组
} WifiMsg;

void createWifiTask(void);

#endif // !_TASK_WIFI_H_
