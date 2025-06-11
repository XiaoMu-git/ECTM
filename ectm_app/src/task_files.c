#include "task_files.h"

FilesHandle hfiles;

/// @brief files_core 任务函数
/// @param param 
void filesCoreTask(void* param) {
    FilesHandle* hfiles = (FilesHandle*)param;
    FilesMsg req_msg, resp_msg;
    while (1) {
        if (xQueueReceive(hfiles->req_queue, &resp_msg, portMAX_DELAY) == pdTRUE) {
            
        }
    }
}

/// @brief 创建 files 任务
/// @param  
void createFilesTask(void)
{
    hfiles.htask = NULL;
    hfiles.req_queue = xQueueCreate(QUEUE_LENGTH_SMALL, sizeof(FilesMsg));
    xTaskCreatePinnedToCore(filesCoreTask, "files_core", TASK_STACK_LARGE, &hfiles, TASK_PRIO_NORMAL, &hfiles.htask, APP_CPU_NUM);
}

/// @brief 发送文件操作消息
/// @param msg 指向 FilesMsg 结构体的指针，包含文件操作的详细信息
/// @return pdTRUE 发送成功，pdFALSE 超时或失败
BaseType_t sendFileMsg(FilesMsg *msg) {
    if (msg == NULL) return pdFALSE;
    return xQueueSend(hfiles.req_queue, msg, pdMS_TO_TICKS(1000));
}
