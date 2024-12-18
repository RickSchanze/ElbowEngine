//
// Created by Echo on 24-12-18.
//

#pragma once

#include "Core/Base/FlatMap.h"
#include "Core/Core.h"

namespace core
{
class CLASS() ObjectRegistry
{
public:
    ObjectHandle NextInstanceHandle();
    ObjectHandle NextPersistentHandle();

private:
    // 所有的Object
    FlatMap<ObjectHandle, Object*> objects_;

    // 下一个可用的持久化对象的handle
    PROPERTY()
    int32_t next_handle_persistent_ = 1;

    // 下一个可用的临时对象的handle
    int32_t next_handle_instanced_ = -1;

    // 可用的, 由于删除造成的handle(persistent only)
    PROPERTY()
    Array<ObjectHandle> free_handles_;
};
}   // namespace core
