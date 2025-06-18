#include "task_uart.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"

/// @brief uart 外设初始化
/// @param  
void initUartPeripheral(void) {
    const char *TAG = "uartInit";

    // 配置 UART 参数
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_param_config(UART_NUM_2, &uart_config);
    ESP_LOGI(TAG, "Uart parameter configuration completed.");

    // 配置 UART 引脚
    uart_set_pin(UART_NUM_1, UART_TX1_PIN, UART_RX1_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_pin(UART_NUM_2, UART_TX2_PIN, UART_RX2_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 安装 UART 驱动
    uart_driver_install(UART_NUM_1, UART_BUFF_SIZE, 0, 0, NULL, 0);
    uart_driver_install(UART_NUM_2, UART_BUFF_SIZE, 0, 0, NULL, 0);
    ESP_LOGI(TAG, "Uart parameter initialization completed.");
}
