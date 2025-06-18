#include "task_test.h"
#include "task_file.h"
#include "task_wifi.h"
#include "string.h"

TestHandle htest1;
TestHandle htest2;

/// @brief test1_core 任务函数
/// @param param 
void test1CoreTask(void *param) {
    static const char* TAG = "test1_core";
    TestHandle *htest = (TestHandle*)param;
    ESP_UNUSED(htest);
    char tasks_info[256] = {0};

    while (1) {
        ESP_LOGI(TAG, "Free heap size: %u bytes", xPortGetFreeHeapSize());
        vTaskList(tasks_info);
        // ESP_LOGI(TAG, "\nTask Name\tStatus\tPrio\tStack\t#Core\tNum\n%s", tasks_info);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/// @brief test2_core 任务函数
/// @param param 
void test2CoreTask(void *param) {
    static const char* TAG = "test2_core";
    TestHandle *htest = (TestHandle*)param;
    ESP_UNUSED(TAG);
    ESP_UNUSED(htest);

    vTaskDelay(pdMS_TO_TICKS(1000));
    sendWifiMsg(WIFI_CMD_DISCONNECT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendWifiMsg(WIFI_CMD_RECONNECT, NULL, 0);
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief 创建 task 任务
/// @param  
void createTestTask(void) {
    htest1.htask = NULL;    
    xTaskCreatePinnedToCore(test1CoreTask, "test1_core", TASK_STACK_LARGE, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
    htest2.htask = NULL;    
    xTaskCreatePinnedToCore(test2CoreTask, "test2_core", TASK_STACK_LARGE, &htest1, TASK_PRIO_LOW, &htest1.htask, APP_CPU_NUM);
}
