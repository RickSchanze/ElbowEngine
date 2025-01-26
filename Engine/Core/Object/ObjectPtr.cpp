//
// Created by Echo on 24-12-18.
//
#include "ObjectPtr.h"
#include "Core/Core.h"
#include "ObjectRegistry.h"

#include GEN_HEADER("Core.ObjectPtr.generated.h")

GENERATED_SOURCE()

core::ObjectPtrBase::~ObjectPtrBase() { SetObject(INVALID_OBJECT_HANDLE); }

void core::ObjectPtrBase::SetOuter(const ObjectHandle outer) {
  if (outer == INVALID_OBJECT_HANDLE)
    return;
  outer_ = outer;
  // object_ 可能有序列化修改而不是INVALID
  if (object_ != INVALID_OBJECT_HANDLE) {
    const auto out_obj = ObjectManager::GetObjectByHandle(outer_);
    const auto obj = ObjectManager::GetObjectByHandle(object_);
    Assert::Require(logcat::Core_Object, out_obj && obj, "Outer和Object都必须有效");
    out_obj->AddReferencing(object_);
    obj->AddReferenced(outer_);
  }
}

void core::ObjectPtrBase::SetObject(const ObjectHandle handle) {
  if (outer_ != INVALID_OBJECT_HANDLE) {
    if (handle == INVALID_OBJECT_HANDLE) {
      if (object_ == INVALID_OBJECT_HANDLE) {
        return;
      } else {
        const auto outer_obj = ObjectManager::GetObjectByHandle(outer_);
        const auto obj = ObjectManager::GetObjectByHandle(object_);
        if (outer_obj && obj) {
          // TODO: 这里应该RemoveReferencing吗？因为不Remove可以知道这里曾经引用了一个东西然后Fallback
          // TODO: 或者给个选项决定是否要RemoveReferencing
          outer_obj->RemoveReferencing(object_);
          obj->RemoveReferenced(outer_);
        }
        object_ = INVALID_OBJECT_HANDLE;
      }
    } else {
      if (object_ == INVALID_OBJECT_HANDLE) {
        const auto outer_obj = ObjectManager::GetObjectByHandle(outer_);
        const auto obj = ObjectManager::GetObjectByHandle(handle);
        if (outer_obj && obj) {
          outer_obj->AddReferencing(handle);
          obj->AddReferenced(outer_);
        }
        object_ = handle;
      } else {
        const auto outer_obj = ObjectManager::GetObjectByHandle(outer_);
        const auto new_obj = ObjectManager::GetObjectByHandle(handle);
        const auto old_obj = ObjectManager::GetObjectByHandle(object_);
        if (outer_obj && old_obj) {
          outer_obj->RemoveReferencing(object_);
          old_obj->RemoveReferenced(outer_);
        }
        if (outer_obj && new_obj) {
          outer_obj->AddReferencing(handle);
          new_obj->AddReferenced(outer_);
        }
        object_ = handle;
      }
    }
  } else {
    object_ = handle;
  }
}

void core::ObjectPtrBase::ModifyOuter(ObjectHandle handle) {
  if (handle == INVALID_OBJECT_HANDLE)
    return;
  if (outer_ == handle)
    return;
  Object *outer_obj = ObjectManager::GetObjectByHandle(outer_);
  Object *obj = ObjectManager::GetObjectByHandle(object_);
  if (outer_obj && obj) {
    outer_obj->RemoveReferencing(object_);
    obj->RemoveReferenced(outer_);
    outer_ = handle;
    outer_obj->AddReferencing(object_);
    obj->AddReferenced(outer_);
  }
}

core::Object *_ObjectManagerGetObjectByHandle(core::ObjectHandle handle) {
  return core::ObjectManager::GetObjectByHandle(handle);
}