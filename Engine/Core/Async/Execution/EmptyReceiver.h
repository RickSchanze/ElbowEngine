//
// Created by Echo on 25-3-8.
//

#pragma once

namespace core::exec {

template <typename T> struct EmptyReceiver {
  using receive_type = T;

  void SetValue(T &&t) {}
  void SetException(std::exception_ptr ptr) { std::rethrow_exception(ptr); }
};

} // namespace core::exec
