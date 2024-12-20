/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "Core/Core.h"
#include "Core/Log/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/Archive.h"

#include "Core.Object.generated.h"
#include "PersistentObject.h"

GENERATED_SOURCE()

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
    handle_ = ObjectManager::Get()->GetRegistry().NextInstanceHandle();
}

void core::Object::RegisterSelf()
{
    ObjectManager::Get()->GetRegistry().RegisterObject(this);
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

void core::Object::PerformPersistentObjectLoad()
{
    if (Is(TypeOf<PersistentObject>()))
    {
        Cast<PersistentObject>(this)->PerformLoad();
    }
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
