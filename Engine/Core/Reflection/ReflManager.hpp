//
// Created by Echo on 2025/3/21.
//
#pragma once

#include "Core/Logger/VLog.hpp"
#include "Core/Manager/Manager.hpp"
#include "Core/String/String.hpp"
#include "Core/TypeAlias.hpp"
#include "Core/TypeTraits.hpp"
#include "Type.hpp"

template <typename T>
concept HasGetTypeMem = requires(T& t) {
    { t.GetStaticType() };
};

struct RTTITypeInfo;
template <>
struct std::hash<RTTITypeInfo>
{
    size_t operator()(const RTTITypeInfo& type) const noexcept;
};

static void EnumConstructor(void* ptr)
{
    new (ptr) Int32();
}
static void EnumDestructor(void* ptr)
{
    static_cast<Int32*>(ptr)->~Int32();
}

typedef Type* (*MetaDataRegisterer)();
typedef void (*InplaceCtor)(void*);
typedef void (*InplaceDtor)(void*);

struct CtorDtor
{
    InplaceCtor ctor;
    InplaceDtor dtor;
};

struct RTTITypeInfo
{
    StringView Name;
    size_t HashCode{};

    template <typename T>
    static RTTITypeInfo Create(const StringView InName)
    {
        return {InName, typeid(T).hash_code()};
    }

    bool operator==(const RTTITypeInfo& o) const
    {
        return HashCode == o.HashCode;
    }
};

class ReflManager : public Manager<ReflManager>
{
public:
    virtual void Startup() override;

    virtual void Shutdown() override;

    void RegisterType(const RTTITypeInfo& type_info);

    void RegisterTypeRegisterer(const RTTITypeInfo& type_info, MetaDataRegisterer registerer);

    void RegisterType(const RTTITypeInfo& type_info, Type* type);

    /**
     * 根据一个类型名字寻找此类型, 如果此类型已注册则直接返回, 没有则先注册则返回
     * 没有注册也在待注册里找不到, 返回nullptr
     * @param InName
     * @return
     */
    const Type* FindTypeImpl(StringView InName);
    static const Type* FindType(const StringView InName)
    {
        return GetByRef().FindTypeImpl(InName);
    }

    template <typename T>
    static T* CreateInstance()
    {
        return static_cast<T*>(CreateInstance(T::GetStaticType()));
    }

    static void* CreateInstance(const Type* InType)
    {
        if (InType == nullptr)
            return nullptr;
        void* Ptr = Malloc(InType->GetSize());
        if (ConstructAt(InType, Ptr))
        {
            return Ptr;
        }
        else
        {
            return nullptr;
        }
    }

    template <typename T>
    void Register(StringView InName)
    {
        auto type_info = RTTITypeInfo::Create<T>(InName);
        RegisterTypeRegisterer(type_info, &T::ConstructType);
        RegisterCtorDtor(type_info, &T::ConstructSelf, &T::DestructSelf);
    }

    template <typename T>
    void RegisterNoConstructor(StringView InName)
    {
        auto type_info = RTTITypeInfo::Create<T>(InName);
        RegisterTypeRegisterer(type_info, &T::ConstructType);
    }

    template <typename T>
        requires Traits::IsEnum<T>
    void Register(StringView InName, MetaDataRegisterer registerer)
    {
        auto type_info = RTTITypeInfo::Create<T>(InName);
        RegisterTypeRegisterer(type_info, registerer);
        RegisterCtorDtor(type_info, EnumConstructor, EnumDestructor);
    }

    Type* GetType(const RTTITypeInfo& type_info);

    void RegisterCtorDtor(const RTTITypeInfo& info, InplaceCtor ctor, InplaceDtor dtor);

    static bool ConstructAt(const Type* info, void* ptr);

    bool DestroyAt(const Type* info, void* ptr) const;

    virtual Float GetLevel() const override
    {
        return 2;
    }
    virtual StringView GetName() const override
    {
        return "ReflManager";
    }

private:
    Map<RTTITypeInfo, Type*> mTypesRegistered;
    Map<RTTITypeInfo, MetaDataRegisterer> mMetaDataRegisters;
    Map<RTTITypeInfo, CtorDtor> mCtors;
};

template <typename T, bool ForceFind = false>
const Type* TypeOf()
{
    if constexpr (HasGetTypeMem<T> && !ForceFind)
    {
        return T::GetStaticType();
    }
    else
    {
        using OriginalT = std::remove_cvref_t<T>;
        const RTTITypeInfo info = {typeid(OriginalT).name(), typeid(OriginalT).hash_code()};
        return ReflManager::Get()->GetType(info);
    }
}

inline size_t std::hash<RTTITypeInfo>::operator()(const RTTITypeInfo& type) const noexcept
{
    return type.HashCode;
}
