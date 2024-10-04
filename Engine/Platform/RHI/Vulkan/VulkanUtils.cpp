/**
 * @file VulkanUtils.cpp
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#include "VulkanUtils.h"

#include "glm/glm.hpp"
#include "VulkanContext.h"

namespace rhi::vulkan
{
size_t VulkanUtils::GetDynamicUniformModelAlignment()
{
    static size_t dynamic_aligment = 0;
    if (dynamic_aligment == 0)
    {
        dynamic_aligment           = sizeof(glm::mat4);
        uint32_t min_ubo_alignment = VulkanContext::Get()->GetMinUniformBufferOffsetAlignment();
        if (min_ubo_alignment > 0)
        {
            dynamic_aligment = (dynamic_aligment + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
        }
    }
    return dynamic_aligment;
}
}
