//
// Created by Echo on 2025/3/21.
//
#include "Any.hpp"

#include "Helper.hpp"

Any &Any::operator=(const Any &rhs) {
    if (this == &rhs)
        return *this;
    Delete(ptr_);
    ptr_ = rhs.ptr_->Clone();
    return *this;
}

Any &Any::operator=(Any &&rhs) noexcept {
    Delete(ptr_);
    ptr_ = rhs.ptr_;
    rhs.ptr_ = nullptr;
    return *this;
}

bool Any::IsPrimitive() const { return refl_helper::IsPrimitive(GetType()); }

Optional<Int64> Any::AsInt64() const {
    if (GetType() == nullptr || ptr_ == nullptr || ptr_->GetData() == nullptr)
        return {};
    if (GetType()->IsEnumType())
        return *static_cast<Int32 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int8>())
        return *static_cast<Int8 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int16>())
        return *static_cast<Int16 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int32>())
        return *static_cast<Int32 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int64>())
        return *static_cast<Int64 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt8>())
        return *static_cast<UInt8 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt16>())
        return *static_cast<UInt16 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt32>())
        return *static_cast<UInt32 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt64>())
        return *static_cast<UInt64 *>(ptr_->GetData());
    if (GetType() == TypeOf<bool>())
        return *static_cast<bool *>(ptr_->GetData());
    return {};
}

Optional<double> Any::AsDouble() const {
    if (GetType() == nullptr || ptr_ == nullptr || ptr_->GetData() == nullptr)
        return {};
    if (GetType()->IsEnumType())
        return *static_cast<Int32 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int8>())
        return *static_cast<Int8 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int16>())
        return *static_cast<Int16 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int32>())
        return *static_cast<Int32 *>(ptr_->GetData());
    if (GetType() == TypeOf<Int64>())
        return *static_cast<Int64 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt8>())
        return *static_cast<UInt8 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt16>())
        return *static_cast<UInt16 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt32>())
        return *static_cast<UInt32 *>(ptr_->GetData());
    if (GetType() == TypeOf<UInt64>())
        return *static_cast<UInt64 *>(ptr_->GetData());
    if (GetType() == TypeOf<bool>())
        return *static_cast<bool *>(ptr_->GetData());
    if (GetType() == TypeOf<Float>())
        return *static_cast<Float *>(ptr_->GetData());
    if (GetType() == TypeOf<Double>())
        return *static_cast<Double *>(ptr_->GetData());
    return {};
}
