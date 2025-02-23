//
// Created by Echo on 24-12-20.
//

#pragma once
#include "Enums.h"
#include "IResource.h"

namespace platform::rhi {

struct BufferDesc {
  size_t size{};
  BufferUsage usage{};
  BufferMemoryProperty memory_property{};

  BufferDesc(const size_t size_, const BufferUsage usage_, const BufferMemoryProperty memory_property_)
      : size(size_), usage(usage_), memory_property(memory_property_) {}
};

class Buffer : public IResource {
public:
  explicit Buffer(const BufferDesc &create_info) : create_info_(create_info) {}

  [[nodiscard]] size_t GetSize() const { return create_info_.size; }
  [[nodiscard]] BufferUsage GetUsage() const { return create_info_.usage; }

  virtual void *BeginWrite() = 0;
  virtual void EndWrite() = 0;
  virtual void *BeginRead() = 0;
  virtual void EndRead() = 0;

  template <typename T> void WriteType(const T &data, UInt64 offset = 0) { Write(&data, sizeof(T), offset); }

protected:
  BufferDesc create_info_;
};
} // namespace platform::rhi
