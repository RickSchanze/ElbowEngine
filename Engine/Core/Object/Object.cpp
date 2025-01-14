/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "Core/Async/Execution/StartAsync.h"
#include "Core/Async/Execution/Then.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/Log/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/Archive.h"
#include "PersistentObject.h"

#include GEN_HEADER("Core.Object.generated.h")

GENERATED_SOURCE()

using namespace core;

void Object::SetDisplayName(StringView display_name)
{
#if WITH_EDITOR
    display_name_ = display_name;
#endif
}

using namespace exec;

void Object::AddReferencing(ObjectHandle handle)
{
    referencing_.push_back(handle);
}

void Object::RemoveReferencing(ObjectHandle handle)
{
    erase(referencing_, handle);
}

void Object::AddReferenced(ObjectHandle handle)
{
    referenced_.push_back(handle);
}

void Object::RemoveReferenced(ObjectHandle handle)
{
    erase(referenced_, handle);
}

void Object::GenerateInstanceHandle()
{
    handle_ = ObjectManager::GetRegistry().NextInstanceHandle();
}

void Object::RegisterSelf()
{
    ObjectManager::GetRegistry().RegisterObject(this);
}

void Object::ResolveObjectPtr()
{
    const auto type   = GetType();
    const auto fields = type->GetFields();
    for (const auto& field: fields)
    {
        if (field->GetType() == TypeOf<ObjectPtrBase>())
        {
            if (field->IsSequentialContainer())
            {
            }
            else if (field->IsAssociativeContainer())
            {
            }
            else
            {
                auto ptr = field->GetFieldPtr(this);
                static_cast<ObjectPtrBase*>(ptr)->SetOuter(handle_);
            }
        }
    }
}

AsyncResultHandle<ObjectHandle> Object::PerformPersistentObjectLoad()
{
    if (!IsPersistent()) return NULL_ASYNC_RESULT_HANDLE;

    auto* persistent = static_cast<PersistentObject*>(this);
    if (GetConfig<CoreConfig>()->IsMultiThreadPersistentLoadEnabled())
    {
        auto& scheduler = ThreadManager::GetScheduler();
        return StartAsync(Schedule(scheduler, ThreadSlot::Resource) | Then([this, persistent] {
                              persistent->PerformLoad();
                              return handle_;
                          }));
    }
    else
    {
        persistent->PerformLoad();
        return MakeAsyncResult(GetHandle());
    }
}

void Object::PostSerialized() {}

void Object::PostDeserialized()
{
    RegisterSelf();
    ResolveObjectPtr();
    PerformPersistentObjectLoad();
}

void Object::OnCreated()
{
    GenerateInstanceHandle();
    RegisterSelf();
    ResolveObjectPtr();
}

void Object::InternalSetAssetHandle(ObjectHandle handle)
{
    handle_ = handle;
    RegisterSelf();
}
