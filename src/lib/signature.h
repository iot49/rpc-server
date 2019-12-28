#pragma once

#include "types.h"
#include <string>
#include <vector>

typedef std::vector<std::string> signature_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
// #pragma GCC diagnostic ignored "-Wpmf-conversions"

/**
 * Recursion terminator for {_parameterTypes()}.
 */
void _parameterTypes(signature_t &sig, void (*)(void));

/**
 * Get the types of all function parameters.
 *
 * We isolate the first parameter type {T} from function pointer {*f_}. This
 * type is used to instantiate the variable {data}, which is passed to
 * {_typeof()} to encode its type. The first parameter type {T} is removed from
 * function pointer {*f_} in the recursive call.
 *
 * @arg {void (*)(T, Args...)} f_ - Dummy function pointer.
 *
 * @return {std::string} - Space separated parameter types.
 */
template<class T, class... Args>
void _parameterTypes(signature_t &sig, void (*f_)(T, Args...)) {
    T data;
    sig.push_back(_typeof(data));
    _parameterTypes(sig, (void (*)(Args...))f_);
}

/*
// Parameter of type {T &}.
template<class T, class... Args>
void _parameterTypes(signature_t &sig, void (*f_)(T &, Args...)) {
  T data;
    sig.push_back(_typeof(data));
    _parameterTypes(sig, (void (*)(Args...))f_);
}
*/

/**
 * Get the signature of a function.
 *
 * We prepare a dummy function pointer, referred to as {f_} in the template
 * functions above, which will be used to isolate parameter types. The return
 * type of this function pointer is removed to avoid unneeded template
 * expansion.
 *
 * @arg {R (*)(Args...)} f - Function pointer.
 *
 * @return {std::string} - Function signature.
 */
template<class R, class... Args>
void signature(signature_t &sig, R (*f)(Args...)) {
    _parameterTypes(sig, (void (*)(Args...))f);
}

// Void function.
template<class... Args>
void signature(signature_t &sig, void (*f)(Args...)) {
    _parameterTypes(sig, f);
}

/*
// Class member function.
template<class R, class C, class... Args>
void signature(signature_t &sig, R (C::*f)(Args...)) {
    _parameterTypes(sig, (void (*)(Args...))f);
}

// Void class member function.
template<class C, class... Args>
void signature(signature_t &sig, void (C::*f)(Args...)) {
    _parameterTypes(sig, (void (*)(Args...))f);
}
*/

#pragma GCC diagnostic pop
