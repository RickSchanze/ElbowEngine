//
// Created by Echo on 2025/3/22.
//

#pragma once

namespace NRHI {
class IResource {
public:
  virtual ~IResource() = default;

  virtual void *GetNativeHandle() const = 0;

  virtual bool IsValid() const { return GetNativeHandle() != nullptr; }

  template <typename T> T GetNativeHandleT() const { return static_cast<T>(GetNativeHandle()); }
};
} // namespace rhi
