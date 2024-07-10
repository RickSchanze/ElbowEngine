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
#include "Singleton/Singleton.h"

extern Logger gLogger;

// 对Log函数进行宏定义
#define STRINGIFY(x) #x
#define LSTRINGIFY(x) L#x

#define LOG_INFO(Text, ...) gLogger.Info(L##Text, __VA_ARGS__)
#define LOG_INFO_CATEGORY(Category, Text, ...) gLogger.Info(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)

#define LOG_WARNING(Text, ...) gLogger.Warning(L##Text, __VA_ARGS__)
#define LOG_WARNING_CATEGORY(Category, Text, ...) gLogger.Warning(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)

#define LOG_ERROR(Text, ...) gLogger.Error(L##Text, __VA_ARGS__)
#define LOG_ERROR_CATEGORY(Category, Text, ...) gLogger.Error(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)

#define LOG_CRITIAL(Text, ...) gLogger.Critical(L##Text, __VA_ARGS__)
#define LOG_CRITIAL_CATEGORY(Category, Text, ...) gLogger.Critical(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)

#ifdef ELBOW_DEBUG
#    define LOG_DEBUG(Text, ...) gLogger.Debug(L##Text, __VA_ARGS__)
#    define LOG_DEBUG_CATEGORY(Category, Text, ...) gLogger.Debug(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#    define LOG_TRACE(Text, ...) gLogger.Debug(L##Text, __VA_ARGS__)
#    define LOG_TRACE_CATEGORY(Category, Text, ...) gLogger.Debug(L"[" LSTRINGIFY(Category) L"] " Text, __VA_ARGS__)
#else
#    define LOG_DEBUG(Text, ...)
#    define LOG_DEBUG_CATEGORY(Category, Text, ...)
#    define LOG_TRACE(Text, ...)
#    define LOG_TRACE_CATEGORY(Category, Text, ...)
#endif

#define ASSERTF(Condition, Message) \
    if (Condition) LOG_CRITIAL(Message)

#define ASSERTF_CATEGORY(Category, Condition, Message) \
    if (Condition) LOG_CRITIAL_CATEGORY(Category, Message)

#define ASSETC

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
    using Type = TSharedPtr<T>;
};

template<typename T>
struct NewReturnType<T, ENewReturnType::UniquePtr>
{
    using Type = TUniquePtr<T>;
};

// 使用这个可以不在头文件引入ObjectManager.h
class ObjectCreateHelper
{
public:
    // 获得可用的ID
    static uint32_t GetAvailableID();
    static void     SetObjectID(Object* Obj, uint32_t ID);
    static void     SetObjectID(const TSharedPtr<Object>& Obj, uint32_t ID);
    static void     SetObjectID(const TUniquePtr<Object>& Obj, uint32_t ID);
};

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
typename NewReturnType<T, Strategy>::Type New(const String& name = L"", Args&&... args)
{
    // T不能是个单例
    static_assert(!std::is_base_of_v<Singleton<T>, T>, "T can not be a singleton.");
    int32_t available_id = ObjectCreateHelper::GetAvailableID();

    typename NewReturnType<T, Strategy>::Type rtn;
    if constexpr (Strategy == ENewReturnType::Raw)
    {
        rtn = new T(Forward<Args>(args)...);
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
    ObjectManager::Get().AddObject(rtn);
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
        int32_t draw_calls = 0;
    } graphics;

    float    time_delta                  = 0;
    uint64_t frame_count                 = 0;
    int32_t  fps                         = 0;       // 帧率
    bool     is_hide_mouse               = false;   // 是否隐藏鼠标
    int32_t  object_count                = 0;       // 当前总对象数
    int32_t  parallel_render_frame_count = 2;       // 同时渲染的帧数
    int32_t  swapchain_image_count       = -1;      // 交换链图像数量
    uint32_t current_image_index         = 0;       // 当前交换链索引
    int32_t  current_frame_index         = 0;       // 当前渲染帧索引

    // current_frame_index++
    void IncreaseFrameIndex();
    void ResetDrawCalls();
    void IncreaseDrawCall(uint32_t count = 1);
};

extern EngineStatistics g_engine_statistics;

extern String STRING_NONE;
