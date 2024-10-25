/**
 * @file ProfileMacro.h
 * @author Echo 
 * @Date 24-9-15
 * @brief 
 */

#pragma once

#ifdef ENABLE_PROFILING
#include "tracy/Tracy.hpp"

uint32_t GetColor(int index);

struct ColorScoped
{
    static inline int32_t level = 0;

    ColorScoped() { level++; }

    ~ColorScoped() { level--; }
};

static uint32_t GetColor()
{
    return GetColor(ColorScoped::level);
}

template<typename T>
struct MemoryTraceAllocator
{
    using value_type = T;

    MemoryTraceAllocator() = default;

    template<typename U>
    constexpr explicit MemoryTraceAllocator(const MemoryTraceAllocator<U>&) noexcept
    {
    }

    [[nodiscard]] T* allocate(std::size_t n)
    {
        if (n > std::size_t(-1) / sizeof(T))
        {
            throw std::bad_alloc();
        }
        T* p = static_cast<T*>(std::malloc(n * sizeof(T)));
        if (!p)
        {
            throw std::bad_alloc();
        }
        TracyAlloc(p, n * sizeof(T));
        return p;
    }

    void deallocate(T* p, std::size_t n) noexcept
    {
        TracyFree(p);
        std::free(p);
    }
};

template<typename T>
struct MemoryTraceDeleter
{
    void operator()(T* ptr) const
    {
        if (ptr)
        {
            ptr->~T();   // 调用析构函数
            TracyFree(ptr);
            std::free(ptr);
        }
    }
};

template<typename T, typename U>
bool operator==(const MemoryTraceAllocator<T>&, const MemoryTraceAllocator<U>&)
{
    return true;
}

template<typename T, typename U>
bool operator!=(const MemoryTraceAllocator<T>&, const MemoryTraceAllocator<U>&)
{
    return false;
}

#define COLOR_SCOPED ColorScoped __color_scope##__LINE__;
#define PROFILE_SCOPE_AUTO \
    ZoneScoped;            \
    COLOR_SCOPED ZoneColor(GetColor());
#define PROFILE_SCOPE(name) \
    ZoneScopedN(name);      \
    COLOR_SCOPED ZoneColor(GetColor());
#define MARK_FRAME(name) FrameMarkNamed(name)
#define MARK_FRAME_AUTO FrameMark
#define SEND_MESSAGE_TO_PROFILER(txt, size) TracyMessage(txt, size)
#else
#define PROFILE_SCOPE_AUTO
#define PROFILE_SCOPE(name)
#define MARK_FRAME(name)
#define MARK_FRAME_AUTO
#define SEND_MESSAGE_TO_PROFILER(txt, size)
#endif
