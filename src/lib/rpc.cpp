#include "rpc.h"

// global
RPC rpc = RPC(MsgPack());

void RPC::init(int baud_rate, size_t rx_buffer_size, size_t tx_buffer_size)
{
    msgpack_.init(baud_rate, rx_buffer_size, tx_buffer_size);
}

void RPC::send_response_nil()
{
    Lock lk("server", msgpack_.lock());
    msgpack_.pack(task_id());
    msgpack_.pack_nil();
    msgpack_.write_eot();
}

void RPC::send_log_message(const char *msg)
{
    Lock lk("server", msgpack_.lock());
    msgpack_.pack(ID_LOG_MESSAGE);
    msgpack_.pack(msg);
    msgpack_.write_eot();
}

/**
     * Read a value of type string from serial.
     *
     * @arg {string *} data - string.
     */
void RPC::_read(std::string *data)
{
    size_t size;
    _read(&size);
    (*data).resize(size);
    for (size_t i=0;  i<size;  i++) {
        _read(&(*data)[i]);
    }
}
