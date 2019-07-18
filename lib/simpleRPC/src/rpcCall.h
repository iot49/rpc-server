#ifndef __SIMPLE_RPC_RPCCALL_TCC__
#define __SIMPLE_RPC_RPCCALL_TCC__

#include "settings.h"
#include "lib/uart.h"
#include "lib/rpc_task.h"
#include "esp_log.h"

#include "read.h"
#include "tuple.h"
#include "write.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"

/**
 * Execute a function.
 *
 * All parameters have been collected since function pointer {*f_} has no
 * parameter types. All values are now present in the {args} parameter pack.
 *
 * @arg {void (*)(void)} - Dummy function pointer.
 * @arg {R (*)(Tail...)} f - Function pointer.
 * @arg {Args...} args... - Parameter pack for {f}.
 */
template<class R, class... Tail, class... Args>
void _call(void (*)(void), R (*f)(Tail...), Args&... args) {
    // release lock if this is an async call
    bool is_async = rpc->is_async_call();
    if (is_async) rpc->unlock();

    // evaluate
    R data = f(args...);

    // return results
    {
        Lock lock("uart", uart.lock());
        // result type
        if (is_async)
        {
            Serial.write(RPC_ASYNC_RESPONSE);
            int aid = rpc->async_id();
            _write(&aid);
            byte cmd = rpc->command();
            _write(&cmd);
        }
        else
        {
            Serial.write(RPC_RESPONSE);
        }
        // result
        _write(&data);
    }
}

// Void function.
template<class... Tail, class... Args>
void _call(void (*)(void), void (*f)(Tail...), Args&... args) {
    bool is_async = rpc->is_async_call();
    if (is_async) rpc->unlock();

    // evaluate
    f(args...);

    // acknowledge
    {
        Lock lock("uart", uart.lock());
        if (is_async) {
            Serial.write(RPC_ASYNC_VOID_RESPONSE);
            int aid = rpc->async_id();
            _write(&aid);
            uint8_t cmd = rpc->command();
            _write(&cmd);
        }
        else
        {
            Serial.write(RPC_VOID_RESPONSE);
        }
    }
}

// Class member function.
template<class C, class P, class R, class... Tail, class... Args>
void _call(void (*)(void), Tuple <C *, R (P::*)(Tail...)>t, Args&... args) {
    // release lock if this is an async call
    bool is_async = rpc->is_async_call();
    if (is_async) rpc->unlock();

    // evaluate
    R data = (*t.head.*t.tail.head)(args...);

    // send results
    {
        Lock lock("uart", uart.lock());
        // result type
        if (is_async)
        {
            Serial.write(RPC_ASYNC_RESPONSE);
            int aid = rpc->async_id();
            _write(&aid);
            uint8_t cmd = rpc->command();
            _write(&cmd);
        }
        else
        {
            Serial.write(RPC_RESPONSE);
        }
        // result
        _write(&data);
    }
}

// Void class member function.
template<class C, class P, class... Tail, class... Args>
void _call(void (*)(void), Tuple <C *, void (P::*)(Tail...)>t, Args&... args) {
    bool is_async = rpc->is_async_call();
    if (is_async) rpc->unlock();
    // evaluate function
    (*t.head.*t.tail.head)(args...);
    // acknowledge
    {
        Lock lock("uart", uart.lock());
        if (is_async) {
            Serial.write(RPC_ASYNC_VOID_RESPONSE);
            int aid = rpc->async_id();
            _write(&aid);
        }
        else
        {
            Serial.write(RPC_VOID_RESPONSE);
        }
    }
}

/**
 * Collect parameters of a function from serial.
 *
 * We isolate the first parameter type {T} from function pointer {*f_}. This
 * type is used to instantiate the variable {data}, which is used to receive
 * {sizeof(T)} bytes from the serial stream. This value is passed recursively
 * to {_call}, adding it to the {args} parameter pack. The first parameter type
 * {T} is removed from function pointer {*f_} in the recursive call.
 *
 * @arg {void (*)(T, Tail...)} f_ - Dummy function pointer.
 * @arg {F} f - Function pointer.
 * @arg {Args...} args... - Parameter pack for {f}.
 */
template<class T, class... Tail, class F, class... Args>
void _call(void (*f_)(T, Tail...), F f, Args... args) {
    T data;

    _read(&data);
    _call((void (*)(Tail...))f_, f, args..., data);
}

// Parameter of type {T &}.
template<class T, class... Tail, class F, class... Args>
void _call(void (*f_)(T &, Tail...), F f, Args... args) {
    T data;

    _read(&data);
    _call((void (*)(Tail...))f_, f, args..., data);
}


/**
 * Set up function parameter collection, execution and writing to serial.
 *
 * We prepare a dummy function pointer, referred to as {f_} in the template
 * functions above, which will be used to isolate parameter types. The return
 * type of this function pointer is removed to avoid unneeded template
 * expansion.
 *
 * @arg {R (*)(Args...)} f - Function pointer.
 */
template<class R, class... Args>
void rpcCall(R (*f)(Args...)) {
    _call((void (*)(Args...))f, f);
}

// Class member function.
template<class C, class P ,class R, class... Args>
void rpcCall(Tuple <C *, R (P::*)(Args...)>t) {
    _call((void (*)(Args...))t.tail.head, t);
}

#pragma GCC diagnostic pop

#endif
