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

namespace function {

struct RequireImGuiGraphicsPipelineEvent : TEvent<rhi::vulkan::ImguiGraphicsPipeline** /** out_pipeline */>
{
};

inline RequireImGuiGraphicsPipelineEvent OnRequireImGuiGraphicsPipeline;

}
