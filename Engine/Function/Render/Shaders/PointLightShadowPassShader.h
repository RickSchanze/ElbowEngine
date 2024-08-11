/**
 * @file PointLightShadowPassShader.h
 * @author Echo 
 * @Date 24-8-11
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/Shader.h"
#include "type_traits"

FUNCTION_NAMESPACE_BEGIN

class PointLightShadowPassVertShader : public RHI::Vulkan::Shader
{
    DECLARE_VERT_SHADER(PointLightShadowPassVertShader)
public:
    void RegisterShaderVariables() override;
};

class PointLightShadowPassFragShader : public RHI::Vulkan::Shader
{
    DECLARE_FRAG_SHADER(PointLightShadowPassFragShader)
public:
    void RegisterShaderVariables() override;
};

FUNCTION_NAMESPACE_END
