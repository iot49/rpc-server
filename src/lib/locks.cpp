#include "settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "locks.h"


Lock::Lock(const char *lock_name, SemaphoreHandle_t &m, int timeout_ms) : m_(m)
{
    if (xSemaphoreTakeRecursive(m_, pdMS_TO_TICKS(timeout_ms)) != pdTRUE)
    {
        char buffer[120];
        sprintf(buffer, "Failed to acquire lock %s", lock_name);
        throw(const char *) & buffer;
    }
}

Lock::~Lock()
{
    xSemaphoreGiveRecursive(m_);
}

