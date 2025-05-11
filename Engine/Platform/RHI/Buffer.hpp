//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Core.hpp"
#include "Enums.hpp"
#include "IResource.hpp"

namespace NRHI {
    struct BufferDesc {
        size_t size{};
        BufferUsage usage{};
        BufferMemoryProperty memory_property{};

        BufferDesc(const size_t size_, const BufferUsage usage_, const BufferMemoryProperty memory_property_) :
            size(size_), usage(usage_), memory_property(memory_property_) {}
    };

    class Buffer : public IResource {
    public:
        explicit Buffer(const BufferDesc &create_info) : create_info_(create_info) {}

        size_t GetSize() const { return create_info_.size; }
        BufferUsage GetUsage() const { return create_info_.usage; }

        virtual UInt8 *BeginWrite() = 0;
        virtual void EndWrite() = 0;
        virtual UInt8 *BeginRead() = 0;
        virtual void EndRead() = 0;

    protected:
        BufferDesc create_info_;
    };
} // namespace rhi
