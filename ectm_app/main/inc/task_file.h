#ifndef _TASK_FILE_H_
#define _TASK_FILE_H_

#include "task_config.h"
#include "cJSON.h"

#define FILE_KEY_SIZE_MAX       32
#define FILE_KEY_COUNT_MAX      16
#define FILE_MAX_SIZE           1024

typedef struct {
    const char *path;
    SemaphoreHandle_t mutex;
    const char *data_defualt;
} FileHandle;

extern FileHandle hfiles[];

void initSpiffsPeripheral(void);
size_t readFile(FileHandle *hfile, char *buff, uint32_t length);
size_t writeFile(FileHandle *hfile, char *data, uint32_t length);
cJSON* getJsonItem(cJSON *root, const char *keys);

#endif // !_TASK_FILE_H_
