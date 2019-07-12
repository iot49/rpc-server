#include "sdkconfig.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "nvs_flash.h"

#include "message_types.h"
#include "lib/locks.h"
#include "lib/error.h"
#include "lib/rpc_task.h"
#include "lib/blink_task.h"


static const char *TAG = "main";


// #define TX  GPIO_NUM_17
// #define RX  GPIO_NUM_16
#define TX  GPIO_NUM_1
#define RX  GPIO_NUM_3
#define RTS GPIO_NUM_18
#define CTS GPIO_NUM_19

// log messages: acquire tx lock and send type 
int _log_vprintf(const char *fmt, va_list args)
{
    //Lock tx("tx", tx_lock);
    int res = 0;
    xSemaphoreTake(tx_lock, tx_timeout);
    putc(LOG_MESSAGE, stdout);
    res = vprintf(fmt, args);
    xSemaphoreGive(tx_lock);
    return res;
}

// https://github.com/platformio/platform-espressif32/pull/193

extern "C" void app_main()
{
    // tag all log messages & set logging level
    esp_log_set_vprintf(_log_vprintf);
    esp_log_level_set("*", ESP_LOG_DEBUG);

    // configure uart for hardware flow control & change baudrate
    vTaskDelay(100 / portTICK_PERIOD_MS);   // needed ... for whatever reason
    uart_set_hw_flow_ctrl(UART_NUM_0, UART_HW_FLOWCTRL_CTS_RTS, 122);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, RTS, CTS);
    uart_set_baudrate(UART_NUM_0, 921600);

    // sync message (for RPC-Client)
    printf("\n\nGRPC-Server V1\n");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGI(TAG, "nvs_flash_erase");
        err_check_log(TAG, nvs_flash_erase());
        ESP_LOGI(TAG, "nvs_flash_init");
        err_check_log(TAG, nvs_flash_init());
    }

    ESP_LOGI(TAG, "rpc server started");

    // start blink task (blinks LED on Huzzah32)
    xTaskCreate(&blink_task, "blink_task", 4096, NULL, 5, NULL);
    xTaskCreate(&rpc_task, "rpc_task", 4*4096, NULL, 5, NULL);

}
