#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "locks.h"


/*******************************************************************************/

/* NEAT ... but crashes ESP32! DISABLED */

Lock::Lock(const char *lock_name, SemaphoreHandle_t m, int timeout_ms) : m_(m)
{
    if (xSemaphoreTake(m_, (TickType_t)timeout_ms) != pdTRUE)
    {
        // ESP_LOGI("lock", "Failed to acquire lock %s", lock_name);
        // TaskHandle_t holder = xSemaphoreGetMutexHolder(m_);
        char buffer[120];
        sprintf(buffer, "Failed to acquire lock %s", lock_name);
        throw(const char *) & buffer;
    }
}


Lock::~Lock()
{
    xSemaphoreGive(m_);
}
