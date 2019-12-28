#include "cpp_main.h"
#include "settings.h"
#include "rpc.h"
#include "rpc_os.h"
#include "rpc_test.h"
#include "error.h"
#include <string.h>

static const char *TAG = "cpp-main";

// log messages: acquire tx lock
int ___log_vprintf(const char *fmt, va_list args) {
    const int N = 128;
    char buffer[N];
    vsnprintf(buffer, N, fmt, args);
    // not sure why this would be required, but it is?
    buffer[N-1] = 0;
    rpc.send_log_message(buffer);
    return strlen(buffer);
}

void cpp_main() {
    ESP_LOGI(TAG, "--------------------- cpp_main ----------------------");
    rpc.init(UART_BAUDRATE,
             UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE,
             UART_TX, UART_RX, UART_RTS, UART_CTS);

    // tag log messages and route to host
    esp_log_set_vprintf(___log_vprintf);

    // logging level
    esp_log_level_set("*", ESP_LOG_DEBUG);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "rpc server started");

    while (true) {
        try {
            rpc.run(
                // os
                version, "version",
                reset_uart, "reset_uart",
                // test
                test, "test",
                echo_int, "echo_int",
                echo_vec, "echo_vec",
                echo_str, "echo_str",
                echo_bool, "echo_bool",
                echo_bytes, "echo_bytes",
                echo_v_uint8, "echo_v_uint8"
            );
        }
        catch (RPCException re) {
            ESP_LOGE(TAG, "RPCException %s", re.what());
        }
        catch (...) {
            ESP_LOGE(TAG, "unhandled exception");
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
