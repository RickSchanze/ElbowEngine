/**
 * @file HashUtils.cpp
 * @author Echo
 * @Date 24-11-9
 * @brief
 */

#include "HashUtils.h"
#include "xxhash.h"

uint64_t core::HashUtils::GetHash(StringView str) { return XXH64(str.Data(), str.Length(), 0); }

UInt64 core::HashUtils::CombineHash(UInt64 a, UInt64 b) { return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2)); }

UInt64 core::HashUtils::GetPtrHash(void *ptr) { return reinterpret_cast<UInt64>(ptr); }
