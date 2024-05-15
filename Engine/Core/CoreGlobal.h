/**
 * @file CoreGlobal.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#pragma once

#include "Log/Logger.h"
#include "Object/Object.h"
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

/** BEGIN IsValid函数族 */
inline bool IsValid(Object* Obj) {
    return Obj != nullptr && Obj->IsValid();
}

inline bool IsValid(const Object* Obj) {
    return Obj != nullptr && Obj->IsValid();
}

template<typename T>
bool IsValid(T) = delete;
/** END IsValid函数族 */

enum class ENewReturnType { Raw, SharedPtr, UniquePtr };

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
class ObjectCreateHelper {
public:
    // 获得可用的ID
    static uint32 GetAvailableID();
    static void   SetObjectID(Object* Obj, uint32 ID);
    static void   SetObjectID(const TSharedPtr<Object>& Obj, uint32 ID);
    static void   SetObjectID(const TUniquePtr<Object>& Obj, uint32 ID);
};

/**
 * 新建一个对象，自动选择合适的ID
 * @tparam T 对象类型 必须是Object的子类
 * @tparam Strategy 对象创建策略
 * ReturnRaw: 返回裸指针
 * ReturnSharedPtr: 返回SharedPtr
 * ReturnUniquePtr: 返回UniquePtr
 * 默认为ReturnRaw
 * @param Name 对象名称
 * @return
 */
template<typename T, ENewReturnType Strategy = ENewReturnType::SharedPtr>
    requires IsObject<T>
typename NewReturnType<T, Strategy>::Type New(const String& Name = L"") {
    // T不能是个单例
    static_assert(!std::is_base_of_v<Singleton<T>, T>, "T can not be a singleton.");
    uint32 AvailableID = ObjectCreateHelper::GetAvailableID();

    typename NewReturnType<T, Strategy>::Type Rtn;
    if constexpr (Strategy == ENewReturnType::Raw) {
        Rtn = new T();
    } else if constexpr (Strategy == ENewReturnType::SharedPtr) {
        Rtn = MakeShared<T>();
    } else if constexpr (Strategy == ENewReturnType::UniquePtr) {
        Rtn = MakeUnique<T>();
    } else {
        return nullptr;
    }
    if (Name.empty()) {
        Rtn->SetName(std::vformat(L"Object_{}", std::make_wformat_args(AvailableID)));
    } else {
        Rtn->SetName(Name);
    }
    ObjectCreateHelper::SetObjectID(Rtn, AvailableID);
    return Rtn;
}
