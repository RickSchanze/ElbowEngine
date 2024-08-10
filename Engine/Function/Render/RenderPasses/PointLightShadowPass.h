/**
 * @file PointLightShadowPass.h
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/RenderPass.h"

FUNCTION_NAMESPACE_BEGIN

class PointLightShadowPass : public RHI::Vulkan::RenderPass {

public:
    void SetupAttachments() override;
    void SetupSubpassDependency() override;
};

FUNCTION_NAMESPACE_END
