#pragma once

#include "rpc.h"
#include "esp_heap_caps.h"

#include <iostream>

typedef std::tuple<int,int,int> version_t;

version_t version()
{
    return version_t(0, 1, 0);
}

uint32_t get_baudrate()
{
    return rpc.get_msgpack().get_baudrate();
}

uint32_t reset_uart(uint32_t baudrate, size_t rx_buffer_size, size_t tx_buffer_size)
{
    return rpc.get_msgpack().reset_uart(baudrate, rx_buffer_size, tx_buffer_size);
}

void heap_info()
{
    // see esp32 idf MALLOC_CAP_* for more detailed reports
    multi_heap_info_t info;
    uint32_t malloc_cap = 0;
    heap_caps_get_info(&info, malloc_cap);
    rpc.send_response_map(
        "total_free_bytes", info.total_free_bytes,
        "total_allocated_bytes", info.total_allocated_bytes,
        "largest_free_block", info.largest_free_block,
        "minimum_free_bytes", info.minimum_free_bytes,
        "allocated_blocks", info.allocated_blocks,
        "free_blocks", info.free_blocks,
        "total_blocks", info.total_blocks
    );
}