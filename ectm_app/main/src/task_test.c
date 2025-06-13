#include "task_test.h"
#include "task_files.h"
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

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        char task_list[512]; 
        vTaskList(task_list);
        ESP_LOGI(TAG, "\nTask Name\tStatus\tPrio\tStack\t#Core\tNum\n%s", task_list);
        size_t heap_remaining = xPortGetFreeHeapSize();
        ESP_LOGI(TAG, "Free heap size: %u bytes", heap_remaining);
    }
}

/// @brief test2_core 任务函数
/// @param param 
void test2CoreTask(void *param)
{
    const char *TAG = pcTaskGetName(NULL);
    // TestHandle *htest = (TestHandle *)param;

    while (1) {
        ESP_LOGI(TAG, "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief test3_core 任务函数
/// @param param 
void test3CoreTask(void *param)
{
    const char *TAG = pcTaskGetName(NULL);
    // TestHandle *htest = (TestHandle *)param;

    while (1) {
        ESP_LOGI(TAG, "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief test4_core 任务函数
/// @param param 
void test4CoreTask(void *param)
{
    const char *TAG = pcTaskGetName(NULL);
    // TestHandle *htest = (TestHandle *)param;
    
    while (1) {
        ESP_LOGI(TAG, "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief 创建 test 任务
void createTestTask(void)
{
    htest1.htask = NULL;
    xTaskCreatePinnedToCore(test1CoreTask, "test1_core", TASK_STACK_SMALL, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
    // htest2.htask = NULL;
    // xTaskCreatePinnedToCore(test2CoreTask, "test2_core", TASK_STACK_SMALL, &htest2, TASK_PRIO_LOW, &htest2.htask, APP_CPU_NUM);
    // htest3.htask = NULL;
    // xTaskCreatePinnedToCore(test3CoreTask, "test3_core", TASK_STACK_SMALL, &htest3, TASK_PRIO_LOW, &htest3.htask, APP_CPU_NUM);
    // htest4.htask = NULL;
    // xTaskCreatePinnedToCore(test4CoreTask, "test4_core", TASK_STACK_SMALL, &htest4, TASK_PRIO_LOW, &htest3.htask, APP_CPU_NUM);
}
