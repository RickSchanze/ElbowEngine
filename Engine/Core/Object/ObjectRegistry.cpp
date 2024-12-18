//
// Created by Echo on 24-12-18.
//

#include "ObjectRegistry.h"
core::ObjectHandle core::ObjectRegistry::NextInstanceHandle()
{
    return next_handle_instanced_--;
}

core::ObjectHandle core::ObjectRegistry::NextPersistentHandle()
{
    if (!free_handles_.empty())
    {
        const auto handle = free_handles_.back();
        free_handles_.pop_back();
        return handle;
    }
    return next_handle_persistent_++;
}