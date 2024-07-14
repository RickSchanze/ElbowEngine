/**
 * @file ImageView.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "ImageView.h"

#include "RHI/Vulkan/Render/LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"
RHI::Vulkan::ImageView::ImageView(const vk::ImageView& handle, const char* debug_name)
{
    handle_ = handle;
#ifdef ELBOW_DEBUG
    if (debug_name)
    {
        debug_name_ = debug_name;
        VulkanContext::Get()->GetLogicalDevice()->SetImageViewDebugName(handle, debug_name_.data());
    }
#endif
}

RHI::Vulkan::ImageView::~ImageView()
{
    InternalDestroy();
}

void RHI::Vulkan::ImageView::InternalDestroy()
{
    if (!IsValid()) return;
    VulkanContext::Get()->GetLogicalDevice()->GetHandle().destroy(handle_);
    handle_ = VK_NULL_HANDLE;
}

void RHI::Vulkan::ImageView::Destroy()
{
    InternalDestroy();
}
