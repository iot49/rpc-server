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

void uart_init(int baudrate, int timeout, size_t rx_buffer_size, size_t tx_buffer_size);
int uart_in_waiting();
Vector<uint8_t> read(int sz);
int write(Vector<uint8_t> &data);

/*
typedef enum {
    UART_DATA,              / * !< UART data event* /
    UART_BREAK,           / * !< UART break event* /
    UART_BUFFER_FULL, / * !< UART RX buffer full event* /
    UART_FIFO_OVF,    / * !< UART FIFO overflow event* /
    UART_FRAME_ERR,   / * !< UART RX frame error event* /
    UART_PARITY_ERR,  / *!< UART RX parity event* /
    UART_DATA_BREAK,  / *!< UART TX data and break event* /
    UART_PATTERN_DET, / *!< UART pattern detected * /
    UART_EVENT_MAX,   / *!< UART event max index* /
}
uart_event_type_t;
*/

int uart_events_waiting();
int uart_next_event();
