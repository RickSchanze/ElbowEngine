/**
 * @file CoreGlobal.h
 * @author Echo
 * @Date 24-4-7
 * @brief
 */

#pragma once

#include "Log/Logger.h"
#include "Object/Object.h"
#include "Object/ObjectManager.h"
#include "Path/Path.h"
#include "Profiler/ProfileMacro.h"
#include "Singleton/Singleton.h"

extern Logger g_logger;

// 对Log函数进行宏定义
#define STRINGIFY(x) #x
#define LSTRINGIFY(x) L#x

#define LOG_INFO(Text, ...) g_logger.Info(L##Text, __VA_ARGS__)
#define LOG_INFO_CATEGORY(Category, Text, ...) g_logger.Info(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#define LOG_INFO_ANSI(Text, ...) g_logger.InfoAnsi(##Text, __VA_ARGS__)
#define LOG_INFO_ANSI_CATEGORY(Category, Text, ...) g_logger.InfoAnsi("[" #Category "] " Text, __VA_ARGS__)

#define LOG_WARNING(Text, ...) g_logger.Warning(L##Text, __VA_ARGS__)
#define LOG_WARNING_CATEGORY(Category, Text, ...) g_logger.Warning(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#define LOG_WARNING_ANSI(Text, ...) g_logger.WarningAnsi(##Text, __VA_ARGS__)
#define LOG_WARNING_ANSI_CATEGORY(Category, Text, ...) g_logger.WarningAnsi("[" #Category "] " Text, __VA_ARGS__)

#define LOG_ERROR(Text, ...) g_logger.Error(L##Text, __VA_ARGS__)
#define LOG_ERROR_CATEGORY(Category, Text, ...) g_logger.Error(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#define LOG_ERROR_ANSI(Text, ...) g_logger.ErrorAnsi(##Text, __VA_ARGS__)
#define LOG_ERROR_CATEGORY_ANSI(Category, Text, ...) g_logger.ErrorAnsi("[" #Category "] " Text, __VA_ARGS__)

#define LOG_ERROR_NO_BREAK(Text, ...) g_logger.ErrorNoBreak(L##Text, __VA_ARGS__)
#define LOG_ERROR_CATEGORY_NO_BREAK(Category, Text, ...) g_logger.ErrorNoBreak(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#define LOG_ERROR_ANSI_NO_BREAK(Text, ...) g_logger.ErrorNoBreakAnsi(##Text, __VA_ARGS__)
#define LOG_ERROR_ANSI_CATEGORY_NO_BREAK(Category, Text, ...) g_logger.ErrorNoBreakAnsi("[" #Category "] " Text, __VA_ARGS__)

#define LOG_CRITIAL(Text, ...) g_logger.Critical(L##Text, __VA_ARGS__)
#define LOG_CRITIAL_CATEGORY(Category, Text, ...) g_logger.Critical(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#define LOG_CRITIAL_ANSI(Text, ...) g_logger.CriticalAnsi(##Text, __VA_ARGS__)
#define LOG_CRITIAL_ANSI_CATEGORY(Category, Text, ...) g_logger.CriticalAnsi("[" #Category "] " Text, __VA_ARGS__)


#define LOG_DEBUG(Text, ...) g_logger.Debug(L##Text, __VA_ARGS__)
#define LOG_DEBUG_CATEGORY(Category, Text, ...) g_logger.Debug(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#define LOG_TRACE(Text, ...) g_logger.Debug(L##Text, __VA_ARGS__)
#define LOG_TRACE_CATEGORY(Category, Text, ...) g_logger.Debug(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)

#define Assert(Category, Condition, Message, ...) \
    if (!(Condition)) LOG_CRITIAL_CATEGORY(Category, Message, __VA_ARGS__)

#if ELBOW_DEBUG
#define DebugAssert(Category, Condition, Message, ...) \
    if (!(Condition)) LOG_ERROR_CATEGORY_ANSI(Category, Message, __VA_ARGS__)
#else
#define DebugAssert(Category, Condition, Message, ...)
#endif

#define NEVER_ENTRY_CRITICAL() g_logger.CriticalAnsi("This function should never be executed {}:{}, {}", __FILE__, __LINE__, __FUNCSIG__);
#define NEVER_ENTRY_WARNING() g_logger.WarningAnsi("This function should never be executed {}:{}, {}", __FILE__, __LINE__, __FUNCSIG__);

/** BEGIN IsValid函数族 */
inline bool IsValid(Object* Obj)
{
    return Obj != nullptr && Obj->IsValid();
}

inline bool IsValid(const Object* Obj)
{
    return Obj != nullptr && Obj->IsValid();
}

template<typename T>
bool IsValid(T) = delete;
/** END IsValid函数族 */

enum class ENewReturnType
{
    Raw,
    SharedPtr,
    UniquePtr
};

template<typename T, ENewReturnType Strategy>
struct NewReturnType;

template<typename T>
struct NewReturnType<T, ENewReturnType::Raw>
{
    using Type = T*;
};

template<typename T>
struct NewReturnType<T, ENewReturnType::SharedPtr>
{
    using Type = SharedPtr<T>;
};

template<typename T>
struct NewReturnType<T, ENewReturnType::UniquePtr>
{
    using Type = UniquePtr<T>;
};

// 使用这个可以不在头文件引入ObjectManager.h
class ObjectCreateHelper
{
public:
    // 获得可用的ID
    static uint32_t GetAvailableID();
    static void     SetObjectID(Object* Obj, uint32_t ID);
    static void     SetObjectID(const SharedPtr<Object>& Obj, uint32_t ID);
    static void     SetObjectID(const UniquePtr<Object>& Obj, uint32_t ID);
};

template<typename T, typename... Args>
T* New(Args&&... args)
{
#if ENABLE_PROFILING
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
#if ENABLE_PROFILING
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

/**
 * 新建一个对象，自动选择合适的ID
 * @tparam T 对象类型 必须是Object的子类
 * @tparam Strategy 对象创建策略
 * ReturnRaw: 返回裸指针
 * ReturnSharedPtr: 返回SharedPtr
 * ReturnUniquePtr: 返回UniquePtr
 * 默认为ReturnRaw
 * @param name 对象名称
 * @return
 */
template<typename T, ENewReturnType Strategy = ENewReturnType::Raw, typename... Args>
    requires IsObject<T>
typename NewReturnType<T, Strategy>::Type NewObject(const String& name = L"", Args&&... args)
{
    // T不能是个单例
    static_assert(!std::is_base_of_v<Singleton<T>, T>, "T can not be a singleton.");
    int32_t available_id = ObjectCreateHelper::GetAvailableID();

    typename NewReturnType<T, Strategy>::Type rtn;
    if constexpr (Strategy == ENewReturnType::Raw)
    {
        rtn = New<T>(Forward<Args>(args)...);
    }
    else if constexpr (Strategy == ENewReturnType::SharedPtr)
    {
        rtn = MakeShared<T>(Forward<Args>(args)...);
    }
    else if constexpr (Strategy == ENewReturnType::UniquePtr)
    {
        rtn = MakeUnique<T>(Forward<Args>(args)...);
    }
    else
    {
        return nullptr;
    }
    if (rtn->GetName().empty())
    {
        if (name.empty())
        {
            rtn->SetName(std::vformat(L"Object_{}", std::make_wformat_args(available_id)));
        }
        else
        {
            rtn->SetName(name);
        }
    }
    ObjectCreateHelper::SetObjectID(rtn, available_id);
    ObjectManager::Get()->AddObject(rtn);
    return rtn;
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
    void IncreaseDrawCall(const int32_t count = 1);
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

template <typename EnumType>
constexpr int GetEnumValue(EnumType t)
{
    return static_cast<int>(t);
}

extern EngineStatistics g_engine_statistics;

extern String STRING_NONE;

float GetFrameTime();
