#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include <sys/stat.h>
#include <dirent.h>

#include "task_test.h"
#include "task_console.h"
#include "task_files.h"
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
    vTaskSuspendAll();  // 暂停任务调度
    createTestTask();
    createConsoleTask();
    createFilesTask();
    createUartTask();
    createBluetoothTask();
    createWifiTask();
    xTaskResumeAll();   // 恢复任务调度
}

/// @brief 初始化SPIFFS外设
void initSpifsPeriph(void)
{
    const char *TAG = "spiffs";
    ESP_LOGI(TAG, "start initialization");
    
    // 挂载文件系统
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",         // 挂载路径
        .partition_label = NULL,        // 默认分区标签
        .max_files = 5,                 // 最多同时打开文件数
        .format_if_mount_failed = true  // 如果挂载失败则格式化
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    
    // 检查挂载结果
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) ESP_LOGE(TAG, "failed to mount or format filesystem");
        else if (ret == ESP_ERR_NOT_FOUND) ESP_LOGE(TAG, "failed to find spiffs partition");
        else ESP_LOGE(TAG, "spiffs register failed (%s)", esp_err_to_name(ret));
        return;
    }

    // 获取分区信息
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) ESP_LOGE(TAG, "failed to get spiffs info (%s)", esp_err_to_name(ret));
    else ESP_LOGI(TAG, "spiffs mounted: total=%d bytes, used=%d bytes", total, used);

    // 检索所有文件
    ESP_LOGI(TAG, "listing files in /spiffs:");
    DIR *dir = opendir("/spiffs");
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) ESP_LOGI(TAG, "  File: %s", entry->d_name);
        closedir(dir);
    } 
    else ESP_LOGE(TAG, "failed to open \"/spiffs\" directory");

    // 检查 setting.json 和 data.json 是否存在
    struct stat st;
    char path[32];

    strcpy(path, "/spiffs/setting.json");
    if (stat(path, &st) == 0) ESP_LOGI(TAG, "found setting.json (size: %ld bytes)", st.st_size);
    else {
        ESP_LOGI(TAG, "%s not found, create new file", path);
        FILE *fp = fopen(path, "w");
        if (fp == NULL) {
            ESP_LOGE(TAG, "failed to create file: %s", path);
            return;
        }
        fclose(fp);
        ESP_LOGI(TAG, "Created default file: %s", path);
    }

    strcpy(path, "/spiffs/data.json");
    if (stat(path, &st) == 0) ESP_LOGI(TAG, "found setting.json (size: %ld bytes)", st.st_size);
    else {
        ESP_LOGI(TAG, "%s not found, create new file", path);
        FILE *fp = fopen(path, "w");
        if (fp == NULL) {
            ESP_LOGE(TAG, "failed to create file: %s", path);
            return;
        }
        fclose(fp);
        ESP_LOGI(TAG, "Created default file: %s", path);
    }

    strcpy(path, "/spiffs/test.json");
    if (stat(path, &st) == 0) ESP_LOGI(TAG, "found setting.json (size: %ld bytes)", st.st_size);
    else {
        ESP_LOGI(TAG, "%s not found, create new file", path);
        FILE *fp = fopen(path, "w");
        if (fp == NULL) {
            ESP_LOGE(TAG, "failed to create file: %s", path);
            return;
        }
        fclose(fp);
        ESP_LOGI(TAG, "Created default file: %s", path);
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
    
}

/// @brief 初始化蓝牙外设
void initBluetoothPeriph(void)
{
    
}
