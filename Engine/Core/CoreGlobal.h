/**
 * @file CoreGlobal.h
 * @author Echo
 * @Date 24-4-7
 * @brief
 */

#pragma once
#include "Assert.h"
#include "Core/Log/Logger.h"
#include "CoreDef.h"

namespace core
{
class MemoryPool;
}

void*             _MemoryPoolAllocate(core::MemoryPool* pool, size_t size);
void              _MemoryPoolFree(core::MemoryPool* pool, void* p);
core::MemoryPool* _GetMemoryPool(Int32 id);

template <typename T, typename... Args>
T* NewPooled(core::MemoryPool* pool, Args&&... args)
{
    const size_t size = sizeof(T);
    T*           p    = static_cast<T*>(_MemoryPoolAllocate(pool, size));
    if (p)
    {
        new (p) T(std::forward<Args>(args)...);
    }
    return p;
}

template <typename T, typename... Args>
T* NewPooledId(const Int32 id, Args&&... args)
{
    auto pool = _GetMemoryPool(id);
    return NewPooled<T>(pool, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
T* New(Args&&... args)
{
    return NewPooledId<T>(0, std::forward<Args>(args)...);
}

template <typename T>
void DeletePooled(core::MemoryPool* pool, T* p)
{
    p->~T();
    _MemoryPoolFree(pool, p);
}

template <typename T>
void DeletePooledId(const Int32 id, T* p)
{
    auto pool = _GetMemoryPool(id);
    DeletePooled(pool, p);
}

template <typename T>
void Delete(T* p)
{
    DeletePooledId(0, p);
}

// 编译时获得常量字符串长度
template <std::size_t N>
constexpr std::size_t STRLEN(const char (&str)[N])
{
    return N - 1;   // 字符串字面量包含结尾的 '\0'，所以减去 1
}

namespace core
{
inline size_t GetCurrentThreadId()
{
    return ::GetCurrentThreadId();
}
}   // namespace core

enum class RuntimeStage
{
    Startup,
    Running,
    Shutdown,
    Count,
};

inline auto         _run_time_stage = RuntimeStage::Count;
inline RuntimeStage GetRuntimeStage()
{
    return _run_time_stage;
}
inline void SetRuntimeStage(const RuntimeStage stage) {_run_time_stage = stage;}
