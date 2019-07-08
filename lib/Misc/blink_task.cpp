#include <blink_task.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "blink-task";

const gpio_num_t BLINK_GPIO = GPIO_NUM_13;
void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    ESP_LOGD(TAG, "starting");
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while (true)
    {
        /* Blink off (output low) */        
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void blink()
{
    xTaskCreate(&blink_task, "blink_task", 4096, NULL, 5, NULL);
}
