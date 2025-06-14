#include "task_file.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"

FileHandle hfile;

/// @brief 从文件中读取 cJSON 对象
/// @param path 
/// @param root 
/// @return 
bool readJsonRoot(const char *path, cJSON **root) {
    // const char *TAG = "readJsonRoot";
    if (path == NULL || root == NULL) return false;

    // 打开 JSON 文件（只读模式）
    FILE *f = fopen(path, "r");
    if (f == NULL) return false;

    // 获取文件大小
    fseek(f, 0, SEEK_END);
    uint32_t data_len = ftell(f);
    rewind(f);

    // 分配缓存并读取文件内容
    char *data = (char*)malloc(data_len + 1);
    if (data == NULL) {
        fclose(f);
        return false;
    }
    fread(data, 1, data_len, f);
    fclose(f);
    data[data_len] = '\0';

    // 解析 JSON 文本为 cJSON 对象
    *root = cJSON_Parse(data);
    free(data);

    // 如果解析失败，则创建一个空对象作为根
    if (*root == NULL) {
        *root = cJSON_CreateObject();
        if (*root == NULL) return false;
    }

    return true;
}

/// @brief 保存 cJSON 对象到文件中
/// @param path 
/// @param root 
/// @return 
bool writeJsonRoot(const char *path, cJSON **root) {
    // const char *TAG = "writeJsonRoot";
    if (path == NULL|| *root == NULL) return false;

    // 将 cJSON 对象转为字符串
    char *json_str = cJSON_Print(*root);
    if (json_str == NULL) return false;

    // 打开文件，写入文件
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        free(json_str);
        return false;
    }
    fwrite(json_str, 1, strlen(json_str), f);
    fclose(f);
    free(json_str);

    return true;
}

/// @brief 读写多层次 cJSON 对象
/// @param root 根对象
/// @param keys 以 `/` 分隔的键路径，如 "config/network/ip"
/// @param value 要设置的值；如果为 NULL，则为读取模式
/// @return 返回目标节点指针（读取或新建）
cJSON* readWriteJsonObject(cJSON *root, char *keys, char *value) {
    // const char *TAG = "readWriteJsonObject";
    if (root == NULL || keys == NULL || *keys == '\0') return NULL;

    // 拷贝键路径到缓冲区并初始化指针
    uint32_t buff_len = strlen(keys);
    char *buff = (char*)malloc(buff_len + 1);
    char *ptr = buff;
    strcpy(buff, keys);
    
    // 遍历路径，按 '/' 分隔访问每一层
    for (int i = 0; i <= buff_len; i++) {
        if (buff[i] == '/') {
            buff[i] = '\0';  // 截断当前键

            // 查找子节点
            cJSON *child = cJSON_GetObjectItem(root, ptr);

            if (value == NULL) {
                // 如果读取但找不到节点，返回 NULL
                if (child == NULL) {
                    free(buff);
                    return NULL;
                }
            }
            else {
                // 如果写入但节点不存在，则创建
                if (child == NULL) {
                    child = cJSON_CreateObject();
                    if (child == NULL) {
                        free(buff);
                        return NULL;
                    }
                    cJSON_AddItemToObject(root, ptr, child);
                }
            }

            // 进入下一层
            root = child;
            ptr = buff + i + 1;
        }
        else if (buff[i] == '\0') {
            // 最后一层，读取或写入值
            cJSON *child = cJSON_GetObjectItem(root, ptr);

            if (value == NULL) {
                // 读取模式，直接返回该节点
                free(buff);
                return child;
            }
            else {
                // 写入模式，创建新字符串节点
                cJSON *new_item = cJSON_CreateString(value);
                if (new_item == NULL) {
                    free(buff);
                    return NULL;
                }

                // 如果存在则替换，不存在则添加
                if (child == NULL) cJSON_AddItemToObject(root, ptr, new_item);
                else cJSON_ReplaceItemInObject(root, ptr, new_item);

                free(buff);
                return new_item;
            }
        }
    }

    free(buff);
    return NULL;
}

/// @brief file_core 任务函数
/// @param param 
void fileCoreTask(void *param) {
    const char *TAG = pcTaskGetName(NULL);
    FileHandle *hfile = (FileHandle*)param;
    FileMsg req_msg, resp_msg;

    while (true) {
        if (xQueueReceive(hfile->req_queue, &req_msg, portMAX_DELAY) == pdTRUE) {
            cJSON *root = NULL;
            cJSON *obj = NULL;
            // 从文件中读取 JSON 根对象
            if (readJsonRoot(req_msg.path, &root)) {
                // 根据请求类型执行读或写操作
                if (req_msg.mode == FILE_MODE_READ) obj = readWriteJsonObject(root, req_msg.keys, NULL);
                else if (req_msg.mode == FILE_MODE_WRITE) {
                    obj = readWriteJsonObject(root, req_msg.keys, req_msg.value);
                    writeJsonRoot(req_msg.path, &root);
                }
            }

            // 构造响应消息
            if (obj != NULL && obj->valuestring != NULL) {
                strcpy(resp_msg.value, obj->valuestring);
                resp_msg.resualt = true;
            }
            else {
                strcpy(resp_msg.value, "None");
                resp_msg.resualt = false;
            }
            if (root != NULL) cJSON_Delete(root);

            // 将响应消息发送回响应队列
            if (req_msg.resp_queue != NULL) xQueueSend(req_msg.resp_queue, &resp_msg, WAIT_TIME_MEDIUM);
        }
    }
}

/// @brief 创建 files 任务
/// @param  
void createFileTask(void)
{
    hfile.htask = NULL;
    hfile.req_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(FileMsg));
    xTaskCreatePinnedToCore(fileCoreTask, "file_core", TASK_STACK_LARGE, &hfile, TASK_PRIO_NORMAL, &hfile.htask, APP_CPU_NUM);
}

/// @brief 控制文件读写操作（同步）
/// @param path 文件路径
/// @param keys 键路径（支持多级 / 分隔）
/// @param value 输入/输出字符串（读时为输出）
/// @param mode FILE_MODE_READ / FILE_MODE_WRITE
/// @return true: 操作成功，false: 操作失败
bool fileControl(char *path, char *keys, char *value, uint8_t mode) {
    // const char* TAG = "fileControl";
    if (path == NULL || keys == NULL) return false;

    FileMsg req_msg = {0}, resp_msg = {0};
    
    // 参数拷贝
    if (path != NULL) strcpy(req_msg.path, path);
    if (keys != NULL) strcpy(req_msg.keys, keys);
    if (value != NULL) strcpy(req_msg.value, value);
    req_msg.mode = mode;
    req_msg.resp_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(FileMsg));
    if (req_msg.resp_queue == NULL) return false;

    // 发送请求并等待响应
    if (xQueueSend(hfile.req_queue, &req_msg, WAIT_TIME_MEDIUM) == pdTRUE) {
        if (xQueueReceive(req_msg.resp_queue, &resp_msg, WAIT_TIME_MEDIUM) == pdTRUE) {
            if (resp_msg.resualt && value != NULL) strcpy(value, resp_msg.value);
        }
    }

    // 释放响应队列
    vQueueDelete(req_msg.resp_queue);
    return resp_msg.resualt;
}
