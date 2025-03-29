//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Core.hpp"
#include "Object.hpp"


struct ObjectPtrBase {
    typedef ObjectPtrBase ThisClass;
    static void ConstructSelf(void *self) { new (self) ObjectPtrBase(); }
    static void DestructSelf(void *self) { static_cast<ObjectPtrBase *>(self)->~ObjectPtrBase(); }
    const Type *GetType() { return TypeOf<ObjectPtrBase>(); }
    static const Type *GetStaticType() { return TypeOf<ObjectPtrBase, true>(); }
    static Type *ConstructType() { return Type::Create<ObjectPtrBase>("ObjectPtr") | refl_helper::AddField("object", &ObjectPtrBase::object_); }

private:
public:
    ~ObjectPtrBase() { SetObject(0); }

    /**
     * 设置"Outer"
     * class A : Object {
     *   ObjectPtr<B> b;
     * };
     * A a;
     * B b;
     * a.b = b;
     * 此时对于A.b来说, 其Outer为a
     * 因此Outer通常只由对象生成时设置一次, 之后不再修改
     * @param outer
     */
    void SetOuter(const ObjectHandle outer);

    /**
     * 设置object_
     * 如果此时outer_为空, 那么可能是局部变量, 此时不调整引用关系
     * 此函数应该永远只调用一次
     * @param handle
     */
    void SetObject(const ObjectHandle handle);

#if WITH_EDITOR
    /**
     * 将Outer从原来的变成现在的 同时修正引用关系
     * @param handle
     */
    void ModifyOuter(const ObjectHandle handle);
#endif

    ObjectHandle GetOuter() const { return outer_; }
    ObjectHandle GetHandle() const { return object_; }

protected:
    ObjectHandle outer_ = 0; // 即包含此成员的父对象
    ObjectHandle object_ = 0; // 引用的Object
};

REGISTER_TYPE(ObjectPtrBase)

namespace internal {
    Object *GetObjectFromObjectManager(ObjectHandle handle);
} // namespace internal

/**
 * ObjectPtr用于引用一个Object
 * 不支持嵌套ObjectPtr, 即不支持ObjectPtr<ObjectPtr<>>
 * 但是支持容器操作 即Array<ObjectPtr<>>
 * @tparam T
 */
template<typename T>
class ObjectPtr : public ObjectPtrBase {
public:
    static const Type *GetStaticType() { return TypeOf<ObjectPtrBase>(); }
    ObjectPtr() : ObjectPtr(nullptr) {}

    ObjectPtr(std::nullptr_t) {
        SetObject(0);
    }

    ObjectPtr(T *obj) {
        if (obj == nullptr) {
            SetObject(0);
        } else {
            SetObject(static_cast<Object *>(obj)->GetHandle());
        }
    }

    operator T *() {
        Object *obj = internal::GetObjectFromObjectManager(object_);
        return reinterpret_cast<T *>(obj);
    }

    operator T *() const {
        Object *obj = internal::GetObjectFromObjectManager(object_);
        return reinterpret_cast<T *>(obj);
    }

    T *operator->() { return static_cast<T *>(*this); }
    const T *operator->() const { return static_cast<T *>(*this); }

    T &operator*() { return *static_cast<T *>(*this); }

    operator bool() { return static_cast<T *>(*this) != nullptr; }
    operator bool() const { return static_cast<T *>(*this) != nullptr; }

    ObjectPtr &operator=(const ObjectPtr &other) {
        if (this == &other)
            return *this;
        SetObject(other.object_);
        return *this;
    }

    ObjectPtr &operator=(const T *other) {
        if (other == nullptr)
            return *this;
        SetObject(other->GetHandle());
        return *this;
    }
};

template<typename T>
struct IsObjectPtr : std::false_type {};

template<typename T>
struct IsObjectPtr<ObjectPtr<T>> : std::true_type {};

template<typename T>
struct ObjPtrTraits {
    using Type = typename T::Type;
};
