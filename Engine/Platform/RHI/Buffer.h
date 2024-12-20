//
// Created by Echo on 24-12-20.
//

#pragma once
#include "Enums.h"
#include "IResource.h"


namespace platform::rhi
{

struct BufferCreateInfo
{
    size_t               size{};
    BufferUsage          usage{};
    BufferMemoryProperty memory_property{};

    BufferCreateInfo(const size_t size_, const BufferUsage usage_, const BufferMemoryProperty memory_property_) :
        size(size_), usage(usage_), memory_property(memory_property_)
    {
    }
};

class Buffer : IResource
{
public:
    explicit Buffer(const BufferCreateInfo& create_info) : create_info_(create_info) {}

    [[nodiscard]] size_t      GetSize() const { return create_info_.size; }
    [[nodiscard]] BufferUsage GetUsage() const { return create_info_.usage; }
protected:
    BufferCreateInfo create_info_;
};
}   // namespace platform::rhi
