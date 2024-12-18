//
// Created by Echo on 24-12-18.
//

#pragma once
#include "Core/Reflection/MetaInfoManager.h"
#include "Object.h"

#include GEN_HEADER("Core.ObjectPtr.generated.h")

namespace core
{

class CLASS() ObjectPtrBase
{
    GENERATED_CLASS(ObjectPtrBase)
public:
    void SetOuter(const ObjectHandle outer) { outer_ = outer; }

protected:
    ObjectHandle outer_ = InvalidObjectHandle;   // 即包含此成员的父对象

    PROPERTY()
    ObjectHandle object_ = InvalidObjectHandle;   // 引用的Object
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