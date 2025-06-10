#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TASK_STACK_SMALL    (uint16_t)(1024 / 4)
#define TASK_STACK_MEDIUM   (uint16_t)(2048 / 4)
#define TASK_STACK_LARGE    (uint16_t)(4096 / 4)

#define TASK_PRIO_LOW       (uint8_t)(5)
#define TASK_PRIO_MEDIUM    (uint8_t)(10)
#define TASK_PRIO_HIGH      (uint8_t)(15)

#endif // !_TASK_CONFIG_H_
