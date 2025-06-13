#include "task_files.h"
#include "esp_spiffs.h"

FilesHandle hfiles;

/// @brief 从文件读取 JSON 对象
/// @param path 文件绝对路径，例如 "/spiffs/config.json"
/// @param root cJSON 对象的根节点
/// @return 如果读取成功返回 true，否则返回 false
bool readJsonRoot(const char *path, cJSON **root)
{
    const char *TAG = "readJsonRoot";

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

    // 空文件读取
    if (read_len > 0) {
        // 解析为 json 格式
        *root = cJSON_Parse(data);
        free(data);
        if (*root == NULL) {
            ESP_LOGE(TAG, "failed to parse json");
            return false;
        }
    }
    else *root = cJSON_CreateObject();

    return true;
}

/// @brief 将 JSON 数据写入文件
/// @param path 文件绝对路径，例如 "/spiffs/config.json"
/// @param root cJSON 对象的根节点
/// @return 如果写入成功返回 true，否则返回 false
bool writeJsonRoot(const char *path, cJSON *root)
{
    const char *TAG = "writeJsonRoot";
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
/// @param keys      json 层级路径
/// @param value     不为 NULL 时设置最深层 key 的字符串值，NULL 时不修改，只返回对象
/// @return          返回最深层 cJSON 对象（或字符串节点），失败返回 NULL
cJSON* readWriteJsonObject(cJSON *father, char *keys, char *value)
{
    const char *TAG = "getJsonObject";
    if (father == NULL || keys == NULL || *keys == '\0') {
        ESP_LOGE(TAG, "invalid parameters");
        return NULL;
    }

    // 读取一个 key
    char key[FILES_KEYS_SIZE] = {0};
    uint8_t idx = 0;
    while (keys[idx] != '\\' && keys[idx] != '\0' && idx < FILES_KEYS_SIZE) {
        key[idx] = keys[idx];
        idx++;
    }
    key[idx] = '\0';
    bool is_end = (keys[idx] == '\0');
    ESP_LOGI(TAG, "%s", key);
    
    cJSON *child = cJSON_GetObjectItem(father, key);
    if (value == NULL) {
        // 查询模式
        if (child == NULL || is_end) return child;
        else return readWriteJsonObject(child, keys + idx + 1, value);
    } 
    else {
        // 写入模式
        if (is_end) {
            cJSON *new_item = cJSON_CreateString(value);
            if (new_item == NULL) {
                ESP_LOGE(TAG, "failed to create json string for key: %s", key);
                return NULL;
            }
            cJSON_ReplaceItemInObject(father, key, new_item);
            return new_item;
        } 
        else {
            if (child == NULL) {
                child = cJSON_CreateObject();
                if (child == NULL) {
                    ESP_LOGE(TAG, "failed to create object for key: %s", key);
                    return NULL;
                }
                cJSON_AddItemToObject(father, key, child);
            }
            return readWriteJsonObject(father, keys + idx + 1, value);
        }
    }
}

/// @brief files_core 任务函数
/// @param param 
void filesCoreTask(void *param) {
    const char *TAG = pcTaskGetName(NULL);
    FilesHandle* hfiles = (FilesHandle*)param;
    FilesMsg req_msg, resp_msg;

    while (1) {
        if (xQueueReceive(hfiles->req_queue, &req_msg, portMAX_DELAY) == pdTRUE) {
            // 读取对应的 json 文件
            cJSON *root = NULL;
            readJsonRoot(req_msg.path, &root);
            ESP_LOGI(TAG, "%s", cJSON_Print(root));
            if (root == NULL) {
                ESP_LOGE(TAG, "Failed to parse JSON from %s", req_msg.path);
                continue;
            }

            // 处理不同的操作
            if (req_msg.mode == FILES_MODE_READ) {
                cJSON *obj = readWriteJsonObject(root, req_msg.keys, NULL);
                strcpy(resp_msg.value, (obj == NULL) ? "" : obj->valuestring);
                resp_msg.resualt = (obj != NULL);
                xQueueSend(req_msg.resp_queue, &resp_msg, WAIT_TIME_MEDIUM);
            }
            else if (req_msg.mode == FILES_MODE_WRITE) {
                readWriteJsonObject(root, req_msg.keys, req_msg.value);
                writeJsonRoot(req_msg.path, root);
            }

            cJSON_Delete(root);
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

/// @brief 请求读取文件参数
/// @param path 文件路径
/// @param keys 多层 JSON 键路径
/// @param key_layer 实际使用了多少层键
/// @param value 返回读取的值
/// @return 如果读取成功返回 true，否则返回 false
bool filesReadParam(char *path, char *keys, char *value)
{
    const char *TAG = "filesReadParam";
    FilesMsg req_msg = {0};
    strcpy(req_msg.path, path);
    strcpy(req_msg.keys, keys);
    req_msg.mode = FILES_MODE_READ;
    req_msg.resp_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(FilesMsg));

    if (xQueueSend(hfiles.req_queue, &req_msg, WAIT_TIME_MEDIUM) == pdTRUE) {
        if (xQueueReceive(req_msg.resp_queue, &req_msg, WAIT_TIME_MEDIUM) == pdTRUE) {
            if (req_msg.resualt) strcpy(value, req_msg.value);
        }
    }

    vQueueDelete(req_msg.resp_queue);
    return req_msg.resualt;
}

/// @brief 请求写入文件参数
/// @param path 文件路径
/// @param keys 多层 JSON 键路径
/// @param key_layer 实际使用了多少层键
/// @param value 填写需要写入的字符串
/// @return 如果写入成功返回 true，否则返回 false
bool filesWriteParam(char *path, char *keys, char *value)
{
    const char *TAG = "filesWriteParam";
    FilesMsg req_msg = {0};
    strcpy(req_msg.path, path);
    strcpy(req_msg.keys, keys);
    strcpy(req_msg.value, value);
    req_msg.mode = FILES_MODE_WRITE;

    if (xQueueSend(hfiles.req_queue, &req_msg, WAIT_TIME_MEDIUM) == pdTRUE) return true;
    else return false;
}
