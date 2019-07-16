#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// ESP idf config
// https://github.com/espressif/esp-idf/blob/master/components/freertos/include/freertos/FreeRTOSConfig.h
// https://www.freertos.org/a00110.html

// platformio esp-idf
// /Users/boser/.platformio/packages/framework-espidf/components/app_update

// #define ENABLE_ARDUINO_DEPENDS 1
// #define CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE 1

// freertos
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configUSE_RECURSIVE_MUTEXES 1
#define configUSE_TRACE_FACILITY 1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

// esp idf
#define ESP_CONNECT_LOGGING_LEVEL ESP_LOG_DEBUG

// prefixes of messages sent to host
#define RPC_PROTOCOL 'A'
#define RPC_RESPONSE 'B'
#define RPC_EXCEPTION 'C'
#define RPC_ASYNC_RESPONSE 'D'
#define RPC_ASYNC_EXCEPTION 'E'
#define RPC_LOG_MESSAGE 'F'
#define RPC_BOOT 'G'
#define RPC_ERROR 'H'

// uart configuration (communication with host)
#define UART_NO UART_NUM_1
#define UART_BAUDRATE 9600
#define UART_RX_BUFFER_SIZE (2*1024)
#define UART_TX_BUFFER_SIZE (4*1024)

#define UART_TIMEOUT_MS 50000
#define UART_TX  GPIO_NUM_17
#define UART_RX  GPIO_NUM_16
#define UART_RTS GPIO_NUM_18
#define UART_CTS GPIO_NUM_19

#endif
