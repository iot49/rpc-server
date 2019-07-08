#include <uart.h>
#include <esp_err.h>

#define ERR_OK (0)

void Uart::init(int baud_rate, size_t tx_buffer_size, size_t rx_buffer_size, gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts)
{
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = true
    };
    if (uart_param_config(port, &uart_config) != ERR_OK)
        throw "bad uart config";
    if (uart_set_pin(port, tx, rx, rts, cts) != ERR_OK)
        throw "bad uart pins";
    if (uart_driver_install(port, tx_buffer_size, rx_buffer_size, 0, nullptr, 0) != ERR_OK)
        throw "uart_driver";
};


size_t Uart::in_waiting() {
    size_t sz;
    uart_get_buffered_data_len(port, &sz);
    return sz;
}


// returns number of bytes read or -1 for error    
int Uart::read(uint8_t* buf, size_t len)
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
int Uart::write(const char *data, size_t len) 
{
    return uart_write_bytes(port, data, len);
}


int Uart::write(char byte) 
{
    return write(&byte, 1);
}


size_t Uart::readBytes(char *buffer, size_t length)
{
    size_t count = 0;
    while (count < length) {
        int c = read();
        if(c < 0) {
            break;
        }
        *buffer++ = (char)c;
        count++;
    }
    return count;
}


String Uart::readStringUntil(char terminator)
{
    String ret;
    int c = read();
    while (c >= 0 && c != terminator) {
        ret += (char)c;
        c = read();
    }
    return ret;
}