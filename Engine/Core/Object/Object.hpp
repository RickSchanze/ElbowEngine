//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/Core.hpp"

#include GEN_HEADER("Object.generated.hpp")

using ObjectHandle = Int32;


enum EENUM(Flag) ObjectFlagBits {
    OFT_Persistent = 1 << 1, // 此对象需要持久化存储
    OFT_Actor = 1 << 2,
    OFT_Component = 1 << 3,
    OFT_Widget = 1 << 4,
};
using ObjectFlag = Int32;


enum EENUM(Flag) ObjectStateBits {
    PendingKill,
};

using ObjectState = Int32;

/**
 * Object不自动生成默认构造函数
 * TODO: Destroy
 */
class ECLASS() Object {
    friend struct ObjectPtrBase;
    friend struct ObjectRegistry;

    GENERATED_BODY(Object)

public:
    explicit Object(const ObjectFlag flag) : mFlags(flag) { GenerateInstanceHandle(); }

    Object() { GenerateInstanceHandle(); }

    virtual ~Object();

    void SetDisplayName(const StringView display_name) {
#if WITH_EDITOR
        mDisplayName = display_name;
#endif
    }

    void SetName(const StringView name) { mName = name; }

    [[nodiscard]] StringView GetName() const { return mName; }
    [[nodiscard]] StringView GetDisplayName() const { return mDisplayName; }

    void SetObjectHandle(const Int32 handle) { mHandle = handle; }

protected:
    EFIELD()
    Int32 mHandle = 0;

    EFIELD()
    ObjectFlag mFlags = 0;

    EFIELD(Transient)
    ObjectState mState = 0;

    EFIELD()
    String mName{};

#if WITH_EDITOR
    EFIELD(Transient)
    String mDisplayName{"空对象"};
#endif

private:
    // 此Object正在引用的对象, 用Array是因为可能会被同一个Object多次引用
    Array<ObjectHandle> referencing_;
    // 哪些Object引用了此Object
    Array<ObjectHandle> referenced_;

    void AddReferencing(const ObjectHandle handle) { referencing_.Add(handle); }
    void RemoveReferencing(const ObjectHandle handle) { referencing_.Remove(handle); }
    void AddReferenced(ObjectHandle handle) { referenced_.Add(handle); }
    void RemoveReferenced(ObjectHandle handle) { referenced_.Remove(handle); }

    void GenerateInstanceHandle();
    /**
     * 只是从ObjectArray中移除自身而不销毁
     */
    void UnregisterSelf() const;
    void RegisterSelf();
    void ResolveObjectPtr();

public:
    virtual void PostSerialized() {}
    virtual void PreSerialized() {}

    /**
     * 默认的实现中, 会遍历所有成员, 将由
     * ObjectPtr包裹的成语初始化
     */
    virtual void PostDeserialized() { RegisterSelf(); }
    virtual void PreDeserialized() {}

    /**
     * 通过NewObject创建出来的对象不是Persistent对象, 不应该被序列化
     * 此时由此函数来生成ObjectHandle, 以及初始化ObjectPtr
     */
    virtual void OnCreated() {
        RegisterSelf();
        ResolveObjectPtr();
    }

    virtual void OnDestroyed();

    [[nodiscard]] bool IsPendingKill() const { return mState & PendingKill; }

    [[nodiscard]] ObjectHandle GetHandle() const { return mHandle; }

    [[nodiscard]] bool IsPersistent() const { return mFlags & OFT_Persistent; }

    void InternalSetAssetHandle(ObjectHandle handle);

    // TODO: 这里是否应该返回一个sender?
    exec::ExecFuture<ObjectHandle> PerformPersistentObjectLoadAsync();

    ObjectHandle PerformPersistentObjectLoad();
};

template<typename T>
T *Cast(Object *obj) {
    auto obj_type = obj->GetType();
    if (auto t_type = TypeOf<T>(); t_type->IsDerivedFrom(obj_type)) {
        return reinterpret_cast<T *>(obj);
    }
    return nullptr;
}

template<typename T, typename... Args>
    requires std::is_base_of_v<Object, T>
T *NewObject(Args &&...args) {
    Assert(IsMainThread(), "对象只能在主线程创建!");
    T *obj = New<T>(Forward<Args>(args)...);
    obj->OnCreated();
    return obj;
}

void Destroy(Object *obj);
