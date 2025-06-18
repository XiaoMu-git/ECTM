#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

// 栈大小等级（单位：字节）
#define TASK_STACK_TINY         ((uint16_t)2048)
#define TASK_STACK_SMALL        ((uint16_t)4096)
#define TASK_STACK_MEDIUM       ((uint16_t)6144)
#define TASK_STACK_LARGE        ((uint16_t)8192)

// 优先级等级
#define TASK_PRIO_LOW           ((uint8_t)5)
#define TASK_PRIO_NORMAL        ((uint8_t)10)
#define TASK_PRIO_HIGH          ((uint8_t)15)
#define TASK_PRIO_CRITICAL      ((uint8_t)20)

// 消息队列长度
#define QUEUE_SIZE_SMALL        ((uint8_t)4)
#define QUEUE_SIZE_MEDIUM       ((uint8_t)8)
#define QUEUE_SIZE_LARGE        ((uint8_t)16)

// 等待时间等级（单位：毫秒）
#define TIME_WAIT_SHORT         ((TickType_t)pdMS_TO_TICKS(100))
#define TIME_WAIT_MEDIUM        ((TickType_t)pdMS_TO_TICKS(500))
#define TIME_WAIT_LONG          ((TickType_t)pdMS_TO_TICKS(2500))

#endif // !_TASK_CONFIG_H_
