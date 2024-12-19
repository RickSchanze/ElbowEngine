/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#pragma once

#include "Core/Base/EString.h"
#include "Core/Reflection/ITypeGetter.h"
#include "Core/Reflection/MetaInfoMacro.h"
#include "Core/Reflection/MetaInfoManager.h"

#include GEN_HEADER("Core.Object.generated.h")

namespace core
{

using ObjectHandle = int32_t;

constexpr inline ObjectHandle INVALID_OBJECT_HANDLE = 0;

enum ENUM(Flag) ObjectFlagType
{
    Persistent = 1 << 1,   // 此对象需要持久化存储
};
using ObjectFlag = int32_t;

enum ENUM(Flag) ObjectStateFlagType
{
    PendingKill,
};
using ObjectStateFlag = int32_t;

/**
 * Object不自动生成默认构造函数
 */
class CLASS() Object : public ITypeGetter
{
    GENERATED_CLASS(Object)
    friend class ObjectPtrBase;

protected:
    PROPERTY()
    String name_;

    PROPERTY()
    ObjectHandle handle_ = 0;

    PROPERTY()
    ObjectFlag flags_ = 0;

    PROPERTY()
    ObjectStateFlag state_ = 0;

private:
    // 此Object正在引用的对象, 用Array是因为可能会被同一个Object多次引用
    Array<ObjectHandle> referencing_;
    // 哪些Object引用了此Object
    Array<ObjectHandle> referenced_;

    void AddReferencing(ObjectHandle handle);
    void RemoveReferencing(ObjectHandle handle);
    void AddReferenced(ObjectHandle handle);
    void RemoveReferenced(ObjectHandle handle);

public:
    virtual void PostSerialized();
    virtual void PreSerialized() {}
    /**
     * 默认的实现中, 会遍历所有成员, 将由
     * ObjectPtr包裹的成语初始化
     */
    virtual void PostDeserialized();
    virtual void PreDeserialized() {}

    [[nodiscard]] bool IsPendingKill() const { return state_ & PendingKill; }

    [[nodiscard]] ObjectHandle GetHandle() const { return handle_; }
};

template <typename T>
T* Cast(Object* obj)
{
    auto obj_type = obj->GetType();
    if (auto t_type = TypeOf<T>(); t_type->IsDerivedFrom(obj_type))
    {
        return reinterpret_cast<T*>(obj);
    }
    return nullptr;
}

}   // namespace core
