/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "sdkconfig.h" 
#include<stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "custom_logger.h"

static const char *TAG = "rpc-server";

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

const gpio_num_t BLINK_GPIO = (gpio_num_t)13;
// #define BLINK_GPIO 13

    void
    blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while (true) {
        /* Blink off (output low) */
        ESP_LOGD(TAG, "blink low");
         gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(900 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        ESP_LOGD(TAG, "blink high");
         gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    esp_log_set_vprintf(_log_vprintf);
    esp_log_level_set("*", ESP_LOG_DEBUG);
    
    printf("hello from rpc-server\n");
    printf("log debug   level %d\n", ESP_LOG_DEBUG);
    printf("log v_v     level %d\n", ESP_LOG_VERBOSE);
    printf("log none    level %d\n", ESP_LOG_NONE);
    fflush(stdout);
    ESP_LOGD(TAG, "server started");

    printf("Hello world!\n");
    fflush(stdout);

    xTaskCreate(&blink_task, "blink_task", 4096, NULL, 5, NULL);
}
