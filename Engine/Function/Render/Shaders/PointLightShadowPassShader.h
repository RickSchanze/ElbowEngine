/**
 * @file PointLightShadowPassShader.h
 * @author Echo 
 * @Date 24-8-11
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Render/Shader.h"
#include "type_traits"

namespace function {

class PointLightShadowPassVertShader : public rhi::vulkan::Shader
{
    DECLARE_VERT_SHADER(PointLightShadowPassVertShader)
public:
    void RegisterShaderVariables() override;
};

class PointLightShadowPassFragShader : public rhi::vulkan::Shader
{
    DECLARE_FRAG_SHADER(PointLightShadowPassFragShader)
public:
    void RegisterShaderVariables() override;
};

}
