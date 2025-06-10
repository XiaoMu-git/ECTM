#include "task_test.h"
#include "task_console.h"
#include "task_files.h"
#include "task_uart.h"
#include "task_bluetooth.h"
#include "task_wifi.h"

void initSpifsPeriph(void);
void initUartPeriph(void);
void initWifiPeriph(void);
void initBluetoothPeriph(void);

void app_main(void)
{
    // vTaskSuspendAll();  // 暂停任务调度

    /*---------- 外设初始化 ----------*/
    initSpifsPeriph();
    initUartPeriph();
    initWifiPeriph();
    initBluetoothPeriph();

    /*---------- 创建任务 ----------*/
    createTestTask();
    createConsoleTask();
    createFilesTask();
    createUartTask();
    createBluetoothTask();
    createWifiTask();

    // xTaskResumeAll();   // 恢复任务调度
}

/// @brief 初始化SPIFFS外设
void initSpifsPeriph(void)
{

}

/// @brief 初始化UART外设
void initUartPeriph(void)
{
    
}

/// @brief 初始化WiFi外设
void initWifiPeriph(void)
{
    
}

/// @brief 初始化蓝牙外设
void initBluetoothPeriph(void)
{
    
}
