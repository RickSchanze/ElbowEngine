//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/Core.hpp"


using ObjectHandle = Int32;

enum ObjectFlagBits {
    OFT_Persistent = 1 << 1, // 此对象需要持久化存储
    OFT_Actor = 1 << 2,
    OFT_Component = 1 << 3,
    OFT_Widget = 1 << 4,
};
using ObjectFlag = Int32;

enum ObjectStateBits {
    PendingKill,
};

using ObjectState = Int32;

/**
 * Object不自动生成默认构造函数
 * TODO: Destroy
 */
class Object {
    friend struct ObjectPtrBase;
    friend struct ObjectRegistry;

public:
    typedef Object ThisClass;
    static Type *ConstructType() {
        const auto t = Type::Create<Object>("CoreConfig") | refl_helper::AddField("name", &Object::name_) |
                       refl_helper::AddField("flags", &Object::flags_) | refl_helper::AddField("state", &Object::state_) |
                       refl_helper::AddField("handle", &Object::handle_) EDITOR_ONLY(| refl_helper::AddField("display_name", &Object::display_name_));
        return t;
    }

    static void ConstructSelf(void *self) { new (self) Object(); }
    static void DestructSelf(void *self) { static_cast<Object *>(self)->~Object(); }
    virtual const Type *GetType() { return TypeOf<Object>(); }
    static const Type *GetStaticType() { return TypeOf<Object, true>(); }

public:
    explicit Object(const ObjectFlag flag) : flags_(flag) { GenerateInstanceHandle(); }

    Object() { GenerateInstanceHandle(); }

    virtual ~Object();

    void SetDisplayName(const StringView display_name) {
#if WITH_EDITOR
        display_name_ = display_name;
#endif
    }

    void SetName(const StringView name) { name_ = name; }

    [[nodiscard]] StringView GetName() const { return name_; }
    [[nodiscard]] StringView GetDisplayName() const { return display_name_; }

    void SetObjectHandle(const Int32 handle) { handle_ = handle; }

    virtual void AwakeFromLoad() {}

protected:
    Int32 handle_ = 0;
    ObjectFlag flags_ = 0;
    ObjectState state_ = 0;
    String name_{};

#if WITH_EDITOR
    String display_name_{"空对象"};
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

    [[nodiscard]] bool IsPendingKill() const { return state_ & PendingKill; }

    [[nodiscard]] ObjectHandle GetHandle() const { return handle_; }

    [[nodiscard]] bool IsPersistent() const { return flags_ & OFT_Persistent; }

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
    obj->AwakeFromLoad();
    return obj;
}
