#include "rpcserver.h"
#include "locks.h"

void RPCServer::send_log_message(const char* msg) {
    Lock lk("server", lock());
    pack(1);
    pack(msg);
    write_eot();
}
