/**
* 一些重要的定义
*/
#pragma once

#include <chrono>
#include <ranges>

#include "CoreMacro.h"

// 一些Typedef
#include "Profiler/ProfileMacro.h"


#include <functional>

// std::reference_wrapper -> Ref
template<typename T>
using Ref = std::reference_wrapper<T>;

// std::ref -> MakeRef
template<typename T>
Ref<T> MakeRef(T& TValue)
{
    return std::ref(TValue);
}

template<typename T>
using ConstRef = std::reference_wrapper<const T>;

// std::cref -> MakeConstRef
template<typename T>
ConstRef<T> MakeConstRef(const T& TValue)
{
    return std::cref(TValue);
}

// std::function -> Function
template<typename T>
using Function = std::function<T>;

// std::vector -> Array
#include <vector>
template<typename T, typename Allocator = std::allocator<T>>
using Array = std::vector<T, Allocator>;

// std::array -> StaticArray
#include <array>
template<typename T, size_t Size>
using StaticArray = std::array<T, Size>;

// std::set -> Set
#include <set>
template<class KeyType, class Comparator = std::less<KeyType>, class Allocator = std::allocator<KeyType>>
using Set = std::set<KeyType, Comparator, Allocator>;

// std::map -> Map
#include <map>
template<class KeyType, class ValueType, class Comparator = std::less<KeyType>, class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using Map = std::map<KeyType, ValueType, Comparator, Allocator>;

#include <list>
template<typename T, typename Allocator = std::allocator<T>>
using List = std::list<T, Allocator>;

// std::wstring -> String
// unicode字符串,此项目中全部使用此字符串
// std::string用于编写生成器
#include <string>
typedef std::wstring String;
typedef std::string  AnsiString;

// std::optional -> Optional
#include <optional>
template<typename T>
using Optional = std::optional<T>;

// std::shared_ptr -> SharedPtr
#include <memory>
template<typename T>
using SharedPtr = std::shared_ptr<T>;
// std::weak_ptr -> WeakPtr
template<typename T>
using WeakPtr = std::weak_ptr<T>;
// std::make_shared -> MakeShared
template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args)
{
#if ENABLE_PROFILING
    return std::allocate_shared<T>(MemoryTraceAllocator<T>(), std::forward<Args>(args)...);
#else
    return std::make_shared<T>(std::forward<Args>(args)...);
#endif
}

template<typename T>
class UniquePtr
{
public:
    // 构造函数
    template<typename... Args>
    static UniquePtr<T> Create(Args&&... args)
    {
#if ENABLE_PROFILING
        MemoryTraceAllocator<T> allocator;
        T*                      ptr = allocator.allocate(1);
        try
        {
            new (ptr) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            allocator.deallocate(ptr, 1);
            throw;
        }
#else
        T* ptr = new T(std::forward<Args>(args)...);
#endif
        return UniquePtr<T>(ptr);
    }

    // 默认构造函数
    UniquePtr() noexcept = default;

    // 构造函数
    UniquePtr(T* ptr) noexcept : ptr_(ptr) {}

    // 移动构造函数
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }

    // 移动赋值运算符
    UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        if (this != &other)
        {
            Reset();
            std::swap(ptr_, other.ptr_);
        }
        return *this;
    }

    // 子类到父类的转换
    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    UniquePtr(UniquePtr<U>&& other) noexcept : ptr_(other.Release()) {}

    // 删除拷贝构造函数和赋值运算符
                UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&)  = delete;

    // 析构函数
    ~UniquePtr() { Reset(); }

    // 重置指针
    void Reset(T* ptr = nullptr) noexcept
    {
        if (ptr_)
        {
#if ENABLE_PROFILING
            MemoryTraceDeleter<T>()(ptr_);
#else
            delete ptr_;
#endif
        }
        ptr_ = ptr;
    }

    // 释放指针
    T* Release() noexcept
    {
        T* temp = ptr_;
        ptr_    = nullptr;
        return temp;
    }

    // 获取指针
    T* Get() const noexcept { return ptr_; }

    // 解引用运算符
    T& operator*() const { return *ptr_; }

    // 成员访问运算符
    T* operator->() const noexcept { return ptr_; }

    // 检查指针是否为空
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    T* ptr_ = nullptr;
};

template<typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args)
{
    return UniquePtr<T>::Create(std::forward<Args>(args)...);
}

// std::forward -> Forward
template<typename T>
constexpr T&& Forward(std::remove_reference_t<T>& Arg) noexcept
{
    return static_cast<T&&>(Arg);
}

// std::move -> Move
template<typename T>
constexpr std::remove_reference_t<T>&& Move(T&& Arg) noexcept
{
    return static_cast<std::remove_reference_t<T>&&>(Arg);
}

// std::unordered_map -> HashMap
#include <unordered_map>
template<
    class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>,
    class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using HashMap = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, Allocator>;

// std::unordered_set -> HashSet
#include <unordered_set>
template<class KeyType, class Hasher = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>, class Alloc = std::allocator<KeyType>>
using HashSet = std::unordered_set<KeyType, Hasher, KeyEqual, Alloc>;

template<typename T>
constexpr T&& Forward(std::remove_reference_t<T>&& Arg) noexcept
{
    static_assert(!std::is_lvalue_reference_v<T>, "Can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(Arg);
}

// std::dynamic_pointer_cast -> StaticPointerCast
template<typename T, typename U>
SharedPtr<T> StaticPointerCast(const SharedPtr<U>& InSharedPtr)
{
    return std::static_pointer_cast<T>(InSharedPtr);
}

// std::wstring_view -> StringView
#include <string_view>
typedef std::wstring_view StringView;
typedef std::string_view  AnsiStringView;

// Stream typedefs
#include <fstream>
#include <sstream>
typedef std::wostream      OutputStream;
typedef std::wistream      InputStream;
typedef std::wstringstream StringStream;

typedef std::ostream      AnsiOutputStream;
typedef std::istream      AnsiInputStream;
typedef std::stringstream AnsiStringStream;
typedef std::ofstream     FileOutputStream;
typedef std::ifstream     FileInputStream;

// 反射相关类的定义
#include "rttr/registration"
#include "rttr/argument.h"
typedef rttr::type     Type;
typedef rttr::property Property;

template<typename T>
Type TypeOf()
{
    return rttr::type::get<T>();
}

// 定义反射宏
#include "rttr/policy.h"
#include "rttr/registration_friend"
#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define CONCAT_IMPL3(x, y, z) x##y##z
#define MACRO_CONCAT3(x, y, z) CONCAT_IMPL3(x, y, z)

#define GENERATED_BODY(class_name) MACRO_CONCAT3(GENERATED_BODY_, CURRENT_FILE_ID, class_name)

#define GENERATED_SOURCE()                               \
    RTTR_REGISTRATION                                    \
    {                                                    \
        MACRO_CONCAT(GENERATED_SOURCE_, CURRENT_FILE_ID) \
    }

// 反射宏
#define _CONCAT2(a, b) a##b
#define CONCAT2(a, b) _CONCAT2(a, b)
#ifdef REFLECTION
#define ECLASS(...) extern void CONCAT2(REFLECTION_CLASS_TRAIT, __LINE__)(const char* param = #__VA_ARGS__);
#define ESTRUCT(...) extern void CONCAT2(REFLECTION_STRUCT_TRAIT, __LINE__)(const char* param = #__VA_ARGS__);
#define EPROPERTY(...) __attribute__((annotate("Property, " #__VA_ARGS__)))
#define EFUNCTION(...) __attribute__((annotate("Function, " #__VA_ARGS__)))
#define EENUM(...) extern void CONCAT2(REFLECTION_ENUM_TRAIT, __LINE__)(const char* param = #__VA_ARGS__);
#define EVALUE(...) __attribute__((annotate("Value, " #__VA_ARGS__)))
#else
#define ECLASS(...)
#define ESTRUCT(...)
#define EPROPERTY(...)
#define EFUNCTION(...)
#define EENUM(...)
#define EVALUE(...)
#endif

#if WITH_EDITOR
#define EDITOR_META(...) (__VA_ARGS__)
#else
#define EDITOR_META(...)
#endif

// 获取当前调用栈的宏
#include "cpptrace/cpptrace.hpp"
#define GENERATE_STACKTRACE()                                     \
    const auto CurrentStackTrace    = cpptrace::generate_trace(); \
    auto       CurrentStackTraceStr = CurrentStackTrace.to_string();

// 表示一个接口
#define interface class

// 表示这个参数是一个输出参数
#define OUT

// 这个宏表示这个方法必须由子类实现
#define INTERFACE_METHOD \
    {                    \
    }

#define _CRT_SECURE_NO_WARNINGS

// 项目Debug宏
#define ELBOW_DEBUG _DEBUG || RELWITHDEBINFO

#define U8(Text) (const char*)u8##Text

#define INTERFACE_IMPL(type)                                     \
    RTTR_REGISTRATION                                            \
    {                                                            \
        rttr::registration::class_<type>(#type).constructor<>(); \
}
