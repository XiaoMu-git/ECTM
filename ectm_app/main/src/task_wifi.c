#include "task_wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

WifiHandle hwifi;

/// @brief wifi_core 任务函数
/// @param param 
void wifiCoreTask(void *param) 
{
    const char *TAG = pcTaskGetName(NULL);
    WifiHandle *hwifi = (WifiHandle*)param;
    WifiMsg *req_msg;

    while (true) {
        if (xQueueReceive(hwifi->cmd_queue, &req_msg, portMAX_DELAY) == pdTRUE) {
            
        }
    }
}

/// @brief 创建 wifi 任务
/// @param  
void createWifiTask(void)
{
    hwifi.htask = NULL;
    hwifi.cmd_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(WifiMsg));
    xTaskCreatePinnedToCore(wifiCoreTask, "wifi_core", TASK_STACK_LARGE, &hwifi, TASK_PRIO_NORMAL, &hwifi.htask, APP_CPU_NUM);
}
