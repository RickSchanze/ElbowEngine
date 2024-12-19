//
// Created by Echo on 24-12-18.
//

#pragma once

#include "Core/Base/FlatMap.h"
#include "Core/Reflection/MetaInfoMacro.h"
#include "Core/Object/Object.h"

namespace core
{
class CLASS() ObjectRegistry
{
public:
    ObjectHandle NextInstanceHandle();
    ObjectHandle NextPersistentHandle();

    Object* GetObjectByHandle(ObjectHandle handle);

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

class ObjectManager : public Manager<ObjectManager>
{
private:
    ObjectRegistry registry_;

public:
    [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::Top; }
    [[nodiscard]] StringView   GetName() const override { return "ObjectManager"; }

    void Startup() override;
    void Shutdown() override;

    static ObjectRegistry& GetRegistry();
    static Object*         GetObjectByHandle(ObjectHandle handle);
};
}   // namespace core
