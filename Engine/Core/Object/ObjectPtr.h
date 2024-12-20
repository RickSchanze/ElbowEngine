//
// Created by Echo on 24-12-18.
//

#pragma once
#include "Core/Reflection/MetaInfoManager.h"
#include "Object.h"
#include "ObjectRegistry.h"

#include GEN_HEADER("Core.ObjectPtr.generated.h")


namespace core
{

class CLASS() ObjectPtrBase
{
    GENERATED_CLASS(ObjectPtrBase)
public:
    ~ObjectPtrBase();

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
     * @param handle
     */
    void SetObject(const ObjectHandle handle);

protected:
    ObjectHandle outer_ = INVALID_OBJECT_HANDLE;   // 即包含此成员的父对象

    PROPERTY()
    ObjectHandle object_ = INVALID_OBJECT_HANDLE;   // 引用的Object
};

/**
 * ObjectPtr用于引用一个Object
 * 不支持嵌套ObjectPtr, 即不支持ObjectPtr<ObjectPtr<>>
 * 但是支持容器操作 即Array<ObjectPtr<>>
 * @tparam T
 */
template <typename T>
    requires std::is_base_of_v<Object, T>
class ObjectPtr : public ObjectPtrBase
{
public:
    operator T*()
    {
        Object* obj = ObjectManager::GetObjectByHandle(object_);
        return static_cast<T*>(obj);
    }

    T* operator->() { return static_cast<T*>(this); }

    T& operator*() { return *static_cast<T*>(this); }

    operator bool() { return static_cast<T*>(this) != nullptr; }

    ObjectPtr& operator=(const ObjectPtr& other)
    {
        if (this == &other) return this;
        SetObject(other.object_);
        return this;
    }

    ObjectPtr& operator=(const T* other)
    {
        if (other == nullptr) return *this;
        SetObject(other->GetHandle());
        return *this;
    }
};

template <typename T>
struct IsObjectPtr : std::false_type
{
};

template <typename T>
struct IsObjectPtr<ObjectPtr<T>> : std::true_type
{
};

template <typename T>
struct ObjPtrTraits
{
    using Type = typename T::Type;
};
}