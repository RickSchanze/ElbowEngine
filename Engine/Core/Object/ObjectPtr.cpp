//
// Created by Echo on 2025/3/25.
//

#include "ObjectPtr.hpp"

#include "Core/Assert.hpp"
#include "ObjectManager.hpp"

Object *internal::GetObjectFromObjectManager(ObjectHandle handle) { return ObjectManager::GetObjectByHandle(handle); }

void ObjectPtrBase::SetOuter(const ObjectHandle outer) {
    if (outer == 0)
        return;
    outer_ = outer;
    // object_ 可能有序列化修改而不是INVALID
    if (object_ != 0) {
        const auto out_obj = ObjectManager::GetObjectByHandle(outer_);
        const auto obj = ObjectManager::GetObjectByHandle(object_);
        Assert(out_obj && obj, "Outer和Object都必须有效");
        out_obj->AddReferencing(object_);
        obj->AddReferenced(outer_);
    }
}

void ObjectPtrBase::SetObject(const ObjectHandle handle) {
    if (outer_ != 0) {
        if (handle == 0) {
            if (object_ == 0) {
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
                object_ = 0;
            }
        } else {
            if (object_ == 0) {
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

#if WITH_EDITOR
void ObjectPtrBase::ModifyOuter(const ObjectHandle handle) {
    if (handle == 0)
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
#endif
