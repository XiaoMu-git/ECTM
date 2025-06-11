#include "task_test.h"
#include "esp_log.h"

TestHandle htest1;
TestHandle htest2;
TestHandle htest3;
TestHandle htest4;

void test1CoreTask(void *param)
{
    while (1) {
        char task_list[512]; 
        vTaskList(task_list);
        ESP_LOGI("test1_core", "\nTask Name\tStatus\tPrio\tStack\t#Core\tNum\n%s", task_list);
        size_t heap_remaining = xPortGetFreeHeapSize();
        ESP_LOGI("test1_core", "Free heap size: %u bytes", heap_remaining);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void test2CoreTask(void *param)
{
    while (1) {
        ESP_LOGI("test2_core", "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void test3CoreTask(void *param)
{
    while (1) {
        ESP_LOGI("test3_core", "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void test4CoreTask(void *param)
{
    while (1) {
        ESP_LOGI("test4_core", "running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void createTestTask(void)
{
    xTaskCreatePinnedToCore(test1CoreTask, "test1_core", TASK_STACK_LARGE, NULL, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
    xTaskCreatePinnedToCore(test2CoreTask, "test2_core", TASK_STACK_MEDIUM, NULL, TASK_PRIO_LOW, &htest2.htask, APP_CPU_NUM);
    xTaskCreatePinnedToCore(test3CoreTask, "test3_core", TASK_STACK_SMALL, NULL, TASK_PRIO_LOW, &htest3.htask, APP_CPU_NUM);
    xTaskCreatePinnedToCore(test4CoreTask, "test4_core", TASK_STACK_TINY, NULL, TASK_PRIO_LOW, &htest3.htask, APP_CPU_NUM);
}
