#pragma once

#include "settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

/********************************************************************************
 *** CRASHES ESP32! ***
 ********************************************************************************/

/* Usage:
        {
            Lock tx("tx", uart.tx_lock());
            // ... access resource (e.g. UART transmitter)

            // resource is automatically released when tx
            // goes out of scope at end of block
        }
 */

class Lock
{
    SemaphoreHandle_t &m_;

public:
    Lock(const char *lock_name, SemaphoreHandle_t &m, int timeout_ms = 5000);
    ~Lock();
};
