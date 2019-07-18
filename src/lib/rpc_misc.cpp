#include "rpc_misc.h"
#include "settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Arduino.h"

uint32_t heap_size()
{
    return esp_get_free_heap_size();
}

uint32_t min_free_heap_size()
{
    return esp_get_minimum_free_heap_size();
}

/* log levels:
        0  NONE
        1  ERROR
        2  WARN
        3  INFO
        4  DEBUG
        5  VERBOSE
 */
void setLogLevel(String tag, int level)
{
    esp_log_level_set(tag.c_str(), (esp_log_level_t)level);
}

String task_list()
{
    char buffer[1000];
    // vTaskGetRunTimeStats(buffer);
    vTaskList(buffer);
    return String(buffer);
}

int delay_ms(int ms)
{
    TickType_t ticks = pdMS_TO_TICKS(ms);
    vTaskDelay(ticks);
    return (int)ticks;
}

String reset_reason()
{
    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason)
    {
        case ESP_RST_POWERON:
            return String("poweron");
        case ESP_RST_SW:
            return String("esp_restart");
        case ESP_RST_PANIC:
            return String("panic");
        case ESP_RST_INT_WDT:
            return String("int WDT");
        case ESP_RST_TASK_WDT:
            return String("task WDT");
        case ESP_RST_WDT:
            return String("WDT");
        case ESP_RST_DEEPSLEEP:
            return String("deepsleep");
        case ESP_RST_BROWNOUT:
            return String("brownout");
        default:
            char buf[30];
            sprintf(buf, "unknown %d", reason);
            return String(buf);
    }
        
}

void reset()
{
    esp_restart();
}