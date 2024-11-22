/**
 * @file MetaInfoManager.cpp
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#include "MetaInfoManager.h"

#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/Interface.h"
#include "Core/CoreGlobal.h"
#include "Core/Object/Object.h"
#include "Reflection.h"

#include <ranges>

namespace core
{
#define REGISTER_ATOMIC_TYPE(tname)                               \
    {                                                             \
        Type* tname##_type       = New<Type>();                   \
        tname##_type->type_hash_ = typeid(tname).hash_code();     \
        tname##_type->name_      = #tname;                        \
        tname##_type->attribute_ = Type::Atomic | Type::Trivial;  \
        tname##_type->size_      = sizeof(tname);                 \
        RTTITypeInfo info        = RTTITypeInfo::Create<tname>(); \
        types_registered_[info]  = tname##_type;                  \
    }   // namespace core

static void RegisterOtherType(MetaInfoManager& manager)
{
    {
        // String
        Type* type = Type::Create<String>("core.String");
        manager.RegisterType(RTTITypeInfo::Create<String>(), type);
    }
    {
        // StringView
        Type* type = Type::Create<StringView>("core.StringView");
        manager.RegisterType(RTTITypeInfo::Create<StringView>(), type);
    }
    {
        // ITypeGetter
        Type* type = Type::Create<ITypeGetter>("core.ITypeGetter");
        type->SetAttribute(Type::Interface);
        manager.RegisterType(RTTITypeInfo::Create<ITypeGetter>(), type);
    }
    {
        // Interface
        Type* type = Type::Create<Interface>("core.IInterface");
        type->SetAttribute(Type::Interface);
        manager.RegisterType(RTTITypeInfo::Create<Interface>(), type);
    }
}

void MetaInfoManager::Startup()
{

    REGISTER_ATOMIC_TYPE(int8_t);
    REGISTER_ATOMIC_TYPE(int16_t);
    REGISTER_ATOMIC_TYPE(int32_t);
    REGISTER_ATOMIC_TYPE(int64_t);
    REGISTER_ATOMIC_TYPE(uint8_t);
    REGISTER_ATOMIC_TYPE(uint16_t);
    REGISTER_ATOMIC_TYPE(uint32_t);
    REGISTER_ATOMIC_TYPE(uint64_t);
    REGISTER_ATOMIC_TYPE(float);
    REGISTER_ATOMIC_TYPE(double);
    REGISTER_ATOMIC_TYPE(bool);

    RegisterOtherType(*this);
}

void MetaInfoManager::Shutdown()
{
    for (auto& val: types_registered_ | std::views::values)
    {
        Delete(val);
    }
    types_registered_.clear();
}

void MetaInfoManager::RegisterType(RTTITypeInfo type_info)
{
    if (types_registered_.contains(type_info))
    {
        return;
    }
    if (!meta_data_registers_.contains(type_info))
    {
        LOGGER.Error(logcat::Reflection, "尝试注册类型{}, 但是此类型没有对应的注册函数", type_info.name);
        return;
    }
    types_registered_[type_info] = meta_data_registers_[type_info]();
    meta_data_registers_.erase(type_info);
}

void MetaInfoManager::RegisterTypeRegisterer(RTTITypeInfo type_info, const MetaDataRegisterer registerer)
{
    if (registerer == nullptr) return;
    if (meta_data_registers_.contains(type_info))
    {
        LOGGER.Warn(logcat::Reflection, "重复注册类型注册函数{}", type_info.name);
    }
    meta_data_registers_[type_info] = registerer;
}

void MetaInfoManager::RegisterType(RTTITypeInfo type_info, Type* type)
{
    if (types_registered_.contains(type_info))
    {
        LOGGER.Warn(logcat::Reflection, "重复注册类型{}", type_info.name);
        return;
    }
    types_registered_[type_info] = type;
}

Type* MetaInfoManager::GetType(const RTTITypeInfo& type_info)
{
    if (types_registered_.contains(type_info))
    {
        return types_registered_[type_info];
    }
    RegisterType(type_info);
    if (types_registered_.contains(type_info))
    {
        return types_registered_[type_info];
    }
    return nullptr;
}

}   // namespace core

size_t std::hash<core::RTTITypeInfo>::operator()(const core::RTTITypeInfo& type) const noexcept
{
    return type.hash_code;
}
