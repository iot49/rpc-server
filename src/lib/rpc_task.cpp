#include "rpc_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "error.h"
#include "simpleRPC.h"
#include "network.h"
#include "rpc_misc.h"
#include "rpc_test.h"

static const char *TAG = "rpc";

Network network;

void rpc_task(void *pvParameter)
{
    for (;;)
    {
        ESP_LOGV(TAG, "interface ...");
        try
        {
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
        catch (const char *error)
        {
            ESP_LOGE(TAG, "Error in interface: %s [%d]", error, strlen(error));
            putc(RPC_EXCEPTION, stdout);
            printf(error);
            putc('\n', stdout);
        }
        catch (...)
        {
            ESP_LOGE(TAG, "Unknown error type in interface");
        }

        // feed the wdt?
        vTaskDelay(10);
    }
}