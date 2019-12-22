#include "msgcomm.h"
#include "error.h"
#include <stdint.h>
#include <string.h>
#include <string>
#include <driver/uart.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char* TAG = "connect-msgcomm";

const uint8_t EOT = 0x04;
const uint8_t ESC = 0x1b;
const uint8_t OFF = 0x20;


MsgComm::MsgComm(uart_port_t uart_num, int timeout_ms) : port{uart_num}
{
    timeout_ticks = pdMS_TO_TICKS(timeout_ms);
}

void MsgComm::init(int baud_rate,
          size_t rx_buffer_size, size_t tx_buffer_size,
          gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,  // actual value set below
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = true};
    uart_config.baud_rate = baud_rate;

    if (rx_buffer_size <  16)
        rx_buffer_size =  16;
    if (tx_buffer_size != 0 && tx_buffer_size < 129)
        tx_buffer_size = 129;

    err_check_log(TAG, uart_param_config(port, &uart_config));
    err_check_log(TAG, uart_set_pin(port, tx, rx, rts, cts));

    /* Double buffering:
      We need a count of EOT's in the RX buffer.
      I could not get the pattern detector working, so instead we do our "own"
      buffering in rx_buffer.
      So we don't need to deal with interrupts, we still use the buffered driver.
      The size should be large enough to keep the uart busy.
    */
    err_check_log(TAG, uart_driver_install(port, 512, tx_buffer_size, 0, NULL, 0));

    delete rx_buffer;
    rx_buffer = new Ringbuf<uint8_t>(rx_buffer_size, 0);

    ESP_LOGI(TAG, "initialized");
}

// transfer from driver rx buffer to rx_buffer
void MsgComm::check_rx()
{
    size_t length;
    err_check_log(TAG, uart_get_buffered_data_len(port, (size_t*)&length));
    if (length > rx_buffer->free()) length = rx_buffer->free();
    for (size_t i=0;  i<length;  i++) {
        uint8_t buf;
        // should never timeout since we read only what we know if in buffer ...
        int n = uart_read_bytes(port, &buf, (size_t)1, timeout_ticks);
        if (n != 1) throw "check_rx internal error";
        if (buf == EOT) msg_waiting++;
        rx_buffer->put(buf);
    }
}

size_t MsgComm::read(uint8_t *buf, size_t len)
{
    check_rx();
    if (messages_waiting() < 1) {
        throw "message buffer empty";
    }

    size_t rx_bytes = 0;
    int b;
    while (rx_bytes < len && (b = getc_()) >= 0)
    {
        if (b == EOT) {
            msg_waiting--;
            throw "read past EOT";
        }
        if (b == ESC) {
            b = getc_() - OFF;
        }
        buf[rx_bytes] = b;
        rx_bytes++;
    }
    return rx_bytes;
}

size_t MsgComm::read_eot() {
    check_rx();
    if (messages_waiting() < 1) {
        throw "message buffer empty";
    }

    size_t rx_bytes = 0;
    while (true) {
        // we know this won't fail
        uint8_t buf = getc_();
        if (buf == EOT) {
            msg_waiting--;
        } else {
            rx_bytes++;
        }
    }

    return rx_bytes;
}

std::string MsgComm::read_string() {
    // read null-terminated string
    std::string str;
    for (;;) {
        uint8_t buf;
        read(&buf, 1);
        if (buf == 0) break;
        str += (char)buf;
    }
    return str;
}

size_t MsgComm::write(const uint8_t *data, size_t len) {
    size_t tx_bytes;
    for (tx_bytes = 0; tx_bytes < len; tx_bytes++)
    {
        uint8_t b = data[tx_bytes];
        if (b == ESC || b == EOT) {
            putc_(ESC);
            putc_(b + OFF);
        } else {
            putc_(b);
        }
    }
    return tx_bytes;
}

void MsgComm::write_eot() {
    putc_(EOT);
}

size_t MsgComm::write(const char *str)
{
    return write((const uint8_t*)str, strlen(str));
}
