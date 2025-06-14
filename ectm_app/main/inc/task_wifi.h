#ifndef _TASK_WIFI_H_
#define _TASK_WIFI_H_

#include "task_config.h"

typedef enum {
    WIFI_CMD_
    // 重新连接
    // 断开连接
    // 扫描设备
    // 获取信息
    // 开启TCP服务端
    // 关闭TCP服务端
    // 开启TCP客户端
    // 关闭TCP客户端
    // 开启UDP服务端
    // 关闭UDP服务端
    // 开启UDP客户端
    // 关闭UDP客户端
} WIFI_CMD;


typedef enum {
    WIFI_CMD_CONNECT = 0,               // 连接
    FILE_CMD_WRITE                      // 断开
} WIFI_STATE;

typedef struct {
    int test;
} WifiMsg;

typedef struct {
    TaskHandle_t htask;                 // 任务句柄
    QueueHandle_t cmd_queue;            // 任务请求队列
    uint8_t state;                      // wifi 状态
} WifiHandle;

void createWifiTask(void);

#endif // !_TASK_WIFI_H_
