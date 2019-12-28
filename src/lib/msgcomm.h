#pragma once

#include "settings.h"
#include "ringbuf.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <driver/uart.h>
#include <string>

class MsgComm
{
public:
    MsgComm(uart_port_t uart_num = UART_NO, int timeout_ms = UART_TIMEOUT_MS);

    void init(int baud_rate, size_t rx_buffer_size, size_t tx_buffer_size,
              gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts);

    inline SemaphoreHandle_t& lock() { return tx_lock; }
    inline bool locked() { return uxSemaphoreGetCount(tx_lock) == 0; }

    // number of messages in rx buffer
    size_t messages_waiting() { 
        check_rx();
        return msg_waiting; 
    }

    // read from rx buffer into buf return actual number of bytes read
    size_t read(void *buf, size_t len);

    // read past next EOT; return number of bytes read, not counting EOT
    size_t read_eot();

    // write data; return number of bytes written
    // blocking!
    size_t write(const uint8_t *data, size_t len);

    // write un-escaped EOT to tx buffer
    // blocking!
    void write_eot();

protected :
    uart_port_t port;
    TickType_t timeout_ticks;
    SemaphoreHandle_t tx_lock = xSemaphoreCreateRecursiveMutex();
    Ringbuf<uint8_t> *rx_buffer = nullptr;
    int msg_waiting = 0;

    uint8_t getc_() {
        return rx_buffer->get();
    }

    // send to c to TX, returns success
    // blocking!
    void putc_(uint8_t c) {
        uart_write_bytes(port, (const char *)&c, (size_t)1);
    }

    void check_rx();
};
