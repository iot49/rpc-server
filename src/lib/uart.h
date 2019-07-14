#pragma once

#include <stdint.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include "freertos/semphr.h"
#include "locks.h"

// #define TX GPIO_NUM_1
// #define RX GPIO_NUM_3
#define TX  GPIO_NUM_17
#define RX  GPIO_NUM_16
#define RTS GPIO_NUM_18
#define CTS GPIO_NUM_19

class Uart
{
public:
    Uart(uart_port_t uart_num, int timeout_ms) : port{uart_num}, timeout_ms{timeout_ms} {};

    // rx_buffer_size > 128!
    void init(int baud_rate, size_t rx_buffer_size = 1024, size_t tx_buffer_size = 0,
              gpio_num_t tx = TX, gpio_num_t rx = RX, gpio_num_t rts = RTS, gpio_num_t cts = CTS);

    inline bool lock() { return xSemaphoreTake(tx_lock, (TickType_t)5000); }
    inline bool unlock() { return xSemaphoreGive(tx_lock); }
    inline bool locked() { return uxSemaphoreGetCount(tx_lock) == 0; }

    size_t in_waiting();
    int events_waiting();
    int next_event();

    int read(uint8_t *buf, size_t len);
    uint8_t read();

    int write(const uint8_t *data, size_t len);
    int write(uint8_t byte);
    int write(const char *string);

protected:
    uart_port_t port;
    int timeout_ms;
    QueueHandle_t uart_queue;
    SemaphoreHandle_t tx_lock = xSemaphoreCreateMutex();
};
