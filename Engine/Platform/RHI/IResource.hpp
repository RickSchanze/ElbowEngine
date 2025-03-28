//
// Created by Echo on 2025/3/22.
//

#pragma once

namespace rhi {
class IResource {
public:
  virtual ~IResource() = default;

  [[nodiscard]] virtual void *GetNativeHandle() const = 0;

  [[nodiscard]] virtual bool IsValid() const { return GetNativeHandle() != nullptr; }

  template <typename T> [[nodiscard]] T GetNativeHandleT() const { return static_cast<T>(GetNativeHandle()); }
};
} // namespace rhi
