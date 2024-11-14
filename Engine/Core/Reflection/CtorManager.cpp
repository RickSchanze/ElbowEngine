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
        LOGGER.Warn(logcat::Reflection, "Duplicated ctor registration for type: {}", info.name);
    }
    ctors_[info] = ctor;
}

bool core::CtorManager::ConstructAt(const Type* info, void* ptr) const
{
    if (info == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "type is nullptr");
        return false;
    }
    if (ptr == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "ptr is nullptr");
        return false;
    }
    for (auto& [key, value]: ctors_)
    {
        if (key.hash_code == info->GetTypeHash())
        {
            value(ptr);
            return true;
        }
    }
    LOGGER.Error(logcat::Reflection, "No ctor registered for type: {}", info->GetName());
    return false;
}
