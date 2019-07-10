#include <serial.h>


size_t HardwareSerial::available(void)
{
    int c = getc(stdin);
    if (c == EOF) {
        return 0;
    } else {
        ungetc(c, stdin);
        return 1;
    }
}


size_t HardwareSerial::readBytes(char *buffer, size_t length)
{
    size_t count = 0;
    while (count < length) {
        int c = read();
        if(c < 0) {
            break;
        }
        *buffer++ = (char)c;
        count++;
    }
    return count;
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
