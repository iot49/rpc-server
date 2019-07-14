#pragma once

#define ESP_CONNECT_LOGGING_LEVEL ESP_LOG_DEBUG

// prefixes of messages sent to host
const uint8_t RPC_PROTOCOL = 'A';
const uint8_t RPC_RESPONSE = 'B';
const uint8_t RPC_EXCEPTION = 'C';
const uint8_t RPC_ASYNC_RESPONSE = 'D';
const uint8_t RPC_ASYNC_EXCEPTION = 'E';
const uint8_t RPC_LOG_MESSAGE = 'F';
const uint8_t RPC_BOOT = 'G';
const uint8_t RPC_ERROR = 'H';

// uart configuration (communication with host)
#define UART_NO UART_NUM_1
#define UART_BAUDRATE (9600)
#define UART_RX_BUFFER_SIZE 1024
#define UART_TX_BUFFER_SIZE  512

#define UART_TIMEOUT 1000
#define UART_TX  GPIO_NUM_17
#define UART_RX  GPIO_NUM_16
#define UART_RTS GPIO_NUM_18
#define UART_CTS GPIO_NUM_19
