#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <Wstring.h>
#include "lib/uart.h"


extern Uart uart;


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
        return uart.read();
    }

    inline size_t write(const uint8_t *buffer, size_t size)
    {
        return uart.write(buffer, size);
    }

    inline size_t write(const char *s)
    {
        return uart.write(s);
    }

    inline size_t write(uint8_t c)
    {
        return uart.write(c);
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
