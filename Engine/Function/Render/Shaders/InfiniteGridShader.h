/**
 * @file InfiniteGridShader.h
 * @author Echo 
 * @Date 24-9-22
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Render/Shader.h"

namespace function
{
class InfiniteGridVertShader : public rhi::vulkan::Shader
{
public:
    void RegisterShaderVariables() override;

private:
    DECLARE_VERT_SHADER(InfiniteGridVertShader);
};

class InfiniteGridFragShader : public rhi::vulkan::Shader
{
public:
    void RegisterShaderVariables() override;

private:
    DECLARE_FRAG_SHADER(InfiniteGridFragShader);
};
}   // namespace function
