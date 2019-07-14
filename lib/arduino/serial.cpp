#include <serial.h>


size_t in_waiting();
int events_waiting();
int next_event();

int read(uint8_t *buf, size_t len);
uint8_t read();

size_t HardwareSerial::available(void)
{
    return uart.in_waiting();
}


size_t HardwareSerial::readBytes(char *buffer, size_t length)
{
    return uart.read((uint8_t*)buffer, length);
}


String HardwareSerial::readStringUntil(char terminator)
{
    String ret;
    int c = read();
    while (c >= 0 && c != terminator) {
        ret += (char)c;
        c = read();
    }
    return ret;
}


size_t HardwareSerial::print(const __FlashStringHelper *ifsh)
{
    return print(reinterpret_cast<const char *>(ifsh));
}


size_t HardwareSerial::print(const char *str)
{
    return write((uint8_t*)str, strlen(str));
}


HardwareSerial Serial;
