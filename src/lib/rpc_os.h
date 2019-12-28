#pragma once

#include "rpc.h"

typedef std::tuple<int,int,int> version_t;

version_t version()
{
    return version_t(0, 1, 0);
}

void reset_uart()
{
    rpc.send_error("not implemented");
}
