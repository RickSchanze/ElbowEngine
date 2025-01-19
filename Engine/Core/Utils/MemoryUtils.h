/**
 * @file MemoryUtils.h
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"

class MemoryUtils {
public:
    static UInt64 GetAlignedSize(UInt64 size, UInt64 align);
    static UInt64 GetAlignedOffset(UInt64 offset, UInt64 align);
};
