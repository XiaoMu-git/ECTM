#ifndef _TASK_TEST_H_
#define _TASK_TEST_H_

#include "task_config.h"

typedef struct TestHandle {
    TaskHandle_t htask;  // 任务句柄
} TestHandle;

void createTestTask(void);

#endif // !_TASK_TEST_H_
