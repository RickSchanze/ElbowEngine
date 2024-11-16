/**
 * @file HashUtils.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Core/Base/EString.h"

namespace core
{
class HashUtils {
public:
    static uint64_t GetHash(StringView str);
};
}
