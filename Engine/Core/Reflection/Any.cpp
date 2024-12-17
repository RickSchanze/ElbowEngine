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
    if (GetType() == TypeOf<int8_t>()) return *static_cast<int8_t*>(ptr_->GetData());
    if (GetType() == TypeOf<int16_t>()) return *static_cast<int16_t*>(ptr_->GetData());
    if (GetType() == TypeOf<int32_t>()) return *static_cast<int32_t*>(ptr_->GetData());
    if (GetType() == TypeOf<int64_t>()) return *static_cast<int64_t*>(ptr_->GetData());
    if (GetType() == TypeOf<uint8_t>()) return *static_cast<uint8_t*>(ptr_->GetData());
    if (GetType() == TypeOf<uint16_t>()) return *static_cast<uint16_t*>(ptr_->GetData());
    if (GetType() == TypeOf<uint32_t>()) return *static_cast<uint32_t*>(ptr_->GetData());
    if (GetType() == TypeOf<uint64_t>()) return *static_cast<uint64_t*>(ptr_->GetData());
    if (GetType() == TypeOf<bool>()) return *static_cast<bool*>(ptr_->GetData());
    return {};
}

core::Optional<double> core::Any::AsDouble() const
{
    if (GetType() == TypeOf<float>()) return *static_cast<float*>(ptr_->GetData());
    if (GetType() == TypeOf<double>()) return *static_cast<double*>(ptr_->GetData());
    return {};
}

bool core::CanConvertCopy(const Type* from, const Type* to)
{
    if (from == nullptr || to == nullptr) return false;
    if (from == to) return true;
    if (from->IsEnum() && to == TypeOf<int32_t>()) return true;
    if (from->IsNumericInteger() && to->IsNumericInteger()) return true;
    if (from->IsNumericFloat() && to->IsNumericFloat()) return true;
    return false;
}
