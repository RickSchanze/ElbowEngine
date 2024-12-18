//
// Created by Echo on 24-12-18.
//

#pragma once
#include "Object.h"

namespace core
{

class ObjectPtrBase
{
public:
    void SetOuter(const ObjectHandle outer) { outer_ = outer; }

protected:
    ObjectHandle outer_ = InvalidObjectHandle;   // 即包含此成员的父对象
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
    using Type = T;

    ObjectPtr() = default;

    /**
     * 设置handle并调整引用关系
     * @param ptr
     */
    ObjectPtr(T* ptr);

    /**
     * 通过handle_查找Object表来看是否有效
     * 可以避免指针失效的问题
     * @return
     */
    [[nodiscard]] bool IsValid() const;

    /**
     * 查找Object表 获取对象
     * @return
     * */
    [[nodiscard]] T* Get() const;

    T* operator->() const;
    T& operator*() const;

    /**
     * SetHandle会调整目标Object和outer的引用关系
     * @param handle
     */
    void SetHandle(const ObjectHandle handle);

protected:
    ObjectHandle handle_ = InvalidObjectHandle;   // 指向的对象
};

template <typename T>
struct IsObjectPtr : std::false_type
{
};

template <typename T>
struct IsObjectPtr<ObjectPtr<T>> : std::true_type
{};

template <typename T>
struct ObjPtrTraits
{
    using Type = typename T::Type;
};
}