//
// Created by Echo on 2025/3/22.
//


#pragma once
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Async/Exec/Just.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/Core.hpp"
#include "Object.hpp"

constexpr auto REGISTRY_PATH = "Library/ObjectRegistry.yaml";

struct ObjectRegistry {
    REFLECTED_SINGLE_CLASS(ObjectRegistry)
public:
    Int32 NextInstanceHandle() { return next_handle_instanced_--; }

    exec::ExecFuture<ObjectHandle> NextPersistentHandle();

    Object *GetObjectByHandle(ObjectHandle handle);

    void RemoveObject(Object *object);

    void RemoveAllObjects();

    void RemoveAllObjectLayered();

    void RegisterObject(Object *object);

    void UnregisterHandle(ObjectHandle handle);

    void Save() const;

private:
    // 所有的Object
    Map<ObjectHandle, Object *> objects_{};

    // 下一个可用的持久化对象的handle

    Int32 next_handle_persistent_ = 1;

    // 下一个可用的临时对象的handle
    Int32 next_handle_instanced_ = -1;

    // 可用的, 由于删除造成的handle(persistent only)
    Array<ObjectHandle> free_handles_{};

    Mutex mutex_{};
};

class ObjectManager : public Manager<ObjectManager> {
private:
    ObjectRegistry registry_;

public:
    virtual Float GetLevel() const override { return 14; }
    virtual StringView GetName() const override
    {
        return "ObjectManager";
    }

    virtual void Startup() override;
    virtual void Shutdown() override;

    template<typename T, typename... Args>
        requires std::derived_from<T, Object>
    static exec::ExecFuture<T *> CreateNewObjectAsync(Args &&...args) {
        if (IsMainThread()) {
            return exec::MakeExecFuture(NewObject<T>(Forward<Args>(args)...));
        }
        return ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([args...]() { return NewObject<T>(Forward<Args>(args)...); }),
                                                  NamedThread::Game);
    }

    template<typename T, typename... Args>
        requires std::derived_from<T, Object>
    static T *CreateNewObject(Args &&...args) {
        return CreateNewObjectAsync<T>(Forward<Args>(args)...).Get();
    }

    static bool IsObjectExist(ObjectHandle handle) { return GetRegistry().GetObjectByHandle(handle) != nullptr; }

#if WITH_EDITOR
    static void SaveObjectRegistry() { GetRegistry().Save(); }
#endif

    static ObjectRegistry &GetRegistry() { return Get()->registry_; }
    static Object *GetObjectByHandle(ObjectHandle handle) { return GetRegistry().GetObjectByHandle(handle); }

    template<typename T>
        requires std::is_base_of_v<Object, T>
    static T *GetObjectByHandle(ObjectHandle handle) {
        return static_cast<T *>(GetObjectByHandle(handle));
    }
};

template<typename T, typename... Args>
    requires std::derived_from<T, Object>
T *CreateNewObject(Args &&...args) {
    Assert(IsMainThread(), "CreateNewObject can only be called in main thread, use CreateNewObjectAsync instead.");
    return ObjectManager::CreateNewObject<T>(Forward<Args>(args)...);
}
