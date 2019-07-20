#pragma once

#include "Arduino.h"
#include "vector.h"
#include "tuple.h"

String string_test(String &a);
float float_test(float);
int int_test(int);
int throw_test();
Vector<uint8_t> vec_test1(Vector<uint8_t> &);
Vector<uint8_t> vec_test2(int sz);
int vec_test3(Vector<uint8_t> &, int flags);
int vec_test4(int flags, Vector<uint8_t> &);
Object<unsigned char, int, bool> tup_test(Object<bool, int, unsigned char> &);
Object<Vector<uint8_t>, Vector<uint8_t>> tup_test2(int sz);
Object<unsigned char, Object<int, unsigned char>> object_test(Object<unsigned char, Object<int, unsigned char>> &o);

Object<String, int, Object<String, String>> object_test2(String a, String b);
Object<String, String, int> object_test3(Object<String, String> &o);
