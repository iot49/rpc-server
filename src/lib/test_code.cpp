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

Vector<uint8_t> vec_test1(Vector<uint8_t> &v)
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

int vec_test3(Vector<uint8_t> &v, int flags)
{
    return flags + 1;
}

int vec_test4(int flags, Vector<uint8_t> &v)
{
    return flags + 1;
}

Object<unsigned char, int, bool> tup_test(Object<bool, int, unsigned char> &t)
{
    Object<unsigned char, int, bool> res = {get<2>(t), get<1>(t), get<0>(t)};
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

Object<unsigned char, Object<int, unsigned char>> object_test(Object<unsigned char, Object<int, unsigned char>> &o)
{
    Object<unsigned char, Object<int, unsigned char>> r;

    get<0>(r) = get<0>(o) + 1;
    get<0>(get<1>(r)) = get<0>(get<1>(o)) + 1;
    get<1>(get<1>(r)) = get<1>(get<1>(o)) + 1;

    return r;
}

Object<String, int, Object<String, String>> object_test2(String a, String b)
{
    Object<String, int, Object<String, String>> r;

    get<0>(r) = String("abc.com");
    get<1>(r) = 55;
    get<0>(get<2>(r)) = a;
    get<1>(get<2>(r)) = String("port 80");

    return r;
}

Object<String, String, int> object_test3(Object<String, String> &o)
{
    Object<String, String, int> r;

    get<0>(r) = String("abc");
    get<1>(r) = String("def");
    get<2>(r) = 1234;

    return r;
}
