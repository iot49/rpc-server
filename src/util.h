#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Arduino.h"


uint32_t getHeapSize() {
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
    return info.total_free_bytes + info.total_allocated_bytes;
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