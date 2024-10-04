/**
 * @file Event.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once

#include "Event/Event.h"


namespace rhi::vulkan
{
class ImguiGraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

struct RequireImGuiGraphicsPipelineEvent : TEvent<rhi::vulkan::ImguiGraphicsPipeline** /** out_pipeline */>
{
};

inline RequireImGuiGraphicsPipelineEvent OnRequireImGuiGraphicsPipeline;

FUNCTION_NAMESPACE_END
