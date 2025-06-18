#ifndef _TASK_TEST_H_
#define _TASK_TEST_H_

#include "task_config.h"

typedef struct {
    TaskHandle_t htask;
} TestHandle;

void createTestTask(void);

#endif // !_TASK_TEST_H_
