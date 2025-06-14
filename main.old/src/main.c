#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <sys/stat.h>
#include <dirent.h>

#include "task_test.h"
#include "task_files.h"
#include "task_console.h"
#include "task_uart.h"
#include "task_bluetooth.h"
#include "task_wifi.h"

void initSpifsPeriph(void);
void initUartPeriph(void);
void initWifiPeriph(void);
void initBluetoothPeriph(void);

void app_main(void)
{

    /*---------- 外设初始化 ----------*/
    initSpifsPeriph();
    initUartPeriph();
    initWifiPeriph();
    initBluetoothPeriph();

    /*---------- 创建任务 ----------*/
    vTaskSuspendAll();      // 暂停任务调度
    createTestTask();
    createFilesTask();
    createConsoleTask();
    createUartTask();
    createBluetoothTask();
    createWifiTask();
    xTaskResumeAll();       // 恢复任务调度
}

/// @brief 初始化SPIFFS外设（使用 ESP_ERROR_CHECK）
void initSpifsPeriph(void)
{
    const char *TAG = "spiffs";
    ESP_LOGI(TAG, "start initialization");

    // 配置 SPIFFS 注册文件系统
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    // 获取分区信息
    size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(NULL, &total, &used));
    ESP_LOGI(TAG, "spiffs mounted: total=%d bytes, used=%d bytes", total, used);

    // 遍历文件列表
    ESP_LOGI(TAG, "listing files in /spiffs:");
    DIR *dir = opendir("/spiffs");
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            ESP_LOGI(TAG, "file: %s", entry->d_name);
        }
        closedir(dir);
    } 
    else ESP_LOGE(TAG, "failed to open /spiffs directory");

    // 定义默认文件路径
    const char *default_files[] = {
        "/spiffs/test.json",
        "/spiffs/config.json",
        "/spiffs/data.json"
    };

    // 检查大小并且不存在就会创建
    struct stat st;
    for (int i = 0; i < sizeof(default_files) / sizeof(default_files[0]); i++) {
        const char *path = default_files[i];
        if (stat(path, &st) == 0) ESP_LOGI(TAG, "found %s (size: %ld bytes)", path, st.st_size);
        else {
            ESP_LOGW(TAG, "%s not found, creating new file", path);
            FILE *fp = fopen(path, "w");
            if (fp == NULL) {
                ESP_LOGE(TAG, "failed to create file: %s", path);
                continue;
            }
            fclose(fp);
            ESP_LOGI(TAG, "Created default file: %s", path);
        }
    }

    ESP_LOGI(TAG, "initialization complete");
}

/// @brief 初始化UART外设
void initUartPeriph(void)
{

}

/// @brief 初始化WiFi外设
void initWifiPeriph(void)
{
    const char *TAG = "initWifiPeriph";
    ESP_LOGI(TAG, "start initialization");

    // 初始化 NVS（WiFi 驱动依赖它）
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "nvs init failed, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化 TCP/IP 栈
    ESP_ERROR_CHECK(esp_netif_init());

    // 创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 创建默认的 WiFi STA 接口
    esp_netif_create_default_wifi_sta();

    // 初始化 WiFi 驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 设置 WiFi 模式为 STA（station 模式）
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_LOGI(TAG, "initialization complete");
}

/// @brief 初始化蓝牙外设
void initBluetoothPeriph(void)
{
    
}
