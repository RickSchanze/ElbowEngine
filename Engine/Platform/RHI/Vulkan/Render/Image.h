/**
 * @file Image.h
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#pragma once
#include "ImageView.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN

class ImageBase {
public:
             ImageBase() = default;
    virtual ~ImageBase();

    // 使用一个hanlde来初始化此Image
    explicit ImageBase(const vk::Image& InImgHandle) : mImageHandle(InImgHandle) {}

    bool IsValid() const { return static_cast<bool>(mImageHandle); }

    vk::Image GetHandle() const { return mImageHandle; }

private:
    vk::Image mImageHandle = VK_NULL_HANDLE;
};

// SwapChainImage不需要自己销毁 SwapChain销毁时会自动销毁
class SwapChainImage final : public ImageBase {
public:
    explicit SwapChainImage(const vk::Image& InImgHandle) : ImageBase(InImgHandle) {}

    SwapChainImage() = default;
};

RHI_VULKAN_NAMESPACE_END
