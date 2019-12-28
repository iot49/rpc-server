#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <typeinfo>

#include "msgcomm.h"


/*
MsgPack output according to https://github.com/msgpack/msgpack/blob/master/spec.md.

Features:
- packs basic types and collections, e.g.
  tuple<...>, vector<int>, map<string,float>
- bytes (vector<uint8_t>) output as bin
- pack_array and pack_map output their arguments in the respective formats

Limitations:
- no ext
- float 64 output as float 32
- no uint
- no fixext
*/

typedef bool boolean;
typedef std::vector<uint8_t> bytes;

class MsgPack : public MsgComm {
public:

    template<typename T>
    void pack(T x) {
        pack("MsgPack: no compatible packer");
        /*
        // cannot use typeid with -fno-rtti
        char buffer[128];
        sprintf(buffer, "No packer for %.100s ...", typeid(T).name());
        pack(buffer);
        */
    }

    void pack(int x);
    void pack(uint8_t x) { pack((int)x); }
    void pack(int8_t x) { pack((int)x); }
    void pack(uint16_t x) { pack((int)x); }
    void pack(int16_t x) { pack((int)x); }
    void pack(size_t x) { pack((int)x); }
    void pack(float x);
    void pack(double x) { pack((float)x); }
    void pack(bool b);
    void pack_nil();

    void pack(const char* x);
    void pack(const std::string& x);
    void pack(const bytes& x);

    // tuple (msgpack array)
    template<typename... Args>
    void pack(const std::tuple<Args...>& t)
    {
        _pack_size(sizeof...(Args), 0b10010000, 0xdc);
        TuplePacker<decltype(t), sizeof...(Args)>::pack_tuple(this, t);
    }

    // array-like containers (vector, ...), msgpack array
    template<template <typename, typename> class ContainerType,
              typename ValueType,
              typename AllocType>
    void pack(const ContainerType<ValueType, AllocType>& c) {
        _pack_size(c.size(), 0b10010000, 0xdc);
        for (const auto& v : c) pack(v);
    }

    // map-like containers, msgpack map
    template<template <typename, typename...> class ContainerType,
              typename ValueType, typename... Args>
    void pack(const ContainerType<ValueType, Args...>& c) {
        _pack_size(c.size(), 0b10000000, 0xde);
        for (const auto& v : c) {
            pack(v.first);
            pack(v.second);
        }
    }

    template<typename... Args>
    void pack_array(const Args... args) {
        size_t size = sizeof...(Args);
        _pack_size(size, 0b10010000, 0xdc);
        _pack_array(args...);
    }

    template<typename... Args>
    void pack_map(const Args... args) {
        size_t size = sizeof...(Args) / 2;
        _pack_size(size, 0b10000000, 0xde);
        _pack_map(args...);
    }

protected:

    void write_bytes(uint32_t obj, size_t n_bytes);
    void write_size(uint8_t code, size_t size);
    void _pack_size(size_t size, uint8_t code_short, uint8_t code_long);

    template<class Tuple, std::size_t N>
    struct TuplePacker {
        static void pack_tuple(MsgPack *mp, const Tuple& t)
        {
            TuplePacker<Tuple, N-1>::pack_tuple(mp, t);
            mp->pack(std::get<N-1>(t));
        }
    };

    template<class Tuple>
    struct TuplePacker<Tuple, 1> {
        static void pack_tuple(MsgPack *mp, const Tuple& t)
        {
            mp->pack(std::get<0>(t));
        }
    };

    template<typename... Args>
    void _pack_array(void) {}

    template<typename T, typename... Args>
    void _pack_array(const T first, const Args... args) {
        pack(first);
        _pack_array(args...);
    }

    template<typename... Args>
    void _pack_map(void) {}

    template<typename K, typename V, typename... Args>
    void _pack_map(const K key, const V value, const Args... args) {
        pack(key);
        pack(value);
        _pack_map(args...);
    }
};
