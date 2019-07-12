#pragma once

#include "Arduino.h"

String string_test(String &a);
String newline(String &a);
int throw_test();
float float_test(float);

bool lock(int timeout);
bool unlock();
int lock_test(int timeout);