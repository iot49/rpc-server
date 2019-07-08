#include "sdkconfig.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "custom_logger.h"
#include "blink_task.h"
#include "uart.h"
#include "simpleRPC.h"

static const char *TAG = "rpc-server";

Uart Serial(UART_NUM_1, 1000);


extern "C" void app_main()
{
    esp_log_set_vprintf(_log_vprintf);
    esp_log_level_set("*", ESP_LOG_DEBUG);
    
    ESP_LOGD(TAG, "server started");

    // start blink task (blinks LED on Huzzah32)
    blink();

}
