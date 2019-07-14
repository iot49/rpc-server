#include "uart.h"
#include "error.h"
#include "esp_log.h"
#include <stdint.h>
#include <string.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


static const char* TAG = "connect-uart";

void Uart::init(int baud_rate, size_t rx_buffer_size, size_t tx_buffer_size,
                gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = true};
    uart_config.baud_rate = baud_rate;

    if (rx_buffer_size <= 128)
        rx_buffer_size = 129;
    if (tx_buffer_size != 0 && tx_buffer_size <= 128)
        tx_buffer_size = 129;

    err_check_log(TAG, uart_param_config(port, &uart_config));
    err_check_log(TAG, uart_set_pin(port, tx, rx, rts, cts));
    const int queue_size = 32;
    err_check_log(TAG, uart_driver_install(port, rx_buffer_size, tx_buffer_size, queue_size, &uart_queue, 0));
};

size_t Uart::in_waiting()
{
    size_t sz;
    err_check_log(TAG, uart_get_buffered_data_len(port, &sz));
    return sz;
}

// returns number of bytes read or -1 for error
int Uart::read(uint8_t *buf, size_t len)
{
    return uart_read_bytes(port, buf, len, timeout_ms / portTICK_RATE_MS);
}

uint8_t Uart::read()
{
    uint8_t buf;
    read(&buf, (size_t)1);
    return buf;
}

// returns number of bytes written or -1 for error
int Uart::write(const uint8_t *data, size_t len)
{
    if (!locked()) {
        const char *distress = "H***** uart - write without lock\n";
        uart_write_bytes(port, distress, strlen(distress));
    }
    return uart_write_bytes(port, (const char *)data, len);
}

int Uart::write(uint8_t byte)
{
    return write(&byte, 1);
}

int Uart::write(const char* str)
{
    return write((uint8_t*)str, strlen(str));
}

int Uart::events_waiting()
{
    return (int)uxQueueMessagesWaiting(uart_queue);
}

int Uart::next_event()
{
    uart_event_t event;
    if (xQueueReceive(uart_queue, (void *)&event, (portTickType)0))
    {
        return event.type;
    } 
    else 
    {
        return -1;
    }
}
