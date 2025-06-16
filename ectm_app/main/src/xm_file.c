#include "xm_file.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"

/// @brief spiffs 外设初始化
/// @param  
void initSpiffs(void) {
    const char *TAG = "initSpiffs";
    const char *files[] = {
        "/spiffs/test.json",
        "/spiffs/bluetooth.json",
        "/spiffs/gpio.json",
        "/spiffs/uart.json",
        "/spiffs/wifi.json"
    };

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

    // 检查大小并且不存在就会创建
    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        struct stat st;
        const char *path = files[i];
        if (stat(path, &st) == 0) ESP_LOGI(TAG, "found %s (size: %ld bytes)", path, st.st_size);
        else {
            ESP_LOGW(TAG, "%s not found, creating new file", path);
            FILE *fp = fopen(path, "w");
            if (fp == NULL) {
                ESP_LOGE(TAG, "failed to create file: %s", path);
                continue;
            }
            ESP_ERROR_CHECK(fclose(fp));
            ESP_LOGI(TAG, "created default file: %s", path);
        }
    }

    ESP_LOGI(TAG, "initialization complete");
}

/// @brief 从文件中读取指定长度的数据
/// @param path 要读取的文件路径，例如 "/spiffs/config.json"
/// @param buff 接收读取数据的缓冲区指针（必须预先分配好）
/// @param length 最多读取的数据长度（含 '\0'）
/// @return 实际读取的字节数，若打开文件失败返回 0
size_t readFile(const char *path, char *buff, uint32_t length) {
    if (path == NULL || buff == NULL) return 0;

    // 打开文件
    FILE *f = fopen(path, "r");
    if (f == NULL) return 0;

    // 读取数据
    uint32_t read_length = fread(buff, 1, length - 1, f);
    fclose(f);
    buff[read_length] = '\0';

    return read_length;
}

/// @brief 将数据写入文件，覆盖原内容
/// @param path 要写入的文件路径，例如 "/spiffs/config.json"
/// @param buff 要写入的数据缓冲区指针
/// @param length 要写入的数据长度（字节数）
/// @return 实际写入的字节数，若打开文件失败返回 0
size_t writeFile(const char *path, char *buff, uint32_t length) {
    if (path == NULL || buff == NULL) return 0;

    // 打开文件
    FILE *f = fopen(path, "w");
    if (f == NULL) return 0;

    // 写入数据
    size_t write_length = fwrite(buff, 1, length, f);
    fclose(f);

    return write_length;
}

/// @brief 获取或递归创建指定路径下的 JSON 节点（形如 "config/network/ip"）
/// @param root JSON 根节点指针（cJSON 对象）
/// @param keys 路径字符串，以 '/' 分隔每级节点，如 "config/network/ip"
/// @return 返回路径末尾对应的 cJSON 节点指针，失败则返回 NULL
cJSON* getJsonNode(cJSON *root, char *keys) {
    if (root == NULL || keys == NULL || *keys == '\0') return NULL;

    char *buff = (char*)malloc(strlen(keys) + 1);
    if (buff == NULL) return NULL;
    strcpy(buff, keys);
    cJSON *ret = NULL;
    
    for (char *left = buff, *right = buff; true; right++) {
        if (*right == '/' || *right == '\0') {
            char temp = *right;
            *right = '\0';
            cJSON *child = cJSON_GetObjectItem(root, left);
            if (child == NULL) {
                child = cJSON_CreateObject();
                if (child == NULL) {
                    free(buff);
                    return NULL;
                }
                cJSON_AddItemToObject(root, left, child);
            }

            ret = child;
            if (temp == '\0') break;
            left = right + 1;
        }
    }

    free(buff);
    return ret;
}
