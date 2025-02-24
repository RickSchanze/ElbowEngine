//
// Created by Echo on 25-2-23.
//

#pragma once
#include "Core/Assert.h"
#include "CoreTypeDef.h"

namespace core {

// ArrayProxy是一个数组的代理, 传入一个指针和一个大小,
// 要求指针内容连续且大小必须小于等于指针实际大小
// 不会进行内存分配操作
template <typename T>
  requires std::is_trivially_copyable_v<T>
class ArrayProxy {
public:
  ArrayProxy() : ptr_(0), preserved_size_(0), size_(0) {}
  ArrayProxy(T *ptr, UInt64 preserved_size, UInt64 size = 0)
      : ptr_(ptr), preserved_size_(preserved_size), size_(size) {}

  void Add(const T &item) {
    Assert::Require("Core", size_ < preserved_size_, "ArrayProxy已满");
    if (size_ < preserved_size_) {
      ptr_[size_] = item;
      size_++;
    }
  }

  UInt64 Size() const { return size_; }
  UInt64 Capacity() const { return preserved_size_; }
  // 剩下元素的大小
  UInt64 Remain() const { return preserved_size_ - size_; }
  void Clear() { size_ = 0; }

  T &operator[](UInt64 index) const { return ptr_[index]; }

private:
  T *ptr_ = nullptr;
  UInt64 preserved_size_ = 0;
  UInt64 size_ = 0;
};

} // namespace core
