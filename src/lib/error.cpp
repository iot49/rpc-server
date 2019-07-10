#include "error.h"

// Docs: https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/error-handling.html

void err_check_log(const char *tag, esp_err_t e)
{
    if (e == ESP_OK)
        return;
    const char *msg = esp_err_to_name(e);
    printf("E (%d) %s: %s\n", esp_log_timestamp(), tag, msg);
}

void err_check_throw(esp_err_t e)
{
    if (e == ESP_OK)
        return;
    const char *msg = esp_err_to_name(e);
    throw msg;
}
