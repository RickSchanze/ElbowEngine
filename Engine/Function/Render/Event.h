/**
 * @file Event.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once

#include "Core/Event/Event.h"


namespace rhi::vulkan
{
class ImguiGraphicsPipeline;
}

namespace function
{

DECLARE_EVENT(RequireImGuiGraphicsPipelineEvent, void, rhi::vulkan::ImguiGraphicsPipeline**);

inline RequireImGuiGraphicsPipelineEvent OnRequireImGuiGraphicsPipeline;

}   // namespace function
