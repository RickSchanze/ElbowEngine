/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#pragma once

#include "Core/Base/EString.h"
#include "Core/CoreDef.h"
#include "Core/Reflection/ITypeGetter.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include GEN_HEADER("Core.Object.generated.h")

namespace core
{

class CLASS() Object : public ITypeGetter
{
    GENERATED_CLASS(Object)
public:
    virtual void PostSerialized() {}
    virtual void PreSerialized() {}

protected:
    PROPERTY()
    String name_;

#if WITH_EDITOR
    PROPERTY()
    String display_name_;
#endif
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

template <typename T>
    requires std::is_base_of_v<Object, T>
class ObjectPtr
{
public:
    ObjectPtr() = default;

    ObjectPtr(T* ptr) : ptr_(ptr) {}

    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* Get() const { return ptr_; }
    operator T*() const { return ptr_; }

    [[nodiscard]] const Type* GetObjectType() const
    {
        if (ptr_)
        {
            return ptr_->GetType();
        }
        return nullptr;
    }

private:
    Object* ptr_ = nullptr;
};

template <typename T>
struct ObjectPtrTrait
{
    static constexpr bool IsObjPtr() { return false; }
};

template <typename T>
struct ObjectPtrTrait<ObjectPtr<T>> : std::true_type
{
    using type = T;
    static constexpr bool IsObjPtr() { return true; }
};

}   // namespace core
