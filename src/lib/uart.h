#pragma once

#include "settings.h"
#include "locks.h"
#include <stdint.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// #define TX GPIO_NUM_1
// #define RX GPIO_NUM_3
#define TX  GPIO_NUM_17
#define RX  GPIO_NUM_16
#define RTS GPIO_NUM_18
#define CTS GPIO_NUM_19

class Uart
{
public:
    Uart(uart_port_t uart_num, int timeout_ms) : port{uart_num}
    {
        timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    }

    // _x_buffer_size > 128!
    void init(int baud_rate, size_t rx_buffer_size = 1024, size_t tx_buffer_size = 1024,
              gpio_num_t tx = TX, gpio_num_t rx = RX, gpio_num_t rts = RTS, gpio_num_t cts = CTS);

    inline SemaphoreHandle_t& lock() { return tx_lock; }
    inline bool locked() { return uxSemaphoreGetCount(tx_lock) == 0; }

    size_t in_waiting();

    int read(uint8_t *buf, size_t len);           // number of chars read
    int read();                                   // char or -1 if timeout

    int write(const uint8_t *data, size_t len);   // bytes written or -1 if timeout
    int write(uint8_t byte);
    int write(const char *string);

protected : 
    uart_port_t port;
    TickType_t timeout_ticks;
    SemaphoreHandle_t tx_lock = xSemaphoreCreateRecursiveMutex();
};
