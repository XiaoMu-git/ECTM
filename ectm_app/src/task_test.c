#include "task_test.h"
#include <stdio.h>
#include <string.h>
#include "esp_spiffs.h"

TestHandle htest1;
TestHandle htest2;
TestHandle htest3;
TestHandle htest4;

/// @brief 从文件读取 JSON 数据
/// @param path 文件路径，例如 "/spiffs/config.json"
/// @param json 用户提供的缓冲区
/// @param len 输入：最大长度，输出：实际长度
/// @return 是否读取成功
bool readJsonFile(const char *path, char *json, uint16_t *len)
{
    const char *TAG = "readJsonFile";
    if (path == NULL || json == NULL || len == NULL) return false;

    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGW(TAG, "Failed to open file: %s", path);
        return false;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size <= 0 || size >= *len) {
        ESP_LOGW(TAG, "Invalid file size: %ld", size);
        fclose(f);
        return false;
    }

    size_t read_len = fread(json, 1, size, f);
    json[read_len] = '\0';
    *len = read_len;

    fclose(f);
    return true;
}

/// @brief 将 JSON 数据写入文件
/// @param path 文件路径
/// @param json 要写入的 JSON 字符串
/// @param len 要写入的长度（可为 0 表示使用 strlen）
/// @return 是否写入成功
bool writeJsonFile(const char *path, const char *json, uint16_t len)
{
    const char *TAG = "writeJsonFile";
    if (path == NULL || json == NULL) return false;

    if (len == 0) len = strlen(json);

    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", path);
        return false;
    }

    size_t write_len = fwrite(json, 1, len, f);
    fclose(f);

    if (write_len != len) {
        ESP_LOGW(TAG, "Write size mismatch: %d vs %d", (int)write_len, len);
        return false;
    }

    return true;
}

/// @brief test1_core 任务函数
/// @param param 
void test1CoreTask(void *param)
{
    TestHandle *htest = (TestHandle *)param;
    while (1) {
        char task_list[512]; 
        vTaskList(task_list);
        ESP_LOGI("test1_core", "\nTask Name\tStatus\tPrio\tStack\t#Core\tNum\n%s", task_list);
        size_t heap_remaining = xPortGetFreeHeapSize();
        ESP_LOGI("test1_core", "Free heap size: %u bytes", heap_remaining);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

/// @brief test2_core 任务函数
/// @param param 
void test2CoreTask(void *param)
{
    TestHandle *htest = (TestHandle *)param;
    char data1[256] = {0};
    char data2[256] = {0};
    uint16_t data_len = 0;
    strcpy(data1, "{\"name\":\"test\",\"value\":123}");
    data_len = strlen(data1);
    int ret1 = writeJsonFile("/spiffs/test.json", data1, data_len);
    if (ret1 == false) {
        ESP_LOGE("test2_core", "Failed to write JSON data to /spiffs/test.json");
        return;
    }
    ESP_LOGI("test2_core", "Wrote JSON data to /spiffs/test.json");
    int ret2 = readJsonFile("/spiffs/test.json", data2, &data_len);
    if (ret2 == false) {
        ESP_LOGE("test2_core", "Failed to read JSON data from /spiffs/test.json");
        return;
    }
    ESP_LOGI("test2_core", "Read JSON data from /spiffs/test.json: %s", data2);

    while (1) {
        ESP_LOGI("test2_core", "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief test3_core 任务函数
/// @param param 
void test3CoreTask(void *param)
{
    TestHandle *htest = (TestHandle *)param;
    while (1) {
        ESP_LOGI("test3_core", "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief test4_core 任务函数
/// @param param 
void test4CoreTask(void *param)
{
    TestHandle *htest = (TestHandle *)param;
    while (1) {
        ESP_LOGI("test4_core", "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief 创建 test 任务
void createTestTask(void)
{
    // htest1.htask = NULL;
    // xTaskCreatePinnedToCore(test1CoreTask, "test1_core", TASK_STACK_LARGE, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
    htest2.htask = NULL;
    xTaskCreatePinnedToCore(test2CoreTask, "test2_core", TASK_STACK_MEDIUM, &htest2, TASK_PRIO_LOW, &htest2.htask, APP_CPU_NUM);
    // htest3.htask = NULL;
    // xTaskCreatePinnedToCore(test3CoreTask, "test3_core", TASK_STACK_SMALL, &htest3, TASK_PRIO_LOW, &htest3.htask, APP_CPU_NUM);
    // htest4.htask = NULL;
    // xTaskCreatePinnedToCore(test4CoreTask, "test4_core", TASK_STACK_TINY, &htest4, TASK_PRIO_LOW, &htest3.htask, APP_CPU_NUM);
}
