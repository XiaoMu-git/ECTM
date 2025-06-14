#ifndef _TASK_FILES_H_
#define _TASK_FILES_H_

#include "task_config.h"

#define FILES_PATH_SIZE     32          // 文件路径最大长度
#define FILES_KEYS_SIZE     64          // 最多支持 4 层嵌套 JSON 键
#define FILES_VALUE_SIZE    32          // 值最大长度

typedef enum {
    FILES_MODE_READ = 0,                // 读取模式
    FILES_MODE_WRITE                    // 写入模式
} FILES_MODE;

typedef struct {
    TaskHandle_t htask;                 // 任务句柄
    QueueHandle_t req_queue;            // 任务请求队列
} FilesHandle;

typedef struct {
    char path[FILES_PATH_SIZE];         // 文件路径，例如 "/spiffs/config.json"
    char keys[FILES_KEYS_SIZE];         // 多层 JSON 键路径
    char value[FILES_VALUE_SIZE];       // 要写入或读取返回的值
    uint8_t mode;                       // 操作模式：读或写
    QueueHandle_t resp_queue;           // 操作完成后返回结果的队列
    bool resualt;                       // 操作结果
} FilesMsg;

void createFilesTask(void);
bool filesReadParam(char *path, char *keys, char *value);
bool filesWriteParam(char *path, char *keys, char *value);

#endif // !_TASK_FILES_H_
