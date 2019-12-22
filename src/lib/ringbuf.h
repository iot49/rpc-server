#pragma once

#include <stddef.h>

template<typename T>
class Ringbuf {

private:

    T *buf;
    T def;
    size_t sz;
    size_t iget = 0;
    size_t iput = 0;

public:

    // default_element returned by get when buffer empty
    explicit Ringbuf(int capacity, T default_element) {
        sz = capacity+1;
        buf = new T[sz];
        def = default_element;
    }

    ~Ringbuf() { delete[] buf;  buf = nullptr; }

    // number of elements in buffer
    size_t size() const {
        size_t count = iput - iget;
        if (count < 0) count += sz;
        return count;
    }

    // free space
    size_t free() const {
        return sz - size() - 1;
    }

    // capacity
    size_t capacity() const {
        return sz-1;
    }

    // return oldest element or default_element if buffer is empty
    T get() {
        if (iget == iput) return def;
        T v = buf[iget++];
        if (iget >= sz) iget = 0;
        return v;
    }

    // add element if space, return success
    bool put(T v) {
        size_t iput_new = iput + 1;
        if (iput_new >= sz) iput_new = 0;
        if (iput_new == iget) return false;
        buf[iput] = v;
        iput = iput_new;
        return true;
    }

};
