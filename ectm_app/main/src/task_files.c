#include "task_files.h"
#include <stdio.h>
#include <string.h>
#include "esp_spiffs.h"
#include "cJSON.h"

FilesHandle hfiles;

/// @brief 从文件读取 JSON 对象
/// @param path 文件绝对路径，例如 "/spiffs/config.json"
/// @param root cJSON 对象的根节点
/// @return 如果读取成功返回 true，否则返回 false
bool readJsonRoot(const char *path, cJSON **root)
{
    const char *TAG = "readJsonRoot";
    
    // 参数有效性检查
    if (path == NULL || root == NULL) {
        ESP_LOGE(TAG, "invalid parameters");
        return false;
    }

    // 打开文件
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "failed to open file for reading: %s", path);
        return false;
    }

    // 获取数据长度
    fseek(f, 0, SEEK_END);
    uint32_t data_len = ftell(f);
    rewind(f);

    // 分配缓存区
    char *data = (char*)malloc(data_len + 1);
    if (data == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory");
        fclose(f);
        return false;
    }

    // 读取数据
    uint32_t read_len = fread(data, 1, data_len, f);
    data[data_len] = '\0';
    fclose(f);
    if (read_len <= 0) {
        ESP_LOGE(TAG, "file length invalid");
        return false;
    }

    // 解析为 json 格式
    *root = cJSON_Parse(data);
    free(data);
    if (*root == NULL) {
        ESP_LOGE(TAG, "failed to parse json");
        return false;
    }

    return true;
}

/// @brief 将 JSON 数据写入文件
/// @param path 文件绝对路径，例如 "/spiffs/config.json"
/// @param root cJSON 对象的根节点
/// @return 如果写入成功返回 true，否则返回 false
bool writeJsonRoot(const char *path, cJSON *root)
{
    const char *TAG = "writeJsonRoot";

    // 参数有效性检查
    if (path == NULL|| root == NULL) {
        ESP_LOGE(TAG, "invalid parameters");
        return false;
    }

    // 生成字符串
    char *json_str = cJSON_Print(root);
    if (json_str == NULL) {
        ESP_LOGE(TAG, "failed to print json");
        return false;
    }

    // 打开文件
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "failed to open file for writing: %s", path);
        free(json_str);
        return false;
    }

    // 写入内容
    fwrite(json_str, 1, strlen(json_str), f);
    fclose(f);
    free(json_str);

    return true;
}

/// @brief 递归获取或创建多层嵌套的 JSON 对象，最深层可设置字符串值
/// @param father    父对象（必须是 cJSON 对象）
/// @param keys      字符串数组，每层 key
/// @param layer     层数
/// @param value     不为 NULL 时设置最深层 key 的字符串值，NULL 时不修改，只返回对象
/// @return          返回最深层 cJSON 对象（或字符串节点），失败返回 NULL
cJSON* getJsonObject(cJSON *father, char **keys, uint8_t layer, char *value) {
    const char *TAG = "getJsonObject";

    if (!father || !keys || layer == 0) {
        ESP_LOGE(TAG, "invalid parameters");
        return NULL;
    }

    // 当前层对象
    cJSON *child = cJSON_GetObjectItem(father, keys[0]);
    // 不存在，创建空对象
    if (child == NULL) {
        child = cJSON_CreateObject();
        if (!child) {
            ESP_LOGE(TAG, "failed to create json object");
            return NULL;
        }
        cJSON_AddItemToObject(father, keys[0], child);
        ESP_LOGI(TAG, "created empty json object for key: %s", keys[0]);
    }

    if (layer == 1) {
        // 按需修改元素
        if (value != NULL) {
            cJSON_ReplaceItemInObject(father, keys[0], cJSON_CreateString(value));
            child = cJSON_GetObjectItem(father, keys[0]);
            ESP_LOGI(TAG, "set string value for key: %s to \"%s\"", keys[0], value);
        }
        return child;
    }
    else return getJsonObject(child, keys + 1, layer - 1, value);
}

/// @brief files_core 任务函数
/// @param param 
void filesCoreTask(void* param) {
    const char *TAG = pcTaskGetName(NULL);
    FilesHandle* hfiles = (FilesHandle*)param;
    FilesMsg files_msg;

    while (1) {
        if (xQueueReceive(hfiles->req_queue, &files_msg, portMAX_DELAY) == pdTRUE) {
            
        }
    }
}

/// @brief 创建 files 任务
/// @param  
void createFilesTask(void)
{
    hfiles.htask = NULL;
    hfiles.req_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(FilesMsg));
    xTaskCreatePinnedToCore(filesCoreTask, "files_core", TASK_STACK_LARGE, &hfiles, TASK_PRIO_NORMAL, &hfiles.htask, APP_CPU_NUM);
}

/// @brief 发送文件操作消息
/// @param msg 指向 FilesMsg 结构体的指针，包含文件操作的详细信息
/// @return pdTRUE 发送成功，pdFALSE 超时或失败
BaseType_t sendFileMsg(FilesMsg *msg) {
    if (msg == NULL) return pdFALSE;
    return xQueueSend(hfiles.req_queue, msg, pdMS_TO_TICKS(1000));
}
