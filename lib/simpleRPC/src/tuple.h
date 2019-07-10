#ifndef __SIMPLE_RPC_TUPLE_TCC__
#define __SIMPLE_RPC_TUPLE_TCC__

#include "helper.h"

/*
 * Empty tuple.
 */
template<class... Args>
struct Tuple {};

/*
 * Nested tuple.
 *
 * {T} head - First element.
 * {Args...} tail - Remaining elements.
 */
template<class T, class... Args>
struct Tuple<T, Args...> {
  T head;
  Tuple <Args...>tail;
};

/*
 * Nested object.
 *
 * Preferably this would have been an alias, but this is not supported in the
 * current version of Arduino C++.
 */
template<class... Args>
struct Object : Tuple <Args...> {
  Object(void) {}
  Object(Args... args) : Tuple <Args...>({args...}) {}
};


/*
 * Access the type of the {k}-th element in a tuple.
 *
 * https://eli.thegreenplace.net/2014/variadic-templates-in-c/#id5
 */
template<size_t, class>
struct ElemTypeHolder;

template<class T, class... Args>
struct ElemTypeHolder <0, Tuple <T, Args...> > {
  typedef T type;
};

template<size_t k, class T, class... Args>
struct ElemTypeHolder <k, Tuple <T, Args...> > {
  typedef typename ElemTypeHolder <k - 1, Tuple <Args...> >::type type;
};


/**
 * Get the {k}-th element in a Tuple.
 *
 * This can be used for both retrieving as well as setting the content of an
 * element.
 *
 * @arg {Tuple} t - A tuple.
 *
 * @return {any &} - Reference to the {k}-th element in {t}.
 */
template<size_t k, class... Args>
typename enableIf<
    k == 0, typename ElemTypeHolder <0, Tuple <Args...> >::type&>::type
    get(Tuple <Args...>&t) {
  return t.head;
}

template<size_t k, class T, class... Args>
typename enableIf<
    k != 0, typename ElemTypeHolder <k, Tuple <T, Args...> >::type&>::type
    get(Tuple <T, Args...>&t) {
  return get<k - 1>(t.tail);
}


/**
 * Make a nested tuple from a list of parameters.
 *
 * @arg {Args...} args - Values to store in a nested tuple.
 *
 * @return {Tuple} - Nested tuple containing {args}.
 */
template<class... Args>
Tuple <Args...>pack(Args... args) {
  Tuple <Args...>t = {args...};

  return t;
}

/**
 * Cast a struct to a tuple.
 *
 * @arg {T &} s - Struct.
 *
 * @return {Tuple} - Nested tuple representation of {s}.
 */
template<class... Args, class T>
Tuple <Args...>castStruct(T &s) {
  Tuple <Args...>*t = (Tuple <Args...> *)&s;

  return *t;
}

#endif
