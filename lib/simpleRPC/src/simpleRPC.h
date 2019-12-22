#ifndef __SIMPLE_RPC_H__
#define __SIMPLE_RPC_H__

#include "defs.h"
#include "interface.h"

byte _ping(byte data) {
  return data;
}


/**
 * RPC interface.
 *
 * This function expects parameter pairs (function pointer, name).
 *
 * @arg {Args...} args - Parameter pairs (function pointer, name).
 */
template<class... Args>
void interface(Args... args) {
    rpcInterface(
        _ping, "ping",
        args...);
}

#endif
