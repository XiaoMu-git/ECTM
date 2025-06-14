#include "task_test.h"
#include "task_file.h"
#include "string.h"

TestHandle htest1;
TestHandle htest2;
TestHandle htest3;
TestHandle htest4;

/// @brief test1_core 任务函数
/// @param param 
void test1CoreTask(void *param)
{
    const char *TAG = pcTaskGetName(NULL);
    // TestHandle *htest = (TestHandle *)param;
    char task_list[512]; 
    
    while (true) {
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
        vTaskList(task_list);
        ESP_LOGI(TAG, "\nTask Name\tStatus\tPrio\tStack\t#Core\tNum\n%s", task_list);
    }
}

/// @brief test2_core 任务函数
/// @param param 
void test2CoreTask(void *param)
{
    const char *TAG = pcTaskGetName(NULL);
    // TestHandle *htest = (TestHandle *)param;

    char write_data[32] = "test_ssid";
    char read_data[32] = {0};

    while (true) {
        if (fileWriteTest("config/wifi/ssid", write_data))
            ESP_LOGI(TAG, "write_data = %s", write_data);
        if (fileReadTest("config/wifi/ssid", read_data))
            ESP_LOGI(TAG, "read_data = %s", read_data);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/// @brief 创建 test 任务
void createTestTask(void)
{
    htest1.htask = NULL;
    xTaskCreatePinnedToCore(test1CoreTask, "test1_core", TASK_STACK_SMALL, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
    htest2.htask = NULL;
    xTaskCreatePinnedToCore(test2CoreTask, "test2_core", TASK_STACK_SMALL, &htest2, TASK_PRIO_LOW, &htest2.htask, APP_CPU_NUM);
}
