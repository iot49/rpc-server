#pragma once

#include "rpc.h"
#include "types.h"

void test(int n)
{
    switch (n) {
        case 0:
            rpc.send_response_array("array", 1, false, 3.4);
            break;
        case 1:
            rpc.send_response_map("k1", "v1", "k2", 2);
            break;
        case 2: {
            auto t = std::make_tuple(1, 3.8, "a string", false);
            rpc.send_response(t);
            break;
        }
        case 3: {
            std::vector<float> vf = {4.6, 7};
            std::vector<int> vi = {-3, 4, 33};
            std::vector<std::string> vs = {"a", "\x01\x04\x1b"};
            std::map<std::string, int> m{{"key1", 1}, {"key2", 2}};
            auto t = std::make_tuple(1, 3.8, "a string", false);
            bytes b = {0, 1, 2, 3, 4, 5, 6, 6, 7, 9, 10};
            rpc.send_response_map(
                "v<float>", vf,
                "v<int>", vi,
                "v<string>", vs,
                "map", m,
                "tuple", t,
                "bytes", b,
                "sizeof(map)", sizeof(m)
            );
            break;
        }
        case 4: {
            uint8_t x = 66;
            rpc.send_response(_typeof(x));
            break;
        }
        case 5: {
            int8_t x = -5;
            rpc.send_response(_typeof(x));
            break;
        }
        case 6: {
            int x = -5;
            rpc.send_response(_typeof(x));
            break;
        }
        case 7: {
            rpc.send_response(bytes{'['});
            break;
        }
        case 8: {
            float x = 0;
            bytes res{'['};
            auto t = _typeof(x);
            res.insert(res.end(), t.begin(), t.end());
            rpc.send_response(res);
            break;
        }
        default:
            rpc.send_error("ValueError", "no test", n);
    }
}

int echo_int(int x)
{
    return x;
}

std::string echo_str(std::string s)
{
    return s;
}

std::vector<int> echo_vec(std::vector<int> v)
{
    return v;
}

std::vector<uint8_t> echo_bytes(bytes v)
{
    return v;
}

std::vector<uint8_t> echo_v_uint8(std::vector<uint8_t> v)
{
    return v;
}

bool echo_bool(bool b)
{
    return b;
}

uint32_t get_time_ms()
{
    return esp_timer_get_time() / 1000;
}