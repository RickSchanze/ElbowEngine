/**
 * @file Image.h
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN

class Image {
public:
    Image() = default;

    // 使用一个hanlde来初始化此Image
    explicit Image(const vk::Image& InImgHandle) : mImageHandle(InImgHandle) {}

    [[nodiscard]] bool IsValid() const { return static_cast<bool>(mImageHandle); }

private:
    vk::Image mImageHandle;
};

RHI_VULKAN_NAMESPACE_END
