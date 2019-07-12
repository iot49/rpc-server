#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "rpc_test.h"

String string_test(String &a) { return a; }
int throw_test() { throw "throw test!"; }
float float_test(float f) { return f+1; }

String newline(String &s)
{
    s.replace("\n", "_");
    return s;
}

static SemaphoreHandle_t test_lock = NULL; //xSemaphoreCreateMutex();
 
bool lock(int timeout_ms)
{
    if (test_lock == NULL) {
        test_lock = xSemaphoreCreateMutex();
        ESP_LOGI("lock", "test_lock = %8x", (uint)test_lock);
        if (test_lock == NULL) {
            ESP_LOGI("lock", "test_lock = NULL");
            return false;
        }
    }
    return xSemaphoreTake(test_lock, (TickType_t)timeout_ms);
}

bool unlock()
{
    if (test_lock == NULL)
    {
        test_lock = xSemaphoreCreateMutex();
        ESP_LOGI("unlock", "test_lock = %8x", (uint)test_lock);
        if (test_lock == NULL)
        {
            ESP_LOGI("unlock", "test_lock = NULL");
            return false;
        }
    }
    return xSemaphoreGive(test_lock);
}

const char* TAG = "lock-test";

class LockTest {
    SemaphoreHandle_t &m_;
public:
    LockTest(const char *lock_name, SemaphoreHandle_t m, int timeout_ms) : m_(m)
    {
        if (m == NULL)
        {
            m = test_lock = xSemaphoreCreateMutex();
            ESP_LOGI(TAG, "created test_lock = %8x", (uint)test_lock);
         }
        ESP_LOGI(TAG, "Waiting for lock %s", lock_name);
        if (xSemaphoreTake(m_, (TickType_t)timeout_ms) != pdTRUE)
        {
            ESP_LOGI(TAG, "Failed to acquire lock %s", lock_name);
        } else {
            ESP_LOGI(TAG, "Got lock %s", lock_name);
        }
    }
    ~LockTest()
    {
        if (xSemaphoreGive(m_) == pdTRUE) {
            ESP_LOGI(TAG, "Released lock");
        } else {
            ESP_LOGI(TAG, "Failed to release lock");
        }
    }
};

int lock_test(int timeout)
{
    LockTest("lock_test", test_lock, timeout);
    ESP_LOGI(TAG, "In protected region");
    return 543;
}