#pragma once

#include <stdint.h>
#include <string.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <Wstring.h>

typedef uint8_t byte;


class Uart
{
    public:
        Uart(uart_port_t uart_num, int timeout_ms) : port{uart_num}, timeout_ms{timeout_ms} {};
        void init(int baud_rate, size_t tx_buffer_size, size_t rx_buffer_size, gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts);

        size_t in_waiting();

        int read(uint8_t *buf, size_t len);
        uint8_t read();

        int write(const char *data, size_t len);
        int write(char byte);

        // arduino compatibility
        size_t available() { return in_waiting(); }
        int write(const char *data) { return write(data, strlen(data)); }
        int write(byte *data, unsigned int len) { return write((const char*)data, len); }
        size_t print(char c) { return write(&c, 1); }
        size_t readBytes(char *buf, unsigned int len);
        String readStringUntil(char terminator);
    protected:
        uart_port_t port;
        int timeout_ms;
};
