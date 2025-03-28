//
// Created by Echo on 2025/3/26.
//

#include "SharedAny.hpp"
#include "Core/Assert.hpp"
#include "Any.hpp"

SharedAny::SharedAny(const Type *t) {
    Assert(t != nullptr, "Type不能为空");
    type_ = t;
    data_ = Malloc(t->GetSize());
    ReflManager::Get()->ConstructAt(type_, data_);
    ref_cnt_ = New<int>(1);
}

SharedAny::SharedAny(const SharedAny &other) {
    data_ = other.data_;
    ref_cnt_ = other.ref_cnt_;
    type_ = other.type_;
    AddRef();
}

SharedAny::SharedAny(SharedAny &&other) noexcept {
    data_ = other.data_;
    ref_cnt_ = other.ref_cnt_;
    type_ = other.type_;
    other.data_ = nullptr;
    other.ref_cnt_ = nullptr;
}

SharedAny &SharedAny::operator=(SharedAny &&other) noexcept {
    if (this == &other)
        return *this;
    Release();
    data_ = other.data_;
    ref_cnt_ = other.ref_cnt_;
    other.data_ = nullptr;
    other.ref_cnt_ = nullptr;
    return *this;
}
