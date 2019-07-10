#pragma once

#include "Arduino.h"

String string_test1(String &a) { return a; }
String string_test2(String a) { return a; }
int throw_test() { throw "throw test!"; }
