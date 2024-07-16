/**
* 一些重要的定义
*/
#ifndef ELBOWENGINE_COREDEFS_H
#define ELBOWENGINE_COREDEFS_H
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

// std::cref -> MakeConstRef
template<typename T>
Ref<const T> MakeConstRef(const T& TValue) {
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

// std::map -> Map
#include <map>
template<
    class KeyType, class ValueType, class Comparator = std::less<KeyType>,
    class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using Map = std::map<KeyType, ValueType, Comparator, Allocator>;

// basic typedefs
#include <cstdint>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// std::wstring -> String
// unicode字符串,此项目中全部使用此字符串
// std::string用于编写生成器
#include <string>
typedef std::wstring String;

// 定义反射宏
#include "rttr/policy.h"
#include "rttr/registration"
#include "rttr/registration_friend"
#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define CONCAT_IMPL3(x, y, z) x##y##z
#define MACRO_CONCAT3(x, y, z) CONCAT_IMPL3(x, y, z)
#define REGISTER_CLASS_NAME(FileID, LineNumber) \
    MACRO_CONCAT3(Z_Auto_Register_Class_, FileID, LineNumber)
#define REGISTER_FUNCTION_NAME(FileID, LineNumber) \
    MACRO_CONCAT3(Z_Auto_Register_Function_, FileID, LineNumber)
#define REGISTER_FIELD_NAME(FieldID, LineNumber) \
    MACRO_CONCAT3(Z_Auto_Register_Field_, FieldID, LineNumber)
#define REGISTRATION_IN_CLASS(FileID, LineNumber)               \
private:                                                        \
    struct REGISTER_CLASS_NAME(FileID, LineNumber)              \
    {                                                           \
        REGISTER_CLASS_NAME(FileID, LineNumber)                 \
        () {                                                    \
            REGISTER_FUNCTION_NAME(FileID, LineNumber)          \
            ();                                                 \
        }                                                       \
    };                                                          \
    static inline const REGISTER_CLASS_NAME(FileID, LineNumber) \
        REGISTER_FIELD_NAME(FileID, LineNumber){};              \
    static void REGISTER_FUNCTION_NAME(FileID, LineNumber)()

// GENERATED_BODY定义
#define GENRATED_BODY_COMBINE_IMPL(A, B, C, D) A##B##C##D
#define GENERATED_BODY_COMBINE_IMPL(A, B, C, D) GENRATED_BODY_COMBINE_IMPL(A, B, C, D)
#ifndef REFLECTION
#    define GENERATED_BODY(ClassName) \
        GENERATED_BODY_COMBINE_IMPL(GENERATED_BODY_, CURRENT_FILE_ID, _, ClassName)
#else
#    define GENERATED_BODY(...)
#endif
#ifndef REFLECTION
#    define PROPERTY(...)
#    define REFL
#else
#    define PROPERTY(...) __attribute__((annotate("Reflected, " #__VA_ARGS__)))
#    define REFL __attribute__((annotate("Reflected, ")))
#	 define FUNCTION(...) __attribute__((annotate("Reflected, " #__VA_ARGS__)))
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
    static void rttr_auto_register_reflection_function_##EnumName()

#define CONCAT_IMPL4(a, b, c, d) a##b##c##d

#define GENERATED_ENUM_IMPL(EnumName, FileID)                   \
    ENUM_REGISTRATION(##EnumName) {                             \
        CONCAT_IMPL4(GENERATED_ENUM_CODE_, FileID, _, EnumName) \
    }

#define GENERATED_ENUM(EnumName) GENERATED_ENUM_IMPL(EnumName, CURRENT_FILE_ID)


#endif
