#include "sdkconfig.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "nvs_flash.h"

#include "message_types.h"
#include "util.h"
#include "lib/error.h"
#include "lib/network.h"
#include "lib/blink_task.h"
#include "simpleRPC.h"

static const char *TAG = "rpc-server";

Network network;

// #define TX  GPIO_NUM_17
// #define RX  GPIO_NUM_16
#define TX  GPIO_NUM_1
#define RX  GPIO_NUM_3
#define RTS GPIO_NUM_18
#define CTS GPIO_NUM_19

// mark log messages
int _log_vprintf(const char *fmt, va_list args)
{
    putc(LOG_MESSAGE, stdout);
    return vprintf(fmt, args);
}

String string_test1(String &a) { return a; }
String string_test2(String a) { return a; }
int throw_test() { throw "throw test!"; }

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
    printf("\n\nRPC-Server V1\n");

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
    blink();
    ESP_LOGD(TAG, "return from blink task");

    for (;;) {
        ESP_LOGV(TAG, "interface ...");
        try {
            interface(
                // test
                //string_test1, F("string_test1"),
                //string_test2, F("string_test2"),
                //throw_test, F("throw_test"),
                // wifi
                pack(&network, &Network::connect), F("wifi_connect"),
                pack(&network, &Network::is_connected), F("wifi_connected"),
                pack(&network, &Network::ip_address), F("wifi_ip"),
                pack(&network, &Network::status), F("wifi_status"),
                pack(&network, &Network::epoch), F("epoch"),
                // utilities
                setLogLevel, F("log_level"),
                getHeapSize, F("esp_heap_size"));
        }
        catch (const char *error) {
            ESP_LOGE(TAG, "Error in interface: %s [%d]", error, strlen(error));
            putc(RPC_EXCEPTION, stdout);
            printf(error);
            putc('\n', stdout);
        }
        catch (...) {
            ESP_LOGE(TAG, "Unknown error type in interface");
        }

        // feed the wdt?
        vTaskDelay(10);
    }

}
