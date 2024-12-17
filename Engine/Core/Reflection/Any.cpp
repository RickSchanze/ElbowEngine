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

core::Optional<int64_t> core::Any::AsInt64() const
{
    if (GetType() == TypeOf<int8_t>()) return AsCopy<int8_t>().value();
    if (GetType() == TypeOf<int16_t>()) return AsCopy<int16_t>().value();
    if (GetType() == TypeOf<int32_t>()) return AsCopy<int32_t>().value();
    if (GetType() == TypeOf<int64_t>()) return AsCopy<int64_t>().value();
    if (GetType() == TypeOf<uint8_t>()) return AsCopy<uint8_t>().value();
    if (GetType() == TypeOf<uint16_t>()) return AsCopy<uint16_t>().value();
    if (GetType() == TypeOf<uint32_t>()) return AsCopy<uint32_t>().value();
    if (GetType() == TypeOf<uint64_t>()) return AsCopy<uint64_t>().value();
    if (GetType() == TypeOf<bool>()) return AsCopy<bool>().value();
    return {};
}

bool core::CanConvertTo(const Type* from, const Type* to)
{
    if (from == to) return true;
    if (from->IsEnum() && to == TypeOf<int32_t>()) return true;
    return false;
}
