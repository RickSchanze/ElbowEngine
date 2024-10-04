/**
 * @file SkyboxShader.h
 * @author Echo 
 * @Date 24-8-24
 * @brief 
 */

#pragma once

#include "RHI/Vulkan/Render/Shader.h"

namespace function {

class SkyboxVertShader : public rhi::vulkan::Shader
{
public:
    void RegisterShaderVariables() override;

private:
    DECLARE_VERT_SHADER(SkyboxVertShader)
};

class SkyboxFragShader : public rhi::vulkan::Shader
{
public:
    void RegisterShaderVariables() override;

private:
    DECLARE_FRAG_SHADER(SkyboxFragShader)
};

}
