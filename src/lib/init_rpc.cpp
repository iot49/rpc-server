#include "init_rpc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "string.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"

#include "settings.h"
#include "locks.h"
#include "error.h"
#include "uart.h"
#include "rpc_task.h"
#include "blink_task.h"

static const char *TAG = "init_rpc";

// log messages: acquire tx lock and send type
int _log_vprintf(const char *fmt, va_list args)
{
    int result;
    uart.lock();
    {
        uart.write(RPC_LOG_MESSAGE);
        const int N = 100;
        char buffer[N];
        vsnprintf(buffer, N, fmt, args);
        result = uart.write(buffer);
    }
    uart.unlock();
    return result;
}

Uart uart(UART_NO, UART_TIMEOUT);

void init_rpc()
{
    // configure uart
    uart.init(UART_BAUDRATE, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE, UART_TX, UART_RX, UART_RTS, UART_CTS);

    // tag log messages and route to host
    esp_log_set_vprintf(_log_vprintf);

    // logging level
    esp_log_level_set("*", ESP_CONNECT_LOGGING_LEVEL);

    // startup message
    {
        uart.lock();
        uart.write("\n\nGRPC-Server V1\n");
        uart.unlock();
    }

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGI(TAG, "nvs_flash_erase");
        err_check_log(TAG, nvs_flash_erase());
        ESP_LOGI(TAG, "nvs_flash_init");
        err_check_log(TAG, nvs_flash_init());
    }

    // if we made it to here, we assume the ota image is good ...
    ESP_LOGW(TAG, "esp_ota_mark_app_valid_cancel_rollback NOT IMPLEMENTED");
    // err_check_log(TAG, esp_ota_mark_app_valid_cancel_rollback());

    // start blink task (blinks LED on Huzzah32)
    xTaskCreate(&blink_task, "blink_task", 4096, NULL, 5, NULL);
    xTaskCreate(&rpc_task, "rpc_task", 4 * 4096, NULL, 5, NULL);
}
