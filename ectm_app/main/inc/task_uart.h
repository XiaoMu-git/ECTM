#ifndef _TASK_UART_H_
#define _TASK_UART_H_

#include "task_config.h"

#define UART_BAUD_RATE          115200
#define UART_BUFF_SIZE          1024
#define UART_TX1_PIN            GPIO_NUM_16
#define UART_RX1_PIN            GPIO_NUM_17
#define UART_TX2_PIN            GPIO_NUM_18
#define UART_RX2_PIN            GPIO_NUM_19

void initUartPeripheral(void);

#endif // !_TASK_UART_H_
