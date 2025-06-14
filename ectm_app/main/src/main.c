#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "driver/uart.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "task_test.h"
#include "task_file.h"
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
    createFileTask();
    createWifiTask();
    createConsoleTask();
    createUartTask();
    createBluetoothTask();
    xTaskResumeAll();       // 恢复任务调度
}

/// @brief 初始化SPIFFS外设
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
    if (dir == NULL) {
        ESP_LOGE(TAG, "failed to open /spiffs directory");
    } 
    else {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            ESP_LOGI(TAG, "file: %s", entry->d_name);
        }
        ESP_ERROR_CHECK(closedir(dir));
    }

    // 定义默认文件路径
    const char *default_files[] = {
        "/spiffs/test.json",
        "/spiffs/config.json",
        "/spiffs/data.json"
    };

    // 检查大小并且不存在就会创建
    struct stat st;
    for (size_t i = 0; i < sizeof(default_files) / sizeof(default_files[0]); i++) {
        const char *path = default_files[i];
        if (stat(path, &st) == 0) ESP_LOGI(TAG, "found %s (size: %ld bytes)", path, st.st_size);
        else {
            ESP_LOGW(TAG, "%s not found, creating new file", path);
            FILE *fp = fopen(path, "w");
            if (fp == NULL) {
                ESP_LOGE(TAG, "failed to create file: %s", path);
                continue;
            }
            ESP_ERROR_CHECK(fclose(fp));
            ESP_LOGI(TAG, "Created default file: %s", path);
        }
    }

    ESP_LOGI(TAG, "initialization complete");
}

/// @brief 初始化UART外设
void initUartPeriph(void)
{
    const char* TAG = "initUartPeriph";
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // 配置 UART 参数
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));

    // 设置 TX 和 RX 引脚
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // 安装 UART 驱动
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 2048, 0, 0, NULL, 0));

    ESP_LOGI(TAG, "uart initialized on port %d (TX: GPIO %d, RX: GPIO %d)", UART_NUM_1, 17, 16);
}

/// @brief 初始化WiFi外设
void initWifiPeriph(void)
{
    const char *TAG = "initWifiPeriph";
    ESP_LOGI(TAG, "start initialization");

    // 初始化 NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "nvs init failed, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } 
    else ESP_ERROR_CHECK(ret);

    // 初始化 TCP/IP 栈
    ESP_ERROR_CHECK(esp_netif_init());

    // 创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 创建默认的 WiFi STA 接口
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    if (netif == NULL) {
        ESP_LOGE(TAG, "failed to create default wifi sta interface");
        abort();
    }

    // 初始化 WiFi 驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 设置 WiFi 模式为 STA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_LOGI(TAG, "initialization complete");
}

/// @brief 初始化蓝牙外设
void initBluetoothPeriph(void)
{
    
}
