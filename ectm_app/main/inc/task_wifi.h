#ifndef _TASK_WIFI_H_
#define _TASK_WIFI_H_

#include "task_config.h"
#include "task_file.h"

#define WIFI_SSID           "esp-wroom-32"
#define WIFI_PASSWORD       "88888888"
#define WIFI_MAX_CONNECT    8

typedef enum {
    WIFI_CMD_ERROR = 0,
    WIFI_CMD_RECONNECT,
    WIFI_CMD_DISCONNECT,
    WIFI_CMD_SWITCH_AP,
    WIFI_CMD_SWITCH_STA,
    WIFI_CMD_GET_INFO,
    WIFI_CMD_UDP_ON,
    WIFI_CMD_UDP_OFF,
    WIFI_CMD_WEB_ON,
    WIFI_CMD_WEB_OFF
} WIFI_CMD_TYPE;

typedef struct {
    TaskHandle_t htask_core;
    TaskHandle_t htask_udp;
    TaskHandle_t htask_web;
    QueueHandle_t queue_cmd;
    FileHandle *hfile;
} WifiHandle;

typedef struct {
    uint8_t type;
    uint16_t length;
    uint8_t *data;
} WifiMsg;

void initWifiPeripheral(void);
void createWifiTask(void);
uint8_t sendWifiMsg(uint8_t type, uint8_t *data, uint16_t length);

#endif // !_TASK_WIFI_H_
