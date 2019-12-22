#include "msgpack.h"
#include <string.h>

void MsgPack::pack(int i) {
    if (i > -32 && i < 128)
    {
        write_bytes(i, 1);
    }
    else if ((int8_t)i == i)
    {
        write_bytes(0xd0, 1);
        write_bytes(i, 1);
    }
    else if ((int16_t)i == i)
    {
        write_bytes(0xd1, 1);
        write_bytes(i, 2);
    }
    else if ((int32_t)i == i)
    {
        write_bytes(0xd2, 1);
        write_bytes(i, 4);
    }
}

void MsgPack::pack(float x)
{
    union {
        float _float;
        uint32_t _int;
    } u;
    u._float = x;
    write_bytes(0xca, 1);
    write_bytes(u._int, 4);
}

void MsgPack::pack(bool b)
{
    write_bytes(b ? 0xc3 : 0xc2, 1);
}

void MsgPack::pack_nil()
{
    write_bytes(0xc0, 1);
}

void MsgPack::pack(const char *str) {
    size_t len = strlen(str);
    if (len < 32) {
        write_bytes(0b10100000 | (uint8_t)len, 1);
    } else {
        write_size(0xd9, len);
    }
    write((const uint8_t*)str, len);
}

void MsgPack::pack(const std::string &x)
{
    pack(x.c_str());
}

void MsgPack::pack(const bytes& x)
{
    write_size(0xc4, x.size());
    write(x.data(), x.size());
}

// array and map
void MsgPack::_pack_size(size_t size, uint8_t code_short, uint8_t code_long)
{
    if (size < 16) {
        write_bytes(code_short | (uint8_t)size, 1);
    } else if (size < 0x10000) {
        write_bytes(code_long, 1);
        write_bytes(size, 2);
    } else {
        write_bytes(code_long+1, 1);
        write_bytes(size, 4);
    }
}

void MsgPack::write_bytes(uint32_t obj, size_t n_bytes)
{
    union {
        uint32_t _int;
        uint8_t  _vec[4];
    } u;
    u._int = obj;
    for (size_t i = 0; i < n_bytes;  i++) {
        write(&(u._vec[i]), 1);
    }
}

void MsgPack::write_size(uint8_t code, size_t size)
{
    if ((uint8_t)size == size) {
        write_bytes(code, 1);
        write_bytes(size, 1);
    } else if ((uint16_t)size == size) {
        write_bytes(code + 1, 1);
        write_bytes(size, 2);
    } else {
        write_bytes(code + 2, 1);
        write_bytes(size, 2);
    }
}
