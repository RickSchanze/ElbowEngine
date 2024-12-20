//
// Created by Echo on 24-12-18.
//
#include "ObjectPtr.h"
#include "Core/Core.h"

#include GEN_HEADER("Core.ObjectPtr.generated.h")
#include "ObjectRegistry.h"

GENERATED_SOURCE()

core::ObjectPtrBase::~ObjectPtrBase()
{
    SetObject(INVALID_OBJECT_HANDLE);
}

void core::ObjectPtrBase::SetOuter(const ObjectHandle outer)
{
    // 调整引用
    Assert::Require(logcat::Core_Object, outer_ == INVALID_OBJECT_HANDLE, "Outer只能设置一次");
    if (outer == INVALID_OBJECT_HANDLE) return;
    outer_ = outer;
    // object_ 可能有序列化修改而不是INVALID
    if (object_ != INVALID_OBJECT_HANDLE)
    {
        const auto out_obj = ObjectManager::GetObjectByHandle(outer_);
        const auto obj     = ObjectManager::GetObjectByHandle(object_);
        Assert::Require(logcat::Core_Object, out_obj && obj, "Outer和Object都必须有效");
        out_obj->AddReferencing(object_);
        obj->AddReferenced(outer_);
    }
}

void core::ObjectPtrBase::SetObject(const ObjectHandle handle)
{
    if (outer_ != INVALID_OBJECT_HANDLE)
    {
        if (handle == INVALID_OBJECT_HANDLE)
        {
            if (object_ == INVALID_OBJECT_HANDLE)
            {
                return;
            }
            else
            {
                const auto outer_obj = ObjectManager::GetObjectByHandle(outer_);
                const auto obj       = ObjectManager::GetObjectByHandle(object_);
                if (outer_obj && obj)
                {
                    outer_obj->RemoveReferencing(object_);
                    obj->RemoveReferenced(outer_);
                }
                object_ = INVALID_OBJECT_HANDLE;
            }
        }
        else
        {
            if (object_ == INVALID_OBJECT_HANDLE)
            {
                const auto outer_obj = ObjectManager::GetObjectByHandle(outer_);
                const auto obj       = ObjectManager::GetObjectByHandle(handle);
                if (outer_obj && obj)
                {
                    outer_obj->AddReferencing(handle);
                    obj->AddReferenced(outer_);
                }
                object_ = handle;
            }
            else
            {
                const auto outer_obj = ObjectManager::GetObjectByHandle(outer_);
                const auto new_obj   = ObjectManager::GetObjectByHandle(handle);
                const auto old_obj   = ObjectManager::GetObjectByHandle(object_);
                if (outer_obj && old_obj)
                {
                    outer_obj->RemoveReferencing(object_);
                    old_obj->RemoveReferenced(outer_);
                }
                if (outer_obj && new_obj)
                {
                    outer_obj->AddReferencing(handle);
                    new_obj->AddReferenced(outer_);
                }
                object_ = handle;
            }
        }
    }
    else
    {
        object_ = handle;
    }
}
