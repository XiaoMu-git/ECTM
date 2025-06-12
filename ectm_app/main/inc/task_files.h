#ifndef _TASK_FILES_H_
#define _TASK_FILES_H_

#include "task_config.h"

#define PATH_MAX_LEN        32      // 文件路径最大长度
#define KEY_MAX_LAYER       4       // 最多支持 4 层嵌套 JSON 键
#define KEY_NAME_MAX_LEN    16      // 单个键名最大长度
#define VALUE_MAX_LEN       64      // 值最大长度

typedef enum {
    FILES_MODE_READ = 0,    // 读取模式
    FILES_MODE_WRITE        // 写入模式
} FilesMode;

typedef enum {
    FILES_TYPE_ARRAY = 0,   // 数组类型
    FILES_TYPE_OBJECT       // 对象类型
} FilesType;

typedef struct {
    TaskHandle_t htask;         // 任务句柄
    QueueHandle_t req_queue;    // 任务请求队列
} FilesHandle;

typedef struct {
    char path[PATH_MAX_LEN];                        // 文件路径，例如 "/spiffs/config.json"
    char keys[KEY_MAX_LAYER][KEY_NAME_MAX_LEN];     // 多层 JSON 键路径
    uint8_t key_layer;                              // 实际使用了多少层键
    char value[VALUE_MAX_LEN];                      // 要写入或读取返回的值
    uint8_t type;                                   // 区分对象和数组
    FilesMode mode;                                 // 操作模式：读或写
    QueueHandle_t resp_queue;                       // 操作完成后返回结果的队列
    uint8_t resualt;                                // 操作结果：1 成功，0 失败
} FilesMsg;

void createFilesTask(void);
BaseType_t sendFileMsg(FilesMsg *msg);

#endif // !_TASK_FILES_H_
