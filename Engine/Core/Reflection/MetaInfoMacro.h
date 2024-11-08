/**
 * @file MetaInfoMacro.h
 * @author Echo 
 * @Date 24-10-31
 * @brief 
 */

#pragma once

// 反射宏
#ifdef REFLECTION
// 宏魔法由gpt-4o倾情提供
// Helper macros to handle variable arguments
#define EXPAND(x) x
#define CONCATENATE(arg1, arg2) CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2) CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2) arg1##arg2

// Apply a macro to each argument passed
#define APPLY(macro, ...) EXPAND(CONCATENATE(APPLY_, COUNT(__VA_ARGS__))(macro, __VA_ARGS__))
#define APPLY_1(m, x) m(x)
#define APPLY_2(m, x, ...) m(x) EXPAND(APPLY_1(m, __VA_ARGS__))
#define APPLY_3(m, x, ...) m(x) EXPAND(APPLY_2(m, __VA_ARGS__))
#define APPLY_4(m, x, ...) m(x) EXPAND(APPLY_3(m, __VA_ARGS__))
#define APPLY_5(m, x, ...) m(x) EXPAND(APPLY_4(m, __VA_ARGS__))
// Extend as needed

// Count the number of arguments
#define COUNT(...) COUNT_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
#define COUNT_IMPL(_1, _2, _3, _4, _5, N, ...) N

#define CLASS(...) APPLY(__cppast, Reflection, __VA_ARGS__)
#define STRUCT(...) APPLY(__cppast, Reflection, __VA_ARGS__)
#define PROPERTY(...) APPLY(__cppast, Reflection, __VA_ARGS__)
#define FUNCTION(...) APPLY(__cppast, Reflection, __VA_ARGS__)
#define ENUM(...) APPLY(__cppast, Reflection, __VA_ARGS__)
#define VALUE(...) APPLY(__cppast, Reflection, __VA_ARGS__)
#else
#define CLASS(...)
#define STRUCT(...)
#define PROPERTY(...)
#define FUNCTION(...)
#define ENUM(...)
#define VALUE(...)
#endif

#define CONCAT2(a, b) a##b
#define CONCAT3(a, b, c) a##b##c

#define TO_POINTER(type) type*

#define GENERATED_BODY(class_)                                               \
public:                                                                      \
    static core::Type* CONCAT3(REFLECTION_Register_, class_, _Registerer)(); \
    typedef ThisClass  Super;                                                \
    typedef class_     ThisClass;                                            \
                                                                             \
    [[nodiscard]] const core::Type* GetType() const override                 \
    {                                                                        \
        return core::TypeOf<class_>();                                       \
    }                                                                        \
    CONCAT2(GENERATED_BODY_IMPL_, class_)                                    \
                                                                             \
private:


#define GENERATED_SOURCE()                                         \
    static void Z_MetaInfo_Registration_Func();                    \
    namespace                                                      \
    {                                                              \
    struct Z_MetaInfo_Registration                                 \
    {                                                              \
        Z_MetaInfo_Registration()                                  \
        {                                                          \
            Z_MetaInfo_Registration_Func();                        \
        }                                                          \
    };                                                             \
    static const Z_MetaInfo_Registration Z_meta_info_registration; \
    }                                                              \
    void Z_MetaInfo_Registration_Func(){GENERATED_ALL_CLASS_BODY} GENERATED_IMPLEMENTATION

