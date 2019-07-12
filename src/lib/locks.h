#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

/* Usage:
   NOTE: Clever, but unfortunately crashes the ESP32!

        {
            Lock tx("tx", tx_lock);
            // ... access resource (e.g. UART transmitter)

            // resource is automatically released when tx
            // goes out of scope at end of block
        }
 */

// UART transmitter to host
extern SemaphoreHandle_t tx_lock;
extern TickType_t tx_timeout;

class Lock
{
    SemaphoreHandle_t &m_;

public:
    Lock(const char *lock_name, SemaphoreHandle_t m, int timeout_ms=5000);
    ~Lock();
};
