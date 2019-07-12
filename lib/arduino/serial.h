#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <Wstring.h>
#include <driver/uart.h>


class HardwareSerial
{
public:

    size_t available(void);
    size_t readBytes(char *buf, unsigned int len);
    String readStringUntil(char terminator);
    size_t print(const __FlashStringHelper *);
    size_t print(const char *);

    inline int read(void)
    {
        return getc(stdin);
    }

    inline size_t write(const uint8_t *buffer, size_t size)
    {
        // https://github.com/espressif/esp-idf/blob/master/components/vfs/README.rst
        //return uart_write_bytes(UART_NUM_0, (const char*)buffer, size);
        return fwrite(buffer, sizeof(uint8_t), size, stdout);
    }

    inline size_t write(const char *s)
    {
        return write((uint8_t *)s, strlen(s));
    }

    inline size_t write(uint8_t c)
    {
        fputc(c, stdout);
        return 1;
    }

    inline size_t write(unsigned long n)
    {
        return write((uint8_t) n);
    }

    inline size_t write(long n)
    {
        return write((uint8_t) n);
    }

    inline size_t write(unsigned int n)
    {
        return write((uint8_t) n);
    }

    inline size_t write(int n)
    {
        return write((uint8_t) n);
    }

    size_t print(char c) 
    { 
        return write(c); 
    }

};
