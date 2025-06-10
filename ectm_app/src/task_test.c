#include "task_test.h"
#include "esp_log.h"

TestHandle htest;

void testCoreTask(void *param)
{
    while (1) {
        // 获取任务数量
        ESP_LOGI("test_core", "running...");

        // 模拟任务处理
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void createTestTask(void)
{
    xTaskCreatePinnedToCore(testCoreTask, "test_core", TASK_STACK_SMALL, NULL, TASK_PRIO_LOW, &htest.htask, APP_CPU_NUM);
}
