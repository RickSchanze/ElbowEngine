/**
 * @file MemoryUtils.cpp
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#include "MemoryUtils.h"

UInt64 MemoryUtils::GetAlignedSize(UInt64 size, UInt64 align)
{
    return (size + align - 1) & ~(align - 1);
}

UInt64 MemoryUtils::GetAlignedOffset(UInt64 offset, UInt64 align)
{
    return (offset + align - 1) & ~(align - 1);
}