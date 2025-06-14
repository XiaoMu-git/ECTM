#include "task_wifi.h"
#include "task_files.h"
#include "esp_wifi.h"

WifiHandle hwifi;

/// @brief wifi_core 任务函数
/// @param param 
void wifiCoreTask(void *param)
{
    const char *TAG = pcTaskGetName(NULL);
    WifiHandle *hwifi = (WifiHandle *)param;
    WifiMsg msg;

    while (1) {
        if (xQueueReceive(hwifi->cmd_queue, &msg, WAIT_TIME_LONG) == pdTRUE) {
            if (msg.cmd == WIFI_CMD_CONNECT) {
                ESP_LOGI(TAG, "Connecting to WiFi...");
                // 连接 WiFi 的逻辑可以在这里实现
            } 
            else if (msg.cmd == WIFI_CMD_DISCONNECT) {
                ESP_LOGI(TAG, "Disconnecting from WiFi...");
                // 断开 WiFi 的逻辑可以在这里实现
            } 
            else if (msg.cmd == WIFI_CMD_SCAN) {
                ESP_LOGI(TAG, "Scanning for WiFi networks...");
                // 扫描 WiFi 的逻辑可以在这里实现
            } 
            else if (msg.cmd == WIFI_CMD_GET_STATUS) {
                ESP_LOGI(TAG, "Getting WiFi status...");
                // 获取 WiFi 状态的逻辑可以在这里实现
            } 
            else {
                ESP_LOGW(TAG, "Unknown command: %d", msg.cmd);
            }
        }

        // 自动重连机制
        // 允许连接并且没有建立连接
        if (hwifi->is_connected) {
            // 读取 wifi 账号密码
            
            // 配置 WiFi SSID 和密码

            // 启动 WiFi

            // 尝试连接
        }
    }
}

/// @brief 创建 wifi 任务
void createWifiTask(void)
{
    hwifi.htask = NULL;
    hwifi.cmd_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(WifiMsg));
    hwifi.is_connected = true;
    // xTaskCreatePinnedToCore(wifiCoreTask, "wifi_core", TASK_STACK_LARGE, &hwifi, TASK_PRIO_NORMAL, &hwifi.htask, APP_CPU_NUM);
}
