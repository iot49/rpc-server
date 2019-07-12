#include "rpc_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#include "error.h"
#include "simpleRPC.h"
#include "network.h"
#include "rpc_misc.h"
#include "rpc_test.h"

static const char *TAG = "rpc";

Network network;

int inc(int i) {
    return i+1;
}

void nothing() {

}

void rpc_task(void *pvParameter)
{
    // TODO: just spins, sucking up cpu cycles.
    // FIX:  change simpleRPC.h to block on read().
    //       FOR that, read() must block!
    //       Probably requies UART driver with interrupts.
    //       presently read returns immediately -1 (0xff) when no data is available
    //       0xff happens to be the "list request" char in simpleRPC.

    // https: //github.com/espressif/esp-idf/blob/master/components/vfs/README.rst
    _GLOBAL_REENT->_stdout = fopen("/dev/uart/0", "wb");
    _GLOBAL_REENT->_stderr = _GLOBAL_REENT->_stderr;

    for (;;)
    {
        try
        {
            interface(
                // test
                float_test, F("float_test"),
                newline, F("newline"),
                inc, F("inc"),
                nothing, F("nothing"),
                string_test, F("string_test"),
                throw_test, F("throw_test"),
                unlock, F("unlock"),
                lock, F("lock"),
                lock_test, F("lock_test"),
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
            // TODO: Acquire LOCK!
            putc(RPC_EXCEPTION, stdout);
            printf(error);
            putc('\n', stdout);
        }
        catch (...)
        {
            ESP_LOGE(TAG, "Unhandled Exception in rpc_task");
        }
        // without watchdog errors from IDLE1 task
        vTaskDelay(1);
    }
}
