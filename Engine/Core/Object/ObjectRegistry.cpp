//
// Created by Echo on 24-12-18.
//

#include "ObjectRegistry.h"

#include "Core/CoreEvents.h"
#include "Core/Serialization/YamlArchive.h"
#include "PersistentObject.h"
#include "Core/Async/ThreadManager.h"
#include "Platform/RHI/GfxContext.h"

#include GEN_HEADER("Core.ObjectRegistry.generated.h")


GENERATED_SOURCE()

using namespace core;
using namespace exec;

ObjectHandle ObjectRegistry::NextInstanceHandle() { return next_handle_instanced_--; }

constexpr auto REGISTRY_PATH = "Library/ObjectRegistry.yaml";

AsyncResultHandle<ObjectHandle> ObjectRegistry::NextPersistentHandle() {
  auto GetNext = [this]() -> ObjectHandle {
    if (!free_handles_.empty()) {
      const auto handle = free_handles_.back();
      free_handles_.pop_back();
      return handle;
    }
    return next_handle_persistent_++;
  };
  if (ThreadUtils::IsCurrentMainThread()) {
    return MakeAsyncResult(GetNext());
  }
  return StartAsync(Schedule(ThreadManager::GetScheduler(), ThreadSlot::Game) | Then([GetNext] { return GetNext(); }));
}

Object *ObjectRegistry::GetObjectByHandle(const ObjectHandle handle) {
  PROFILE_SCOPE_AUTO;
  if (handle == INVALID_OBJECT_HANDLE)
    return nullptr;
  if (!objects_.contains(handle))
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
  if (handle == INVALID_OBJECT_HANDLE) {
    LOGGER.Warn(logcat::Core_Object, "无效的ObjectHandle");
    return;
  }
  if (object->IsPersistent()) {
    static_cast<PersistentObject *>(object)->PerformUnload();
  }
  Delete(object);
  objects_.erase(handle);
}

void ObjectRegistry::RemoveAllObjects() {
  for (Int32 i = 0; i < 10; i++) {
    RemoveAllObjectLayered();
  }
  while (!objects_.empty()) {
    RemoveObject(objects_.begin()->second);
  }
}

void ObjectRegistry::RemoveAllObjectLayered() {
  Int32 skip = 0;
  while (!objects_.empty()) {
    auto begin = objects_.begin();
    std::advance(begin, skip);
    if (begin == objects_.end()) {
      return;
    }
    Object *obj = begin->second;
    if (obj->referenced_.empty()) {
      RemoveObject(obj);
    } else {
      skip++;
    }
  }
}

void ObjectRegistry::RegisterObject(Object *object) {
  PROFILE_SCOPE_AUTO;
  std::lock_guard lock(mutex_);
  if (object == nullptr)
    return;
  Assert::Require(logcat::Core_Object, !objects_.contains(object->GetHandle()), "ObjectHandle的重复注册");
  objects_[object->GetHandle()] = object;
}

void ObjectRegistry::UnregisterHandle(ObjectHandle handle) {
  PROFILE_SCOPE_AUTO;
  std::lock_guard lock(mutex_);
  if (handle == INVALID_OBJECT_HANDLE)
    return;
  if (!objects_.contains(handle))
    return;
  objects_.erase(handle);
}

void ObjectRegistry::Save() {
#if WITH_EDITOR
  YamlArchive archive;
  String serialized_str;
  archive.Serialize(*this, serialized_str);
  Event_OnWriteFileText.Invoke(REGISTRY_PATH, serialized_str);
#endif
}

void ObjectManager::Startup() {
  if (auto text = Event_OnRequireReadFileText.Invoke(REGISTRY_PATH)) {
    YamlArchive archive;
    archive.Deserialize(*text, &registry_, TypeOf<ObjectRegistry>());
  } else {
    String serialized_str;
    YamlArchive archive;
    archive.Serialize(registry_, serialized_str);
    Event_OnWriteFileText.Invoke(REGISTRY_PATH, serialized_str);
  }
}

void ObjectManager::Shutdown() {
#if WITH_EDITOR
  SaveObjectRegistry();
#endif
  registry_.RemoveAllObjects();
}

#if WITH_EDITOR
void ObjectManager::SaveObjectRegistry() { GetRegistry().Save(); }
#endif

ObjectRegistry &ObjectManager::GetRegistry() { return Get()->registry_; }

Object *ObjectManager::GetObjectByHandle(ObjectHandle handle) { return GetRegistry().GetObjectByHandle(handle); }

ThreadScheduler& _GetScheduler() {
  return ThreadManager::GetScheduler();
}