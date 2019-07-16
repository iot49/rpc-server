#pragma once

#include "Arduino.h"
#include "vector.h"
#include "tuple.h"

String string_test(String &a);
float float_test(float);
int int_test(int);
int throw_test();
Vector<uint8_t> vec_test(Vector<uint8_t> &);
Vector<uint8_t> vec_test2(int sz);
Object<float, int, bool> tup_test(Object<bool, int, float> &);
Object<Vector<uint8_t>, Vector<uint8_t>> tup_test2(int sz);
