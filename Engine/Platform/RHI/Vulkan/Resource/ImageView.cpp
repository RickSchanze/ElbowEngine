/**
 * @file ImageView.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "ImageView.h"

#include "RHI/Vulkan/Render/LogicalDevice.h"
RHI::Vulkan::ImageView::~ImageView() {
    if (IsValid()) {
        Finialize();
    }
}

void RHI::Vulkan::ImageView::Finialize() {
    if (!IsValid()) return;
    if (mAssociatedLogicalDevice == nullptr || !mAssociatedLogicalDevice->IsValid()) return;
    mAssociatedLogicalDevice->GetHandle().destroy(mViewHandle);
    mViewHandle = VK_NULL_HANDLE;
}