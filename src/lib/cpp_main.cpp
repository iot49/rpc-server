#include "settings.h"
#include "rpcserver.h"
#include "msgpack.h"
#include "locks.h"
#include "error.h"
#include <string.h>

static const char *TAG = "cpp-main";

// global (accessible from simpleRPC)
RPCServer server = RPCServer();


// log messages: acquire tx lock
int ___log_vprintf(const char *fmt, va_list args) {
    const int N = 128;
    char buffer[N];
    vsnprintf(buffer, N, fmt, args);
    // not sure why this would be required, but it is?
    buffer[N-1] = 0;
    server.send_log_message(buffer);
    return strlen(buffer);
}

void cpp_main() {

    ESP_LOGI(TAG, "cpp_main --");
    server.init(UART_BAUDRATE,
        UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE,
        UART_TX, UART_RX, UART_RTS, UART_CTS);

    // tag log messages and route to host
    esp_log_set_vprintf(___log_vprintf);

    // logging level
    esp_log_level_set("*", ESP_LOG_DEBUG);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // TODO:
    // server.serve(
    //    "ping", ping,
    //    "version", version
    // );

    // test code; replace by server.serve (see above)
    ESP_LOGI(TAG, "LOGI ---");
    ESP_LOGD(TAG, "LOGD ---");
    ESP_LOGE(TAG, "LOGE ---");

    while (true) {
        try {
            std::vector<float> vf = {4.6, 7};
            std::vector<int> vi = {};
            std::vector<std::string> vs = {"a", "\x01\x04\x1b"};
            std::map<std::string, int> m{{"key1", 1}, {"key2", 2}};
            auto t = std::make_tuple(1, 3.8, "a string", false);
            bytes b = {0, 1, 2, 3, 4, 5, 6, 6, 7, 9, 10};

            server.send_response("a response string");
            ESP_LOGI(TAG, "map ...");
            server.send_response_map("vf", vf, "vi", vi, "vs", vs, "m", m, "b", b, "t", t);
            
            ESP_LOGI(TAG, "tuple ...");
            server.send_response(t);
            
            ESP_LOGI(TAG, "array ...");
            server.send_response_array("pack_array", 1, false, vf, vi, vs, m, b, t);

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        catch (const char *error)
        {
            server.send_log_message("string exception in cpp_main");
            server.send_log_message(error);
        }
        catch (...)
        {
            server.send_log_message("unhandled exception");
        }
    }
}
