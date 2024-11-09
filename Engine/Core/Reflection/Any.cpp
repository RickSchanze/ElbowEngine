/**
 * @file Any.cpp
 * @author Echo 
 * @Date 24-11-7
 * @brief 
 */

#include "Any.h"
#include "Reflection.h"

core::Any::Any(void* data, const Type* data_type)
{
    ptr_ = New<TypeLessData>(data, data_type);
}

core::Any::Any(const void* data, const Type* data_type)
{
    ptr_ = New<TypeLessData>(const_cast<void*>(data), data_type);
}

bool core::Any::IsDerivedFrom(const Type* t) const
{
    return GetType()->IsDerivedFrom(t);
}

bool core::Any::IsParentOf(const Type* t) const
{
    if (t == nullptr) return false;
    return t->IsDerivedFrom(GetType());
}
void core::LogTypeNotSameError(const Type* t1, const Type* t2)
{
    LOGGER.Error(
        logcat::Reflection,
        "If T is a copyable type, then TypeOf<T> == GetType() must be true, but TypeOf<T>() = {}, GetType() = {}",
        t1->GetName(),
        t2->GetName()
    );
}
