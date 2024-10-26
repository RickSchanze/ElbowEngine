/**
 * @file CoreGlobal.h
 * @author Echo
 * @Date 24-4-7
 * @brief
 */

#pragma once
#include "Log/Logger.h"
#include "Object/Object.h"
#include "Profiler/ProfileMacro.h"
#include "Singleton/Singleton.h"


template<typename T, typename... Args>
T* New(Args&&... args)
{
#ifdef ENABLE_PROFILING
    MemoryTraceAllocator<T> allocator;
    T*                      ptr = allocator.allocate(1);   // 分配 1 个对象的内存
    try
    {
        new (ptr) T(std::forward<Args>(args)...);   // 调用构造函数
    }
    catch (...)
    {
        allocator.deallocate(ptr, 1);
        throw;
    }
    return ptr;
#else
    return new T(std::forward<Args>(args)...);
#endif
}

template<typename T>
void Delete(T* ptr)
{
#ifdef ENABLE_PROFILING
    if (ptr)
    {
        ptr->~T();   // 调用析构函数
        MemoryTraceAllocator<T> allocator;
        allocator.deallocate(ptr, 1);   // 释放内存
    }
#else
    delete ptr;
#endif
}

// 引擎数据
struct EngineStatistics
{
    struct
    {
        int32_t width  = 0;
        int32_t height = 0;
    } window_size;

    struct
    {
        int32_t  draw_calls                             = 0;
        uint32_t max_dynamic_model_uniform_buffer_count = 20;   // 最大动态模型矩阵uniform buffer数量 @see ShaderProgram.cpp
        int32_t  swapchain_image_count                  = 3;    // 交换链图像数量
        int32_t  parallel_render_frame_count            = 2;    // 同时渲染的帧数
        int32_t  max_point_light_count                  = 4;    // 同一场景最大能拥有的直射光的数量
    } graphics;

    float    time_delta          = 0;
    uint64_t frame_count         = 0;
    int32_t  fps                 = 0;       // 帧率
    bool     is_hide_mouse       = false;   // 是否隐藏鼠标
    int32_t  object_count        = 0;       // 当前总对象数
    uint32_t current_image_index = 0;       // 当前交换链索引
    int32_t  current_frame_index = 0;       // 当前渲染帧索引

    // current_frame_index++
    void IncreaseFrameIndex();
    void ResetDrawCalls();
    void IncreaseDrawCall(int32_t count = 1);
};

// 编译时获得常量字符串长度
template<std::size_t N>
constexpr std::size_t STRLEN(const char (&str)[N])
{
    return N - 1;   // 字符串字面量包含结尾的 '\0'，所以减去 1
}

// 获取枚举字符串的函数
template<typename EnumType>
    requires std::is_enum_v<EnumType>
constexpr const char* GetEnumString(EnumType type)
{
    Type              t           = TypeOf<EnumType>();
    rttr::enumeration enumeration = t.get_enumeration();

    return enumeration.value_to_name(type).data();
}

template<typename EnumType>
constexpr int GetEnumValue(EnumType t)
{
    return static_cast<int>(t);
}

extern EngineStatistics g_engine_statistics;

float GetFrameTime();

#define ENUM_OUT_OF_RANGE "OutOfRange"
#define ENUM_INVALID "Invalid"

namespace core
{
inline size_t GetCurrentThreadId()
{
    return ::GetCurrentThreadId();
}
}
