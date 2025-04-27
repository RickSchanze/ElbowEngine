//
// Created by Echo on 25-3-19.
//

#pragma once

#include <chrono>
#include <unordered_map> // for std::hash

#include "Assert.hpp"
#include "Core/Memory/New.hpp"
#include "CoreFeature.hpp"
#include "PlatformDef.hpp"
#include "Reflection/Reflection.hpp"

#ifdef _DEBUG
#define ELBOW_DEBUG
#endif

#ifdef ELBOW_DEBUG
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define DEBUG_ONLY(...)
#endif

#if WITH_EDITOR
#define EDITOR_ONLY(...) __VA_ARGS__
#else
#define EDITOR_ONLY(...)
#endif

#define CONCAT(x, y) x##y

#define REFLECTED_CLASS(_class)                                                                                                                      \
public:                                                                                                                                              \
    typedef ThisClass Super;                                                                                                                         \
    typedef _class ThisClass;                                                                                                                        \
    static void ConstructSelf(void *self) { new (self) _class(); }                                                                                   \
    static void DestructSelf(void *self) { static_cast<_class *>(self)->~_class(); }                                                                 \
    virtual const Type *GetType() const override { return TypeOf<_class>(); }                                                                        \
    static const Type *GetStaticType() { return TypeOf<_class, true>(); }                                                                            \
    static Type *ConstructType();                                                                                                                    \
                                                                                                                                                     \
private:

#define IMPL_REFLECTED_INPLACE(type)                                                                                                                 \
    struct BeforeMainTrigger_##type {                                                                                                                \
        BeforeMainTrigger_##type() { ReflManager::GetByRef().Register<type>(); }                                                                     \
    };                                                                                                                                               \
    static inline BeforeMainTrigger_##type beforeMainTrigger_##type;                                                                                 \
    inline Type *type::ConstructType()

#define IMPL_REFLECTED(type) Type *type::ConstructType()

#define REGISTER_TYPE(type)                                                                                                                          \
    struct BeforeMainTrigger_Register_##type {                                                                                                       \
        BeforeMainTrigger_Register_##type() { ReflManager::GetByRef().Register<type>(); }                                                            \
    };                                                                                                                                               \
    static inline BeforeMainTrigger_Register_##type beforeMainTrigger_Register_##type;

#define REGISTER_ENUM_TYPE(type)                                                                                                                     \
    struct BeforeMainTrigger_Register_##type {                                                                                                       \
        BeforeMainTrigger_Register_##type() { ReflManager::GetByRef().Register<type>(&ConstructType_##type); }                                       \
    };                                                                                                                                               \
    static inline BeforeMainTrigger_Register_##type beforeMainTrigger_Register_##type;

#define IMPL_ENUM_REFL(type) inline Type *ConstructType_##type()
#define DECL_ENUM_REFL(type) Type *ConstructType_##type();

#define EXEC_BEFORE_MAIN_(line)                                                                                                                      \
    static void BeforeMainExec();                                                                                                                    \
    struct BeforeMainGlobal_##line {                                                                                                                 \
        BeforeMainGlobal_##line() { BeforeMainExec(); }                                                                                              \
    };                                                                                                                                               \
    static inline BeforeMainGlobal_##line beforeMainGlobal_##line{};                                                                                 \
    inline void BeforeMainExec()

#define EXEC_BEFORE_MAIN() EXEC_BEFORE_MAIN_(__LINE__)

#define REFLECTED_SINGLE_CLASS(_class)                                                                                                               \
public:                                                                                                                                              \
    typedef _class ThisClass;                                                                                                                        \
    static void ConstructSelf(void *self) { new (self) _class(); }                                                                                   \
    static void DestructSelf(void *self) { static_cast<_class *>(self)->~_class(); }                                                                 \
    const Type *GetType() const { return TypeOf<_class>(); }                                                                                         \
    static const Type *GetStaticType() { return TypeOf<_class, true>(); }                                                                            \
    static Type *ConstructType();                                                                                                                    \
                                                                                                                                                     \
private:

#define REFLECTED_STRUCT(_class)                                                                                                                     \
public:                                                                                                                                              \
    typedef _class ThisStruct;                                                                                                                       \
    static void ConstructSelf(void *self) { new (self) _class(); }                                                                                   \
    static void DestructSelf(void *self) { static_cast<_class *>(self)->~_class(); }                                                                 \
    const Type *GetType() const { return TypeOf<_class>(); }                                                                                         \
    static const Type *GetStaticType() { return TypeOf<_class, true>(); }                                                                            \
    static Type *ConstructType();

#define REFLECTED(...)

#define REFLECTED_STRUCT_INPLACE(_class)                                                                                                             \
public:                                                                                                                                              \
    typedef _class ThisStruct;                                                                                                                       \
    static void ConstructSelf(void *self) { new (self) _class(); }                                                                                   \
    static void DestructSelf(void *self) { static_cast<_class *>(self)->~_class(); }                                                                 \
    const Type *GetType() { return TypeOf<_class>(); }                                                                                               \
    static const Type *GetStaticType() { return TypeOf<_class, true>(); }                                                                            \
    static Type *ConstructType()

#define DEFINE_CFG_ITEM(type, name, NAME, ...) \
type name = __VA_ARGS__; \
public: \
    const type &Get##NAME() const { return name; } \
    void Set##NAME(const type &enable) { \
        if (enable == name) { \
            return; \
        } \
        name = enable; \
        SetDirty(true); \
    } \
private:

#define OUT
#define NAMEOF(x) #x

template<typename T>
    requires IsEnum<T>
StringView GetEnumString(T e)
{
    const Type* type = TypeOf<T>();
    Assert(type != nullptr && type->IsEnumType(), "Type not valid");
    for (auto field: type->GetSelfDefinedFields())
    {
        Assert(type != nullptr && type->IsEnumType(), "Type is a enum but its fields are not enum values");
        if (field->GetEnumFieldValue() == static_cast<Int32>(e))
        {
            return field->GetName();
        }
    }
    return "ENUM_INVALID";
}

void NeverEnter();

// 反射标记
#ifdef ENABLE_REFLECTION
#define META(...) __attribute__((annotate("Reflected, " #__VA_ARGS__)))
#define GEN_HEADER(header) <string>
#define GENERATED_BODY(_class)
#else
#define META(...)
#define GEN_HEADER(header) header
#define GENERATED_BODY(_class) CONCAT(GENERATED_BODY, _class)
#endif

#define ECLASS(...) META(Class, __VA_ARGS__)
#define ESTRUCT(...) META(Struct, __VA_ARGS__)
#define EINTERFACE(...) META(Interface, __VA_ARGS__)
#define EFIELD(...) META(Field, __VA_ARGS__)
#define EENUM(...) META(Enum, __VA_ARGS__)
#define EVALUE(...) META(Value, __VA_ARGS__)
