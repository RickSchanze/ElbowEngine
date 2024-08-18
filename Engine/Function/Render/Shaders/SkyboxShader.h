/**
 * @file SkyboxShader.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/Shader.h"

FUNCTION_NAMESPACE_BEGIN

class SkyboxVertShader : public RHI::Vulkan::Shader
{
    DECLARE_VERT_SHADER(SkyboxVertShader)
public:
    void RegisterShaderVariables() override;
};

class SkyboxFragShader : public RHI::Vulkan::Shader
{
    DECLARE_FRAG_SHADER(SkyboxFragShader)
public:
    void RegisterShaderVariables() override;
};

FUNCTION_NAMESPACE_END
