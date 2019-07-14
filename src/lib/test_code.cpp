#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "test_code.h"

String string_test(String &a)
{
    return a;
}

float float_test(float f) 
{
    return f;
}

int int_test(int i) 
{
    return i;
}

int throw_test()
{
    throw("Exception in throw_test!");
}

Vector<uint8_t> vec_test(Vector<uint8_t> &v)
{
    return v; 
}


