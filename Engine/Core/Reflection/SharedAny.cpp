//
// Created by Echo on 24-12-17.
//

#include "SharedAny.h"
#include "Core/Memory/MemoryManager.h"
#include "CtorManager.h"

core::SharedAny::SharedAny(const Type *t) {
  Assert::Require(logcat::Reflection, t != nullptr, "Type不能为空");
  type_ = t;
  data_ = MemoryManager::Allocate(t->GetSize());
  CtorManager::Get()->ConstructAt(type_, data_);
  ref_cnt_ = New<int>(1);
}

core::SharedAny::SharedAny(const SharedAny &other) {
  data_ = other.data_;
  ref_cnt_ = other.ref_cnt_;
  type_ = other.type_;
  AddRef();
}

core::SharedAny::SharedAny(SharedAny &&other) noexcept {
  data_ = other.data_;
  ref_cnt_ = other.ref_cnt_;
  type_ = other.type_;
  other.data_ = nullptr;
  other.ref_cnt_ = nullptr;
}

core::SharedAny &core::SharedAny::operator=(const SharedAny &other) {
  if (this == &other)
    return *this;
  Release();
  data_ = other.data_;
  ref_cnt_ = other.ref_cnt_;
  AddRef();
  return *this;
}

core::SharedAny &core::SharedAny::operator=(SharedAny &&other) noexcept {
  if (this == &other)
    return *this;
  Release();
  data_ = other.data_;
  ref_cnt_ = other.ref_cnt_;
  other.data_ = nullptr;
  other.ref_cnt_ = nullptr;
  return *this;
}

core::SharedAny::~SharedAny() { Release(); }

core::Any core::SharedAny::AsAny() const { return Any(data_, type_); }

void core::SharedAny::Release() {
  if (ref_cnt_ == nullptr)
    return;
  --(*ref_cnt_);
  if (*ref_cnt_ == 0) {
    CtorManager::Get()->DestroyAt(type_, data_);
    Delete(data_);
    Delete(ref_cnt_);
  }
  data_ = nullptr;
  ref_cnt_ = nullptr;
}

void core::SharedAny::AddRef() const {
  if (ref_cnt_ != nullptr) {
    ++(*ref_cnt_);
  }
}