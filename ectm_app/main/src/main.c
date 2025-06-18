#include "task_config.h"
#include "task_file.h"
#include "task_uart.h"
#include "task_wifi.h"
#include "task_test.h"

void app_main(void) {
    /*---------- 初 始 化 外 设 ----------*/
    initSpiffsPeripheral();
    initUartPeripheral();
    initWifiPeripheral();

    /*---------- 创  建  任  务 ----------*/
    vTaskSuspendAll();
    createTestTask();
    createWifiTask();
    xTaskResumeAll();
    
    /*---------- 测  试  代  码 ----------*/

}
