//
// Created by Echo on 2025/3/22.
//

#include "ObjectManager.hpp"

#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Async/Exec/Just.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/Memory/New.hpp"
#include "Core/Profile.hpp"
#include "Core/Serialization/YamlArchive.hpp"
#include "Object.hpp"
#include "PersistentObject.hpp"


class PersistentObject;
IMPL_REFLECTED_INPLACE(ObjectRegistry) {
    return Type::Create<ObjectRegistry>("ObjectRegistry") |
           refl_helper::AddField("next_handle_persistent", &ObjectRegistry::next_handle_persistent_) |
           refl_helper::AddField("free_handles", &ObjectRegistry::free_handles_);
}

exec::ExecFuture<ObjectHandle> ObjectRegistry::NextPersistentHandle() {
    ProfileScope _(__func__);
    auto GetNext = [this]() -> ObjectHandle {
        if (!free_handles_.Empty()) {
            const auto handle = free_handles_.Last();
            free_handles_.RemoveLast();
            return handle;
        }
        return next_handle_persistent_++;
    };
    if (IsMainThread()) {
        return exec::MakeExecFuture(GetNext());
    }

    auto task = exec::Just() | exec::Then([GetNext]() { return GetNext(); });
    return ThreadManager::ScheduleFutureAsync(task, NamedThread::Game);
}

Object *ObjectRegistry::GetObjectByHandle(ObjectHandle handle) {
    ProfileScope _(__func__);
    if (handle == 0)
        return nullptr;
    if (!objects_.Contains(handle))
        return nullptr;
    Object *ptr = objects_[handle];
    if (ptr->IsPendingKill())
        return nullptr;
    return ptr;
}

void ObjectRegistry::RemoveObject(Object *object) {
    if (object == nullptr)
        return;
    ObjectHandle handle = object->GetHandle();
    if (handle == 0) {
        Log(Error) << "ObjectHandle为0, 无法删除";
        return;
    }
    if (object->IsPersistent()) {
        if (static_cast<PersistentObject *>(object)->IsLoaded()) {
            static_cast<PersistentObject *>(object)->PerformUnload();
        }
    }
    Delete(object);
    objects_.Remove(handle);
}

void ObjectRegistry::RemoveAllObjects() {
    for (Int32 i = 0; i < 10; i++) {
        RemoveAllObjectLayered();
    }
    while (!objects_.Empty()) {
        RemoveObject(objects_.begin()->second);
    }
}

void ObjectRegistry::RemoveAllObjectLayered() {
    Int32 skip = 0;
    while (!objects_.Empty()) {
        auto begin = objects_.begin();
        std::advance(begin, skip);
        if (begin == objects_.end()) {
            return;
        }
        Object *obj = begin->second;
        if (obj->referenced_.Empty()) {
            RemoveObject(obj);
        } else {
            skip++;
        }
    }
}

void ObjectRegistry::RegisterObject(Object *object) {
    ProfileScope _(__func__);
    AutoLock lock(mutex_);
    if (object == nullptr)
        return;
    Assert(!objects_.Contains(object->GetHandle()), "ObjectHandle的重复注册");
    objects_[object->GetHandle()] = object;
}

void ObjectRegistry::UnregisterHandle(ObjectHandle handle) {
    ProfileScope _(__func__);
    AutoLock lock(mutex_);
    if (handle == 0)
        return;
    if (!objects_.Contains(handle))
        return;
    objects_.Remove(handle);
}

void ObjectRegistry::Save() const {
#if WITH_EDITOR
    YamlArchive archive;
    String serialized_str;
    archive.Serialize(*this, serialized_str);
    File::WriteAllText(REGISTRY_PATH, serialized_str);
#endif
}

void ObjectManager::Startup() {
    if (auto text = File::ReadAllText(REGISTRY_PATH)) {
        YamlArchive archive;
        archive.Deserialize(*text, &registry_, TypeOf<ObjectRegistry>());
    } else {
        String serialized_str;
        YamlArchive archive;
        archive.Serialize(registry_, serialized_str);
        File::WriteAllText(REGISTRY_PATH, serialized_str);
    }
}

void ObjectManager::Shutdown() {
#if WITH_EDITOR
    SaveObjectRegistry();
#endif
    registry_.RemoveAllObjects();
}
