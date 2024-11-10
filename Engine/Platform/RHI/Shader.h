/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "IRHIResource.h"

namespace platform::rhi
{
enum class ShaderStage
{
    Vertex,
    Fragment,
    Compute,
    Count,
};

class Shader : public IRHIResource
{
public:
    ~Shader() override = default;

    [[nodiscard]] void* GetNativePtr() const override;
    [[nodiscard]] bool  IsValid() const override;

private:
    // vulkan: ShaderModule
    void*       native_ptr_ = nullptr;
    ShaderStage stage_ = ShaderStage::Count;
};
}   // namespace platform::rhi
