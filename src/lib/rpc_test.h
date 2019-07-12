#pragma once

#include "Arduino.h"
#include "vector.h"

String string_test(String &a);
String newline(String &a);
int throw_test();
float float_test(float);

bool lock(int timeout);
bool unlock();
int lock_test(int timeout);

void uart_init(int baudrate, int timeout, size_t n_rx, size_t n_tx, uint8_t threshold) ;
int uart_in_waiting();
Vector<uint8_t> read(int sz);
int write(Vector<uint8_t> &data);