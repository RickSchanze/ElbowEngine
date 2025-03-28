//
// Created by Echo on 2025/3/25.
//
#pragma once
#include <exception>

#include "Core/TypeAlias.hpp"


inline void Exit(const Int32 code) { exit(code); }

template<typename T>
T *AddressOf(T &value) {
    return &value;
}

inline void RethrowException(const std::exception_ptr &ptr) { std::rethrow_exception(ptr); }

inline ExceptionPtr CurrentException() { return std::current_exception(); }

class HashUtils {
public:
    static UInt64 CombineHash(const UInt64 a, const UInt64 b) { return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2)); }
    static UInt64 GetPtrHash(void *ptr) { return reinterpret_cast<UInt64>(ptr); }
};

template<typename T>
constexpr UInt64 SizeOf(T &&t) {
    return std::size(std::forward<T>(t));
}

template<typename T, UInt64 I>
void CopyArray(const T (&src)[I], T (&dst)[I]) {
    for (UInt64 i = 0; i < I; ++i) {
        dst[i] = src[i];
    }
}
