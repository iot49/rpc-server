#include "settings.h"
#include "rpc_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#include "error.h"
#include "locks.h"
#include "simpleRPC.h"
#include "network.h"
#include "rpc_misc.h"
#include "test_code.h"

static const char *TAG = "rpc";

Network network;

void rpc_task(void *pvParameter)
{
    ESP_LOGI(TAG, "rpc_task started ...");
    for (;;)
    {
        try
        {
            
            
            
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        {
            interface(
                // test
                int_test, F("int_test"),
                float_test, F("float_test"),
                string_test, F("string_test"),
                vec_test, F("vec_test"),
                vec_test2, F("vec_test2"),
                tup_test, F("tup_test"),


                tup_test2, F("tup_test2"),  // crashes (heap corrupt)
                throw_test, F("throw_test"),
                delay_ms, F("delay_ms"),
                // wifi
                pack(&network, &Network::connect), F("wifi_connect"),
                pack(&network, &Network::is_connected), F("wifi_connected"),
                pack(&network, &Network::ip_address), F("wifi_ip"),
                pack(&network, &Network::status), F("wifi_status"),
                pack(&network, &Network::epoch), F("epoch"),
                pack(&network, &Network::mdns), F("mdns"),
                pack(&network, &Network::mdns_service), F("mdns_service"),
                pack(&network, &Network::ota), F("ota"),
                pack(&network, &Network::ota_invalid), F("ota_invalid"),
                // utilities
                reset_reason, F("reset_reason"),
                reset, F("reset"),
                task_list, F("task_list"),
                setLogLevel, F("log_level"),
                getHeapSize, F("esp_heap_size"));
        }
        catch (const char *error)
        {
            // ESP_LOGE(TAG, "Error in interface: %s [%d]", error, strlen(error));
            {
                // we deliberately do not check result
                Lock lock("uart", uart.lock());
                uart.write(RPC_EXCEPTION);
                uart.write(error);
                uart.write('\n');
            }
        }
        catch (...)
        {
            ESP_LOGE(TAG, "Unhandled Exception in rpc_task");
        }
    }
}
