#include "task_test.h"
#include "xm_file.h"
#include <string.h>

TestHandle htest1;
TestHandle htest2;

/// @brief test1_core 任务函数
/// @param param 
void test1CoreTask(void *param) {
    const char *TAG = pcTaskGetName(NULL);
    TestHandle *htest = (TestHandle*)param;
    char tasks_info[256] = {0};

    while (1) {
        ESP_LOGI(TAG, "Free heap size: %u bytes", xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Free heap size: %u bytes", xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Free heap size: %u bytes", xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Free heap size: %u bytes", xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Free heap size: %u bytes", xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskList(tasks_info);
        ESP_LOGI(TAG, "\nTask Name\tStatus\tPrio\tStack\t#Core\tNum\n%s", tasks_info);
    }
}

/// @brief test2_core 任务函数
/// @param param 
void test2CoreTask(void *param) {
    const char *TAG = pcTaskGetName(NULL);
    TestHandle *htest = (TestHandle*)param;
    const char *file_path = "/spiffs/test.json";
    char file_data[512] = {0};

    writeFile(file_path, "{\n\t\"test\":\t\"test\"\n}", strlen("{\n\t\"test\":\t\"test\"\n}"));
    while (1) {
        // === 阶段 1：读取文件 ===
        size_t read_length = readFile(file_path, file_data, sizeof(file_data));
        if (read_length == 0) ESP_LOGE(TAG, "failed to read file or file is empty: %s", file_path);
        else ESP_LOGI(TAG, "file read successfully %u bytes: %s ", read_length, file_data);

        // === 阶段 2：写入文件 ===
        size_t write_length = writeFile(file_path, file_data, strlen(file_data));
        if (write_length == 0) ESP_LOGE(TAG, "failed to write data to file: %s", file_path);
        else ESP_LOGI(TAG, "data written successfully: %s (%u bytes)", file_path, write_length);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief 创建 task 任务
/// @param  
void createTestTask(void) {
    htest1.htask = NULL;    
    xTaskCreatePinnedToCore(test1CoreTask, "test1_core", TASK_STACK_SMALL, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
    htest2.htask = NULL;    
    xTaskCreatePinnedToCore(test2CoreTask, "test2_core", TASK_STACK_SMALL, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
}
