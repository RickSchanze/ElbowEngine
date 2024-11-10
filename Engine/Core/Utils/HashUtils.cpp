/**
 * @file HashUtils.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "HashUtils.h"
#include "xxhash.h"

uint64_t core::HashUtils::GetHash(StringView str)
{
    return XXH64(str.Data(), str.Length(), 0);
}
