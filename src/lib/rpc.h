#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <map>

#include "signature.h"
#include "msgpack.h"
#include "locks.h"

typedef std::tuple<uint8_t, signature_t> func_t;

class RPC {
public:
    RPC(MsgPack msgpack) : msgpack_{msgpack} {}

    void init(int baud_rate, size_t rx_buffer_size, size_t tx_buffer_size);

    // backdoor direct access to msgpack/msgcomm. Use with caution!
    MsgPack get_msgpack() { return msgpack_; }

    // run rpc method, load arguments and send results via MsgPack/MsgComm
    template <class... Args>
    void run(Args... args);

    // id of currently running task
    int task_id() { return (int)task_id_; }

    template <typename T>
    void send_response(T obj);

    template <typename... Args>
    void send_response_array(Args... args);

    template <typename... Args>
    void send_response_map(Args... args);

    template <typename... Args>
    void send_error(Args... args);

    void send_response_nil();

    void send_log_message(const char *msg);

private:
    MsgPack msgpack_;
    int16_t task_id_;
    std::map<std::string,func_t> description;

    template<class R, class... Tail, class... Args>
    void _call(void (*)(void), R (*f)(Tail...), Args&... args);

    template<class... Tail, class... Args>
    void _call(void (*)(void), void (*f)(Tail...), Args&... args);

    template<class T, class... Tail, class F, class... Args>
    void _call(void (*f_)(T, Tail...), F f, Args&... args);

    template<class R, class... Args>
    void rpcCall(R (*f)(Args...));

    template<class F, class D>
    void _writeDescription(uint8_t id, F f, D doc);

    void _describe(uint8_t);

    template <class F, class D, class... Args>
    void _describe(uint8_t id, F f, D doc, Args... args);

    void _select(uint8_t, uint8_t);

    template<class F, class D, class... Args>
    void _select(uint8_t id, uint8_t depth, F f, D, Args... args);

    template <class T>
    void _read(T *data);

    void _read(std::string *data);

    template <class T>
    void _read(std::vector<T> *data);

};

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
template <class R, class... Tail, class... Args>
void RPC::_call(void (*)(void), R (*f)(Tail...), Args &... args)
{
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    R data = f(args...);
    send_response(data);
}

// Void function.
template <class... Tail, class... Args>
void RPC::_call(void (*)(void), void (*f)(Tail...), Args &... args)
{
    f(args...);
    // caller sends response
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
template <class T, class... Tail, class F, class... Args>
void RPC::_call(void (*f_)(T, Tail...), F f, Args &... args)
{
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
template <class R, class... Args>
void RPC::rpcCall(R (*f)(Args...))
{
    RPC::_call((void (*)(Args...))f, f);
}

/**
 * Write the signature and documentation of a function to serial.
 *
 * @arg {F} f - Function pointer.
 * @arg {D} doc - Function documentation.
 */
template <class F, class D>
void RPC::_writeDescription(uint8_t id, F f, D doc)
{
    signature_t sig;
    signature(sig, f);
    func_t func(id, sig);
    description.insert({doc, func});
}

/**
 * Recursion terminator for {_describe()}.
 */
inline void RPC::_describe(uint8_t) {}

/**
 * Describe a list of functions.
 *
 * We isolate the first two parameters {f} and {doc}, pass these to
 * {_writeDescription} and make a recursive call to process the remaining
 * parameters.
 *
 * @arg {F} f - Function pointer.
 * @arg {D} doc - Function documentation.
 * @arg {Args...} args - Remaining parameters.
 */
template <class F, class D, class... Args>
void RPC::_describe(uint8_t id, F f, D doc, Args... args)
{
    _writeDescription(id, f, doc);
    _describe(id+1, args...);
}

/**
 * Recursion terminator for {_select()}.
 */
inline void RPC::_select(uint8_t id, uint8_t depth) {}

/**
 * Select and call a function indexed by {number}.
 *
 * We isolate the parameter {f} and its documentation string, discarding the
 * latter. If we have arrived at the selected function (i.e., if {depth} equals
 * {number}), we call function {f}. Otherwise, we try again recursively.
 *
 * @arg {uint8_t} id - Function index.
 * @arg {uint8_t} depth - Current index.
 * @arg {F} f - Function pointer.
 * @arg {D} - Function documentation.
 * @arg {Args...} args - Remaining parameters.
 */
template <class F, class D, class... Args>
void RPC::_select(uint8_t id, uint8_t depth, F f, D, Args... args)
{
    if (depth == id)
    {
        rpcCall(f);
    }
    else
    {
        _select(id, depth + 1, args...);
    }
}

template <class... Args>
void RPC::run(Args... args)
{
    if (msgpack_.messages_waiting() < 1) return;
    _read(&task_id_);
    uint8_t method_id = 0;
    _read(&method_id);
    if (method_id == 255)
    {
        description.clear();
        _describe(0, args...);

        func_t func((uint8_t)255, signature_t{});
        description.insert({"methods", func});

        send_response(description);
    }
    else
    {
        _select(method_id, 0, args...);
    }
    msgpack_.read_eot();
}

template <typename T>
void RPC::send_response(T obj)
{
    Lock lk("server", msgpack_.lock());
    msgpack_.pack(task_id());
    msgpack_.pack(obj);
    msgpack_.write_eot();
}

template <typename... Args>
void RPC::send_response_array(Args... args)
{
    Lock lk("server", msgpack_.lock());
    msgpack_.pack(task_id());
    msgpack_.pack_array(args...);
    msgpack_.write_eot();
}

template <typename... Args>
void RPC::send_response_map(Args... args)
{
    Lock lk("server", msgpack_.lock());
    msgpack_.pack(task_id());
    msgpack_.pack_map(args...);
    msgpack_.write_eot();
}

template <typename... Args>
void RPC::send_error(Args... args)
{
    Lock lk("server", msgpack_.lock());
    msgpack_.pack(-task_id());
    msgpack_.pack_array(args...);
    msgpack_.write_eot();
}

/**
     * Read a value of basic type from serial.
     *
     * @arg {T *} data - Data.
     */
template <class T>
void RPC::_read(T *data)
{
    msgpack_.read(data, sizeof(T));
}

/**
     * Read a value of type vector from serial.
     *
     * @arg {vector <T> *} data - vector.
     */
template <class T>
void RPC::_read(std::vector<T> *data)
{
    size_t size;
    _read(&size);
    (*data).resize(size);
    for (int i = 0; i < (*data).size(); i++)
    {
        _read(&(*data)[i]);
     }
}

extern RPC rpc;
