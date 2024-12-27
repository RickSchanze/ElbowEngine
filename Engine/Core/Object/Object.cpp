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
#include "Core/Core.h"
#include "Core/Log/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/Archive.h"
#include "PersistentObject.h"

#include GEN_HEADER("Core.Object.generated.h")
GENERATED_SOURCE()

using namespace core::exec;

void core::Object::AddReferencing(ObjectHandle handle)
{
    referencing_.push_back(handle);
}

void core::Object::RemoveReferencing(ObjectHandle handle)
{
    erase(referencing_, handle);
}

void core::Object::AddReferenced(ObjectHandle handle)
{
    referenced_.push_back(handle);
}

void core::Object::RemoveReferenced(ObjectHandle handle)
{
    erase(referenced_, handle);
}

void core::Object::GenerateInstanceHandle()
{
    handle_ = ObjectManager::GetRegistry().NextInstanceHandle();
}

void core::Object::RegisterSelf()
{
    ObjectManager::GetRegistry().RegisterObject(this);
}

void core::Object::ResolveObjectPtr()
{
    const auto type   = GetType();
    const auto fields = type->GetFields();
    for (const auto& field: fields)
    {
        if (field->GetType() == TypeOf<ObjectPtrBase>())
        {
            auto ptr = field->GetFieldPtr(this);
            static_cast<ObjectPtrBase*>(ptr)->SetOuter(handle_);
        }
    }
}

AsyncResultHandle core::Object::PerformPersistentObjectLoad()
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
    persistent->PerformLoad();
    return NULL_ASYNC_RESULT_HANDLE;
}

void core::Object::PostSerialized() {}

void core::Object::PostDeserialized()
{
    RegisterSelf();
    ResolveObjectPtr();
    PerformPersistentObjectLoad();
}

void core::Object::OnCreated()
{
    GenerateInstanceHandle();
    RegisterSelf();
    ResolveObjectPtr();
}

void core::Object::InternalSetAssetHandle(ObjectHandle handle)
{
    handle_ = handle;
    RegisterSelf();
}
