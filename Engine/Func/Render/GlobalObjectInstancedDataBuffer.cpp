//
// Created by Echo on 2025/3/27.
//

#include "GlobalObjectInstancedDataBuffer.hpp"

#include "Core/Memory/Memory.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/VertexLayout.hpp"
using namespace rhi;

Buffer *GlobalObjectInstancedDataBuffer::GetBuffer() { return GetByRef().buffer_.get(); }

void GlobalObjectInstancedDataBuffer::Startup() {
    const BufferDesc desc{sizeof(InstancedData1), BUB_VertexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
    buffer_ = GetGfxContextRef().CreateBuffer(desc, "GlobalInstancedDataBuffer");
    mapped_memory_ = (UInt8 *) buffer_->BeginWrite();
    InstancedData1 data1{};
    data1.scale = {1, 1, 1};
    data1.location = {0, 0, 0};
    Memcpy(mapped_memory_, &data1, sizeof(data1));
}

void GlobalObjectInstancedDataBuffer::Shutdown() {
    GetGfxContext()->WaitForDeviceIdle();
    buffer_->EndWrite();
    buffer_ = nullptr;
    mapped_memory_ = nullptr;
}
