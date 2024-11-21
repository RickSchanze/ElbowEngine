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

bool core::Any::IsPrimitive() const
{
    return GetType()->IsPrimitive();
}

bool core::Any::IsEnum() const
{
    return GetType()->IsEnum();
}

bool core::CanConvertTo(const Type* from, const Type* to)
{
    if (from == to) return true;
    if (from->IsEnum() && to == TypeOf<int32_t>()) return true;
    return false;
}
