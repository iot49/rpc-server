#include "rpc_task.h"
#include "settings.h"
#include "locks.h"
#include "network.h"
#include "sock.h"
#include "init_rpc.h"
#include "rpc_misc.h"
#include "test_code.h"
#include "simpleRPC.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

class RPC;

// the statics are shared between all threads
static const char *TAG = "rpc-task";
static const SemaphoreHandle_t rpc_lock = xSemaphoreCreateRecursiveMutex();
static Network network;

// per thread pointer to RPC instance
__thread RPC *rpc;

int handler_id()
{
    return rpc->id();
}

int rpc_address()
{
    return (int)rpc;
}

void RPC::simpleRPC()
{
    interface(
        // test
        int_test, F("int_test"),
        float_test, F("float_test"),
        string_test, F("string_test"),
        vec_test1, F("vec_test1"),
        vec_test2, F("vec_test2"),
        vec_test3, F("vec_test3"),
        vec_test4, F("vec_test4"),
        tup_test, F("tup_test"),
        tup_test2, F("tup_test2"), // crashes (heap corrupt)
        object_test, F("object_test"),
        object_test2, F("object_test2"),
        object_test3, F("object_test3"),
        throw_test, F("throw_test"),
        delay_ms, F("delay_ms"),
        handler_id, F("handler_id"),
        rpc_address, F("rpc_address"),
        // wifi
        pack(&network, &Network::connect), F("wifi_connect"),
        pack(&network, &Network::is_connected), F("wifi_connected"),
        pack(&network, &Network::ip_address), F("wifi_ip"),
        pack(&network, &Network::mac_address), F("wifi_mac"),
        pack(&network, &Network::status), F("wifi_status"),
        pack(&network, &Network::epoch), F("epoch"),
        pack(&network, &Network::mdns), F("mdns"),
        pack(&network, &Network::mdns_service), F("mdns_service"),
        pack(&network, &Network::ota), F("ota"),
        pack(&network, &Network::ota_invalid), F("ota_invalid"),
        // sockets
        socket_getaddrinfo, F("sock_getaddrinfo"),
        socket_socket, F("sock_socket"),
        socket_setsockopt, F("sock_setsockopt"),
        socket_bind, F("sock_bind"),
        sock_listen, F("sock_listen"),
        socket_accept, F("sock_accept"),
        socket_connect, F("sock_connect"),
        sock_send, F("sock_send"),
        sock_sendall, F("sock_sendall"),
        sock_sendto, F("sock_sendto"),
        sock_recv, F("sock_recv"),
        sock_recvfrom, F("sock_recvfrom"),
        socket_close, F("sock_close"),
        // utilities
        reset_reason, F("reset_reason"),
        reset, F("reset"),
        task_list, F("task_list"),
        setLogLevel, F("log_level"),
        xPortGetFreeHeapSize, F("xPortGetFreeHeapSize"),
        heap_caps_check_integrity_all, F("heap_caps_check_integrity_all"),
        min_free_heap_size, F("min_heap_size"),
        heap_size, F("heap_size"));
}

void RPC::lock()
{
    while (xSemaphoreTakeRecursive(rpc_lock, pdMS_TO_TICKS(100000)) != pdTRUE)
        ESP_LOGD(TAG, "task %d waiting for lock", id());
    ESP_LOGD(TAG, "task %d acquired lock", id());
    has_lock = true;
}

void RPC::unlock()
{
    if (has_lock)
    {
        ESP_LOGD(TAG, "task %d releasing lock", id());
        xSemaphoreGiveRecursive(rpc_lock);
        has_lock = false;
    }
}

void RPC::run_forever()
{
    for (;;)
    {
        lock();
        try
        {
            is_async = false;
            aid_ = 0;
            simpleRPC();
        }
        catch (const char *error)
        {
            // ESP_LOGE(TAG, "Error in interface: %s [%d]", error, strlen(error));
            {
                // we deliberately do not check result
                Lock lock("uart", uart.lock());
                if (is_async_call())
                {
                    uart.write(RPC_ASYNC_EXCEPTION);
                    uart.write((const uint8_t *)&aid_, 4);
                }
                else
                {
                    uart.write(RPC_EXCEPTION);
                }
                uart.write(error);
                uart.write('\n');
            }
        }
        catch (...)
        {
            ESP_LOGE(TAG, "Unhandled Exception in rpc_task");
        }
        // synchronous rpc calls & exceptions release lock here
        // async releases in simpleRPC code 
        unlock();
    }
}

void rpc_task(void *pvParameter)
{
    ESP_LOGI(TAG, "task %2d started ...", *(int *)pvParameter);
    rpc = new RPC(*(int*)pvParameter);
    rpc->run_forever();
}
