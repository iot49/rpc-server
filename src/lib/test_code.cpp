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
    Vector<uint8_t> res = Vector<uint8_t>(v.size);
    for (int i=0;  i<v.size;  i++)
        res[v.size-i-1] = v[i];
    return res; 
}

Vector<uint8_t> vec_test2(int sz)
{
    auto res = Vector<uint8_t>(sz);
    for (int i=0;  i<sz;  i++) res[i] = 'A' + i;
    return res;
}

Object<float, int, bool> tup_test(Object<bool, int, float> &t)
{
    Object<float, int, bool> res = {get<2>(t), get<1>(t), get<0>(t)};
    return res;
}

Object<Vector<uint8_t>, Vector<uint8_t>> tup_test2(int sz)
{
    Vector<uint8_t> v1 = Vector<uint8_t>(sz);
    Vector<uint8_t> v2 = Vector<uint8_t>(sz);
    for (int i=0;  i<sz;  i++) {
        v1[i] = v2[i] = i;
    }
    Object<Vector<uint8_t>, Vector<uint8_t>> res = { v1, v2 };
    return res;
}
