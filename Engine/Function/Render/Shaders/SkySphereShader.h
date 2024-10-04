/**
 * @file SkySphereShader.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/Shader.h"

FUNCTION_NAMESPACE_BEGIN

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

FUNCTION_NAMESPACE_END
