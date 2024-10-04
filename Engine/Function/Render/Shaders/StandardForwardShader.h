/**
 * @file StandardForwardShader.h
 * @author Echo 
 * @Date 24-8-1
 * @brief 
 */

#pragma once

#include "RHI/Vulkan/Render/Shader.h"

namespace function {

class StandardForwardVertShader : public rhi::vulkan::Shader
{
    DECLARE_VERT_SHADER(StandardForwardVertShader)

public:
    void RegisterShaderVariables() override;
};

class StandardForwardFragShader : public rhi::vulkan::Shader
{
    DECLARE_FRAG_SHADER(StandardForwardFragShader)

public:
    void RegisterShaderVariables() override;
};

}
