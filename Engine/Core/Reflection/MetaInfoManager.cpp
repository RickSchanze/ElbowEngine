/**
 * @file MetaInfoManager.cpp
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#include "MetaInfoManager.h"

#include "CoreGlobal.h"
#include "Reflection.h"

namespace core
{
MetaInfoManager::MetaInfoManager()
{
    // 注册基础类型
}


MetaInfoManager::~MetaInfoManager()
{
    for (auto& type: types_registered_)
    {
        Delete(type.second);
    }
    types_registered_.clear();
}

void MetaInfoManager::RegisterType(size_t type_name)
{
    if (types_registered_.contains(type_name))
    {
        return;
    }
    if (!meta_data_registers_.contains(type_name))
    {
        LOGGER.Error(LogCat::Reflection, "尝试注册类型{}, 但是此类型没有对应的注册函数", type_name);
        return;
    }
    types_registered_[type_name] = meta_data_registers_[type_name]();
    meta_data_registers_.erase(type_name);
}

void MetaInfoManager::RegisterTypeRegisterer(size_t type_name, MetaDataRegisterer registerer)
{
    if (registerer == nullptr) return;
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

}   // namespace core
