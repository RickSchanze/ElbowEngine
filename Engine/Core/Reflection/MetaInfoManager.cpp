/**
 * @file MetaInfoManager.cpp
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#include "MetaInfoManager.h"

#include "Base/CoreTypeDef.h"
#include "CoreGlobal.h"
#include "Reflection.h"

namespace core
{

#define REGISTER_ATOMIC_TYPE(tname)                                         \
    Type* tname##_type                         = New<Type>();               \
    tname##_type->type_hash                    = typeid(tname).hash_code(); \
    tname##_type->name                         = #tname;                    \
    tname##_type->attribute                    = Type::Atomic;              \
    tname##_type->size                         = sizeof(tname);             \
    types_registered_[tname##_type->type_hash] = tname##_type;

MetaInfoManager::MetaInfoManager()
{
    // 注册基础类型
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
}


MetaInfoManager::~MetaInfoManager()
{
    for (auto& type: types_registered_)
    {
        Delete(type.second);
    }
    types_registered_.clear();
}

void MetaInfoManager::RegisterType(size_t type_hash)
{
    if (types_registered_.contains(type_hash))
    {
        return;
    }
    if (!meta_data_registers_.contains(type_hash))
    {
        LOGGER.Error(LogCat::Reflection, "尝试注册类型{}, 但是此类型没有对应的注册函数", type_hash);
        return;
    }
    types_registered_[type_hash] = meta_data_registers_[type_hash].Registerer();
    meta_data_registers_.erase(type_hash);
}

void MetaInfoManager::RegisterTypeRegisterer(size_t type_name, const MetaDataRegisterer& registerer)
{
    if (registerer.Registerer == nullptr || registerer.name.Empty()) return;
    if (meta_data_registers_.contains(type_name))
    {
        LOGGER.Warn(LogCat::Reflection, "重复注册类型注册函数{}", type_name);
    }
    meta_data_registers_[type_name] = registerer;
}

Type* MetaInfoManager::GetType(size_t type_name)
{
    if (types_registered_.contains(type_name))
    {
        return types_registered_[type_name];
    }
    RegisterType(type_name);
    if (types_registered_.contains(type_name))
    {
        return types_registered_[type_name];
    }
    return nullptr;
}

Type* MetaInfoManager::GetType(StringView type_name)
{
    for (auto types_registered: types_registered_)
    {
        if (types_registered.second->GetName() == type_name)
        {
            return types_registered.second;
        }
    }
    for (auto meta_data_registers: meta_data_registers_)
    {
        if (meta_data_registers.second.name == type_name)
        {
            RegisterType(meta_data_registers.first);
            break;
        }
    }
    for (auto types_registered: types_registered_)
    {
        if (types_registered.second->GetName() == type_name)
        {
            return types_registered.second;
        }
    }
    return nullptr;
}

}   // namespace core
