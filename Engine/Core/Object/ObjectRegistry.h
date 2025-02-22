//
// Created by Echo on 24-12-18.
//

#pragma once

#include "Core/Base/FlatMap.h"
#include "Core/Object/Object.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include GEN_HEADER("Core.ObjectRegistry.generated.h")
#include "Core/Async/Execution/Then.h"
#include "Core/Async/ThreadManager.h"

namespace core {
class ThreadCluster;
}
core::ThreadScheduler &_GetScheduler();

namespace core {
// TODO: 多线程安全
class CLASS() ObjectRegistry {
  GENERATED_CLASS(ObjectRegistry)
public:
  ObjectHandle NextInstanceHandle();

  exec::AsyncResultHandle<ObjectHandle> NextPersistentHandle();

  Object *GetObjectByHandle(ObjectHandle handle);

  void RemoveObject(Object *object);

  void RemoveAllObjects();

  void RemoveAllObjectLayered();

  void RegisterObject(Object *object);
  void UnregisterHandle(ObjectHandle handle);

  void Save();

private:
  // 所有的Object
  FlatMap<ObjectHandle, Object *> objects_;

  // 下一个可用的持久化对象的handle
  PROPERTY()
  Int32 next_handle_persistent_ = 1;

  // 下一个可用的临时对象的handle
  Int32 next_handle_instanced_ = -1;

  // 可用的, 由于删除造成的handle(persistent only)
  PROPERTY()
  Array<ObjectHandle> free_handles_;

  DECLARE_TRACEABLE_MUTEX(std::mutex, mutex_, "mutex to register/unregister object in multi thread");
};

class ObjectManager : public Manager<ObjectManager> {
private:
  ObjectRegistry registry_;

public:
  [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::L8; }
  [[nodiscard]] StringView GetName() const override { return "ObjectManager"; }

  void Startup() override;
  void Shutdown() override;

  template <typename T, typename... Args>
    requires std::derived_from<T, Object>
  static exec::AsyncResultHandle<T *> CreateNewObjectAsync(Args &&...args) {
    if (ThreadUtils::IsCurrentMainThread()) {
      return exec::MakeAsyncResult(NewObject<T>(Forward<Args>(args)...));
    }
    auto &scheduler = _GetScheduler();
    return exec::StartAsync(exec::Schedule(scheduler, ThreadSlot::Game) |
                            exec::Then([args...]() { return NewObject<T>(Forward<Args>(args)...); }));
  }

  template <typename T, typename... Args>
    requires std::derived_from<T, Object>
  static T *CreateNewObject(Args &&...args) {
    return CreateNewObjectAsync<T>(Forward<Args>(args)...)->GetValue().GetValue() | First;
  }

  static bool IsObjectExist(ObjectHandle handle) { return GetRegistry().GetObjectByHandle(handle) != nullptr; }

#if WITH_EDITOR
  static void SaveObjectRegistry();
#endif

  static ObjectRegistry &GetRegistry();
  static Object *GetObjectByHandle(ObjectHandle handle);

  template <typename T>
    requires std::is_base_of_v<Object, T>
  static T *GetObjectByHandle(ObjectHandle handle) {
    return static_cast<T *>(GetObjectByHandle(handle));
  }
};

} // namespace core
