/**
 * @file StandardForwardShader.h
 * @author Echo 
 * @Date 24-8-1
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/Shader.h"

FUNCTION_NAMESPACE_BEGIN

class StandardForwardVertShader : public RHI::Vulkan::Shader
{
    DECLARE_VERT_SHADER(StandardForwardVertShader)

public:
    void RegisterShaderVariables() override;
};

class StandardForwardFragShader : public RHI::Vulkan::Shader
{
    DECLARE_FRAG_SHADER(StandardForwardFragShader)

public:
    void RegisterShaderVariables() override;
};

FUNCTION_NAMESPACE_END
