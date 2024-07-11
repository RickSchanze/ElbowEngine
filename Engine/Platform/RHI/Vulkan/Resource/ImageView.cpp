/**
 * @file ImageView.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "ImageView.h"

#include "RHI/Vulkan/Render/LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"
RHI::Vulkan::ImageView::~ImageView()
{
    InternalDestroy();
}

void RHI::Vulkan::ImageView::InternalDestroy()
{
    if (!IsValid()) return;
    VulkanContext::Get()->GetLogicalDevice()->GetHandle().destroy(mViewHandle);
    mViewHandle = VK_NULL_HANDLE;
}

void RHI::Vulkan::ImageView::Destroy()
{
    InternalDestroy();
}
