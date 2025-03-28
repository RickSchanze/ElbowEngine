//
// Created by Echo on 25-3-19.
//
#pragma once
#include "Core/TypeAlias.hpp"

inline void Memcpy(void *dest, const void *src, const size_t n) { memcpy(dest, src, n); }

inline UInt64 GetAlignedSize(const UInt64 size, const UInt64 align) { return (size + align - 1) & ~(align - 1); }

inline UInt64 GetAlignedOffset(const UInt64 offset, const UInt64 align) { return (offset + align - 1) & ~(align - 1); }
