//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/TypeAlias.hpp"


namespace rhi {
    class Buffer;
}
class GlobalObjectInstancedDataBuffer : public Manager<GlobalObjectInstancedDataBuffer> {
public:
    [[nodiscard]] Float GetLevel() const override { return 8; }
    [[nodiscard]] StringView GetName() const override { return "GlobalObjectInstancedDataBuffer"; }

    static rhi::Buffer *GetBuffer();

    void Startup() override;
    void Shutdown() override;

private:
    SharedPtr<rhi::Buffer> buffer_;
    UInt8 *mapped_memory_ = nullptr;
};
