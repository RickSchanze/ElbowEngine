/**
 * @file SkySphereShader.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once

#include "RHI/Vulkan/Render/Shader.h"

namespace function {

class SkySphereVertShader : public rhi::vulkan::Shader
{
    DECLARE_VERT_SHADER(SkySphereVertShader)
public:
    void RegisterShaderVariables() override;
};

class SkySphereFragShader : public rhi::vulkan::Shader
{
    DECLARE_FRAG_SHADER(SkySphereFragShader)
public:
    void RegisterShaderVariables() override;
};

}
