//
// Created by Echo on 2025/3/22.
//

#include "Object.hpp"

#include "Core/Async/Exec/Just.hpp"
#include "Core/Async/Exec/Then.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/Core.hpp"
#include "Core/Logger/Logger.hpp"
#include "ObjectManager.hpp"
#include "ObjectPtr.hpp"
#include "PersistentObject.hpp"

using namespace NExec;

class PersistentObject;

Object::~Object()
{
    if (mReferenced.Empty())
        return;
    if constexpr (DUMP_OBJECT_REFERENCE_WHEN_DESTROY)
    {
        Log(Warn, "Object::~Object") << String::Format("对象 {} 被销毁时仍有{}个对象引用它", *mName, mReferenced.Count());
        for (const auto& handle : mReferenced)
        {
            if (Object* obj = ObjectManager::GetRegistry().GetObjectByHandle(handle))
            {
                Log(Warn) << String::FromInt(handle) + ": " + obj->mName;
            }
            else
            {
                Log(Warn) << String::FromInt(handle) + ": " + "已失效";
            }
        }
    }
}

static void FindObjPtr(const Type* t, void* field_ptr, ObjectHandle handle)
{
    if (t == nullptr)
        return;
    const auto fields = t->GetFields();
    for (const auto& field : fields)
    {
        if (field->GetType() == TypeOf<ObjectPtrBase>())
        {
            const auto ptr = static_cast<ObjectPtrBase*>(field->GetFieldPtr(field_ptr));
            ptr->ModifyOuter(handle);
        }
        else
        {
            FindObjPtr(field->GetType(), field->GetFieldPtr(field_ptr), handle);
        }
    }
}

void Object::SetObjectHandle(const Int32 handle)
{
    // 此时自己的Object Handle 需要通知所有的ObjectPtr
    auto type = GetType();
    FindObjPtr(type, this, handle);
    UnregisterSelf();
    mHandle = handle;
    RegisterSelf();
}

void Object::GenerateInstanceHandle()
{
    mHandle = ObjectManager::GetRegistry().NextInstanceHandle();
}

void Object::UnregisterSelf() const
{
    ObjectManager::GetRegistry().UnregisterHandle(GetHandle());
}

void Object::RegisterSelf()
{
    ObjectManager::GetRegistry().RegisterObject(this);
}

void Object::ResolveObjectPtr()
{
    const auto type = GetType();
    const auto fields = type->GetFields();
    for (const auto& field : fields)
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
                static_cast<ObjectPtrBase*>(ptr)->SetOuter(mHandle);
            }
        }
    }
}

void Object::OnDestroyed()
{
}

ExecFuture<ObjectHandle> Object::PerformPersistentObjectLoadAsync()
{
    if (!IsPersistent())
    {
        Log(Error) << "尝试加载非持久化对象, handle="_es + String::FromInt(GetHandle());
        return MakeExecFuture(0);
    }
    auto task = Just() | Then([this] {
                    static_cast<PersistentObject*>(this)->PerformLoad();
                    return GetHandle();
                });
    auto f = ThreadManager::ScheduleFutureAsync(task);
    return ExecFuture(Move(f));
}

ObjectHandle Object::PerformPersistentObjectLoad()
{
    if (!IsPersistent())
        return 0;
    auto* persistent = static_cast<PersistentObject*>(this);
    persistent->PerformLoad();
    return GetHandle();
}

void Destroy(Object* obj)
{
    if (obj)
    {
        ObjectManager::GetRegistry().RemoveObject(obj);
    }
}
