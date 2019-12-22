#pragma once

#include "msgpack.h"
#include "locks.h"

/*
  Global singleton: rpc functions need access to pack ...

  rpc functions:
  - global (not class members)
  - receive id and binary arguments from host
  - reserved id's:
      * neg response id indicates error
      * 0 --> do not send response (dubious)
      * 1 --> log message
      * 2 --> system error
  - response:
      * non-void return type: serve calls send_response
      * void functions are reponsible to call send_response eventually
  - tasks: pending work of rpc_functions
*/

class RPCServer : public MsgPack {
private:

    int task_id_;

public:

    // id of currently running task
    int task_id() { return task_id_; }

    // handle pending tasks and look for more work
    // never returns
    template<typename... Args>
    void serve(Args... args) {
        // TODO: handle pending tasks (see Task.h)
        // do this agressively to keep work from piling up (and filling buffers)
        // abort overdue tasks (timeout)
        if (messages_waiting() > 0) {
            // TODO: read 16-bit task id from rx
            task_id = 128;
            interface(args...);
        }
    }

    template<typename T>
    void send_response(T obj) {
        Lock lk("server", lock());
        pack(task_id());
        pack(obj);
        write_eot();
    }

    template<typename... Args>
    void send_response_array(Args... args) {
        Lock lk("server", lock());
        pack(task_id());
        pack_array(args...);
        write_eot();
    }

    template<typename... Args>
    void send_response_map(Args... args) {
        Lock lk("server", lock());
        pack(task_id());
        pack_map(args...);
        write_eot();
    }

    void send_log_message(const char* msg);

};
