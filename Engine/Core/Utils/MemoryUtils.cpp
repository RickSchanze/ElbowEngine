/**
 * @file MemoryUtils.cpp
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#include "MemoryUtils.h"

#include "Windows.h"

void* MemoryUtils::AlignedAlloc(size_t size, size_t alignment)
{
    void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
    data = _aligned_malloc(size, alignment);
#else
    int res = posix_memalign(&data, alignment, size);
    if (res != 0) data = nullptr;
#endif
    return data;
}

void MemoryUtils::AlignedFree(void* ptr)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(data);
#endif
}

void* MemoryUtils::AlignedRealloc(void* ptr, size_t size, size_t alignment)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_realloc(ptr, size, alignment);
#endif
}
