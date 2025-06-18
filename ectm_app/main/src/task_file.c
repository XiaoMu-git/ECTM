#include "task_file.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "string.h"

FileHandle hfiles[] = {
    {"/spiffs/test.json", NULL, "{}"},
    {"/spiffs/wifi.json", NULL, "{\"is_open\":true,\"mode\":\"sta\",\"sta\":{\"ssid\":\"\",\"password\":\"\"},\"ap\":{\"ssid\":\"\",\"password\":\"\"},\"udp\":{\"is_open\":false,\"port\":10000},\"web\":{\"is_open\":false}}"},
    {"/spiffs/uart.json", NULL, "{}"},
    {"/spiffs/gpio.json", NULL, "{}"}
};

/// @brief spiffs 外设初始化
/// @param  
void initSpiffsPeripheral(void) {
    const char *TAG = "spiffInit";

    // 挂载 spiffs 分区
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 10,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
    ESP_LOGI(TAG, "SPIFFS partition mounting completed.");

    // 清空分区
    // esp_spiffs_format(NULL);

    // 获取 spiffs 分区信息
    size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(NULL, &total, &used));
    ESP_LOGI(TAG, "Memory: total(%u bytes) used(%u bytes).", total, used);

    // 创建默认文件
    for (size_t i = 0; i < sizeof(hfiles) / sizeof(hfiles[0]); i++) {
        hfiles[i].mutex = xSemaphoreCreateMutex();
        struct stat st;
        if (stat(hfiles[i].path, &st) != 0) {
            FILE *fp = fopen(hfiles[i].path, "w");
            if (fp == NULL) {
                ESP_LOGE(TAG, "Failed to create file: %s.", hfiles[i].path);
                continue;
            }
            fwrite(hfiles[i].data_defualt, 1, strlen(hfiles[i].data_defualt), fp);
            fclose(fp);
            ESP_LOGI(TAG, "Created default file: %s.", hfiles[i].path);
        }
        ESP_LOGI(TAG, "File info: %s, %ld bytes.", hfiles[i].path, st.st_size);
    }
    ESP_LOGI(TAG, "SPIFFS initialization completed.");
}

/// @brief 读取文件
/// @param hfile 文件句柄指针
/// @param buff 缓冲区指针
/// @param length 缓冲区长度
/// @return 实际读取的字节数（不含 '\0'），失败返回 0
size_t readFile(FileHandle *hfile, char *buff, uint32_t length) {
    if (hfile == NULL || buff == NULL) return 0;
    uint32_t read_length = 0;

    if (xSemaphoreTake(hfile->mutex, TIME_WAIT_SHORT) == pdTRUE) {
        FILE *fp = fopen(hfile->path, "r");
        if (fp == NULL) {
            xSemaphoreGive(hfile->mutex);
            return 0;
        }
        read_length = fread(buff, 1, length - 1, fp);
        buff[read_length] = '\0';
        fclose(fp);
        xSemaphoreGive(hfile->mutex);
    }

    return read_length;
}
/// @brief 写入文件
/// @param hfile 文件句柄指针
/// @param data 要写入的内容
/// @param length 要写入的字节数
/// @return 实际写入的字节数，失败返回 0
size_t writeFile(FileHandle *hfile, char *data, uint32_t length) {
    if (hfile == NULL || data == NULL) return 0;
    uint32_t write_length = 0;

    if (xSemaphoreTake(hfile->mutex, TIME_WAIT_SHORT) == pdTRUE) {
        FILE *fp = fopen(hfile->path, "w");
        if (fp == NULL) {
            return 0;
            xSemaphoreGive(hfile->mutex);
        }
        write_length = fwrite(data, 1, length, fp);
        fclose(fp);
        xSemaphoreGive(hfile->mutex);
    }

    return write_length;
}

/// @brief 按照路径获取指定对象
/// @param root 根节点
/// @param keys 路径，格式：/aa/bb/cc
/// @return 返回查找节点的指针
cJSON* getJsonItem(cJSON *root, const char *keys) {
    if (root == NULL || keys == NULL) return NULL;
    char buff[FILE_KEY_COUNT_MAX * FILE_KEY_SIZE_MAX];
    strcpy(buff, keys);

    char *ptr = NULL;
    char *key = strtok_r(buff, "/", &ptr);
    while (key != NULL) {
        cJSON *child = cJSON_GetObjectItem(root, key);
        if (child == NULL) {
            child = cJSON_CreateObject();
            if (child == NULL) return NULL;
            cJSON_AddItemToObject(root, key, child);
        }
        root = child;
        key = strtok_r(NULL, "/", &ptr);
    }

    return root;
}
