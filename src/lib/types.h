#pragma once

#include <string>
#include <vector>

typedef std::vector<uint8_t> bytes;

/*
 * Type encoding functions.
 *
 * For more information about the encoding:
 * https://docs.python.org/3.5/library/struct.html#format-std::strings
 */
inline std::string _typeof(bool) {
    return "B";
}

inline std::string _typeof(char) {
    return "c";
}

inline std::string _typeof(int8_t) {
    return "b";
}

inline std::string _typeof(uint8_t) {
    return "B";
}

inline std::string _typeof(int16_t) {
    return "<h";
}

inline std::string _typeof(uint16_t) {
    return "<H";
}

inline std::string _typeof(int32_t) {
    return "<l";
}

inline std::string _typeof(uint32_t) {
    return "<L";
}

inline std::string _typeof(int64_t) {
    return "<q";
}

inline std::string _typeof(uint64_t) {
    return "<Q";
}

inline std::string _typeof(float) {
    return "<f";
}

// special case to handle strings
inline std::string _typeof(std::string) {
    return "\"";
}

// special case to speed up vector<uint8_t>
inline std::string _typeof(bytes) {
    return "#";
}

/**
 * vector<T> type.
 */
template<class T>
std::string _typeof(std::vector <T>&) {
    T x;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
    return "[" + _typeof(x);
#pragma GCC diagnostic pop
}
