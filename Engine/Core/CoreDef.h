/**
* 一些重要的定义
*/
#pragma once

// 一些Typedef
#include <functional>

// std::reference_wrapper -> Ref
template<typename T>
using Ref = std::reference_wrapper<T>;

// std::ref -> MakeRef
template<typename T>
Ref<T> MakeRef(T& TValue) {
    return std::ref(TValue);
}

template<typename T>
using ConstRef = std::reference_wrapper<const T>;

// std::cref -> MakeConstRef
template<typename T>
ConstRef<T> MakeConstRef(const T& TValue) {
    return std::cref(TValue);
}

// std::function -> Function
template<typename T>
using TFunction = std::function<T>;

// std::vector -> Array
#include <vector>
template<typename T, typename Allocator = std::allocator<T>>
using TArray = std::vector<T, Allocator>;

// std::array -> StaticArray
#include <array>
template<typename T, size_t Size>
using TStaticArray = std::array<T, Size>;

// std::set -> Set
#include <set>
template<class KeyType, class Comparator = std::less<KeyType>, class Allocator = std::allocator<KeyType>>
using TSet = std::set<KeyType, Comparator, Allocator>;

// std::map -> Map
#include <map>
template<
    class KeyType, class ValueType, class Comparator = std::less<KeyType>,
    class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using TMap = std::map<KeyType, ValueType, Comparator, Allocator>;

// basic typedefs
#include <cstdint>
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// std::wstring -> String
// unicode字符串,此项目中全部使用此字符串
// std::string用于编写生成器
#include <string>
typedef std::wstring String;
typedef std::string  AnsiString;

// std::optional -> Optional
#include <optional>
template<typename T>
using TOptional = std::optional<T>;

// std::shared_ptr -> SharedPtr
#include <memory>
template<typename T>
using TSharedPtr = std::shared_ptr<T>;
// std::unique_ptr -> UniquePtr
template<typename T>
using TUniquePtr = std::unique_ptr<T>;
// std::weak_ptr -> WeakPtr
template<typename T>
using TWeakPtr = std::weak_ptr<T>;
// std::make_shared -> MakeShared
template<typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
// std::make_unique -> MakeUnique
template<typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// std::forward -> Forward
template<typename T>
constexpr T&& Forward(std::remove_reference_t<T>& Arg) noexcept {
    return static_cast<T&&>(Arg);
}

// std::move -> Move
template<typename T>
constexpr std::remove_reference_t<T>&& Move(T&& Arg) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(Arg);
}

// std::unordered_map -> HashMap
#include <unordered_map>
template<
    class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>,
    class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using THashMap = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, Allocator>;

template<typename T>
constexpr T&& Forward(std::remove_reference_t<T>&& Arg) noexcept {
    static_assert(!std::is_lvalue_reference_v<T>, "Can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(Arg);
}

// std::dynamic_pointer_cast -> StaticPointerCast
template<typename T, typename U>
TSharedPtr<T> StaticPointerCast(const TSharedPtr<U>& InSharedPtr) {
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
typedef rttr::type     Type;
typedef rttr::property Property;

template<typename T>
Type GetType() {
    return rttr::type::get<T>();
}

// 定义反射宏
#include "rttr/policy.h"
#include "rttr/registration_friend"
#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define CONCAT_IMPL3(x, y, z) x##y##z
#define MACRO_CONCAT3(x, y, z) CONCAT_IMPL3(x, y, z)

#define GENERATED_SOURCE()                               \
    RTTR_REGISTRATION {                                  \
        MACRO_CONCAT(GENERATED_SOURCE_, CURRENT_FILE_ID) \
    }

// GENERATED_BODY定义
#define GENRATED_BODY_COMBINE_IMPL(A, B, C, D) A##B##C##D
#define GENERATED_BODY_COMBINE_IMPL(A, B, C, D) GENRATED_BODY_COMBINE_IMPL(A, B, C, D)
#ifndef REFLECTION
#    define GENERATED_BODY(ClassName) GENERATED_BODY_COMBINE_IMPL(GENERATED_BODY_, CURRENT_FILE_ID, _, ClassName)
#else
#    define GENERATED_BODY(...)
#endif
#ifndef REFLECTION
#    define PROPERTY(...)
#    define REFL
#    define FUNCTION(...)
#else
#    define PROPERTY(...) __attribute__((annotate("Reflected, " #__VA_ARGS__)))
#    define REFL __attribute__((annotate("Reflected, ")))
#endif

// GENERATED_ENUM定义
#define ENUM_REGISTRATION(EnumName)                                                    \
    static void rttr_auto_register_reflection_function_##EnumName();                   \
    namespace {                                                                        \
    struct rttr__auto__register__##EnumName                                            \
    {                                                                                  \
        rttr__auto__register__##EnumName() {                                           \
            rttr_auto_register_reflection_function_##EnumName();                       \
        }                                                                              \
    };                                                                                 \
    }                                                                                  \
    static const rttr__auto__register__##EnumName RTTR_CAT(auto_register__, __LINE__); \
    static void                                   rttr_auto_register_reflection_function_##EnumName()

#define CONCAT_IMPL4(a, b, c, d) a##b##c##d

#define GENERATED_ENUM_IMPL(EnumName, FileID)                   \
    ENUM_REGISTRATION(##EnumName) {                             \
        CONCAT_IMPL4(GENERATED_ENUM_CODE_, FileID, _, EnumName) \
    }

#define GENERATED_ENUM(EnumName) GENERATED_ENUM_IMPL(EnumName, CURRENT_FILE_ID)


// // 获取当前调用栈的宏
#include "cpptrace/cpptrace.hpp"
#define GENERATE_STACKTRACE()                                     \
    const auto CurrentStackTrace    = cpptrace::generate_trace(); \
    auto       CurrentStackTraceStr = CurrentStackTrace.to_string();

// 表示一个接口
#define interface class

// 表示这个参数是一个输出参数
#define OUT

// 这个宏表示这个方法必须由子类实现
#define INTERFACE_METHOD = 0;

#define _CRT_SECURE_NO_WARNINGS

// 项目Debug宏
#define ELBOW_DEBUG _DEBUG || RELWITHDEBINFO

#define U8(Text) (const char*)u8## Text
