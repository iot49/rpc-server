#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "locks.h"

// UART transmitter to host
SemaphoreHandle_t tx_lock = xSemaphoreCreateMutex();
TickType_t tx_timeout = (TickType_t)10000;


/*******************************************************************************/

/* NEAT ... but crashes ESP32! DISABLED */

Lock::Lock(const char *lock_name, SemaphoreHandle_t m, int timeout_ms) : m_(m)
{
    // DISABLED: crashes ESP32!
    return;
    if (xSemaphoreTake(m_, (TickType_t)timeout_ms) != pdTRUE)
    {
        //ESP_LOGI("lock", "Failed to acquire lock %s", lock_name);
        //char buffer[80];
        //sprintf(buffer, "Failed to acquire lock %s", lock_name);
        //throw(const char *) & buffer;
    }
}


Lock::~Lock()
{
    // DISABLED: crashes ESP32!
    return;
    xSemaphoreGive(m_);
}
