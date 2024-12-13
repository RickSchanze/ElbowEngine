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
#include "Log/CoreLogCategory.h"

enum class MemoryAllocType
{
    Normal,      // 正常分配
    FrameTemp,   // 使用FrameAllocator分配
};

void* FrameTempAlloc(size_t size);

void* NormalAlloc(size_t size);
void  FreeNormalAllocatedMemory(void* ptr);

template <typename T, typename... Args>
T* New(Args&&... args)
{
    size_t size = sizeof(T);
    void*  t    = NormalAlloc(size);
    core::Assert::Require(logcat::Core_Memory, t, "Memory allocation failed!");
    return new (t) T(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
T* NewFrameTemp(Args&&... args)
{
    size_t size = sizeof(T);
    void*  t    = FrameTempAlloc(size);
    core::Assert::Require(logcat::Core_Memory, t, "Memory allocation failed!");
    return new (t) T(std::forward<Args>(args)...);
}

template <typename T, MemoryAllocType type = MemoryAllocType::Normal>
void Delete(T* ptr)
{
    static_cast<T*>(ptr)->~T();
    if constexpr (type == MemoryAllocType::Normal)
    {
        FreeNormalAllocatedMemory(ptr);
    }
}

// 编译时获得常量字符串长度
template <std::size_t N>
constexpr std::size_t STRLEN(const char (&str)[N])
{
    return N - 1;   // 字符串字面量包含结尾的 '\0'，所以减去 1
}

float GetFrameTime();

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
