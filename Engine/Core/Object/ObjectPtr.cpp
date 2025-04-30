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
    mOuter = outer;
    // object_ 可能有序列化修改而不是INVALID
    if (mObject != 0) {
        const auto out_obj = ObjectManager::GetObjectByHandle(mOuter);
        const auto obj = ObjectManager::GetObjectByHandle(mObject);
        Assert(out_obj && obj, "Outer和Object都必须有效");
        out_obj->AddReferencing(mObject);
        obj->AddReferenced(mOuter);
    }
}

void ObjectPtrBase::SetObject(const ObjectHandle handle) {
    if (mOuter != 0) {
        if (handle == 0) {
            if (mObject == 0) {
                return;
            } else {
                const auto outer_obj = ObjectManager::GetObjectByHandle(mOuter);
                const auto obj = ObjectManager::GetObjectByHandle(mObject);
                if (outer_obj && obj) {
                    // TODO: 这里应该RemoveReferencing吗？因为不Remove可以知道这里曾经引用了一个东西然后Fallback
                    // TODO: 或者给个选项决定是否要RemoveReferencing
                    outer_obj->RemoveReferencing(mObject);
                    obj->RemoveReferenced(mOuter);
                }
                mObject = 0;
            }
        } else {
            if (mObject == 0) {
                const auto outer_obj = ObjectManager::GetObjectByHandle(mOuter);
                const auto obj = ObjectManager::GetObjectByHandle(handle);
                if (outer_obj && obj) {
                    outer_obj->AddReferencing(handle);
                    obj->AddReferenced(mOuter);
                }
                mObject = handle;
            } else {
                const auto outer_obj = ObjectManager::GetObjectByHandle(mOuter);
                const auto new_obj = ObjectManager::GetObjectByHandle(handle);
                const auto old_obj = ObjectManager::GetObjectByHandle(mObject);
                if (outer_obj && old_obj) {
                    outer_obj->RemoveReferencing(mObject);
                    old_obj->RemoveReferenced(mOuter);
                }
                if (outer_obj && new_obj) {
                    outer_obj->AddReferencing(handle);
                    new_obj->AddReferenced(mOuter);
                }
                mObject = handle;
            }
        }
    } else {
        mObject = handle;
    }
}

#if WITH_EDITOR
void ObjectPtrBase::ModifyOuter(const ObjectHandle handle) {
    if (handle == 0)
        return;
    if (mOuter == handle)
        return;
    Object *outer_obj = ObjectManager::GetObjectByHandle(mOuter);
    Object *obj = ObjectManager::GetObjectByHandle(mObject);
    if (outer_obj && obj) {
        outer_obj->RemoveReferencing(mObject);
        obj->RemoveReferenced(mOuter);
        mOuter = handle;
        outer_obj->AddReferencing(mObject);
        obj->AddReferenced(mOuter);
    }
}
#endif
