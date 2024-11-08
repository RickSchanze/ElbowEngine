/**
 * @file CtorManager.cpp
 * @author Echo 
 * @Date 24-11-8
 * @brief 
 */

#include "CtorManager.h"

void core::CtorManager::RegisterCtor(RTTITypeInfo info, InplaceCtor ctor)
{
    if (ctors_.contains(info))
    {
        LOGGER.Warn(LogCat::Reflection, "Duplicated ctor registration for type: {}", info.name);
    }
    ctors_[info] = ctor;
}

void core::CtorManager::ConstructAt(const Type* info, void* ptr) const
{
    if (info == nullptr)
    {
        LOGGER.Error(LogCat::Reflection, "type is nullptr");
        return;
    }
    if (ptr == nullptr)
    {
        LOGGER.Error(LogCat::Reflection, "ptr is nullptr");
        return;
    }
    for (auto& [key, value]: ctors_)
    {
        if (key.hash_code == info->GetTypeHash())
        {
            value(ptr);
            return;
        }
    }
    LOGGER.Error(LogCat::Reflection, "No ctor registered for type: {}", info->GetName());
}
