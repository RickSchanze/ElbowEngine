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
    static size_t alignment = 0;
    if (alignment == 0)
    {
        alignment           = sizeof(glm::mat4);
        uint32_t min_ubo_alignment = VulkanContext::Get()->GetMinUniformBufferOffsetAlignment();
        if (min_ubo_alignment > 0)
        {
            alignment = (alignment + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
        }
    }
    return alignment;
}
}
