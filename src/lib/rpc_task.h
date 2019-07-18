#pragma once

void rpc_task(void *pvParameter);

class RPC
{
public:
    RPC(int id_) : id_{id_} {}
    int id() { return id_; }              // id of this task (just FYI)
    bool is_locked() { return has_lock; } // true if currently holds rpc_lock
    void unlock();                        // release lock, if owned
    void run_forever();                   // handle rpc calls
    void async_call(int aid, int cmd) { aid_ = aid;  is_async = true; cmd_ = cmd; }
    bool is_async_call() { return is_async; }
    int async_id() { return aid_; }
    int command() { return cmd_; }
protected:
    int id_;                              // task id, just FYI
    int aid_ = 0;                         // async call number
    int cmd_ = 0;                         // number of currently executing rpc call
    bool is_async = false;                // call in progress is asynchronous
    bool has_lock = false;                // true if this task holds rpc_lock
    void simpleRPC();                     // rpc call dispatcher
    void lock();                          // block until acquired rpc_lock
};

extern __thread RPC *rpc;
