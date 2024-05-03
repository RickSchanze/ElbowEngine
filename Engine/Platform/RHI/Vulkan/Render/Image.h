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

    virtual bool IsValid() const { return static_cast<bool>(mImageHandle); }

    vk::Image GetHandle() const { return mImageHandle; }

protected:
    vk::Image mImageHandle = VK_NULL_HANDLE;
};

// SwapChainImage不需要自己销毁 SwapChain销毁时会自动销毁
class SwapChainImage final : public ImageBase {
public:
    explicit SwapChainImage(const vk::Image& InImgHandle) : ImageBase(InImgHandle) {}

    SwapChainImage() = default;
};

struct ImageCreateInfo
{
    vk::Format              Format;
    vk::ImageUsageFlags     Usage;
    uint32                  Width;
    uint32                  Height;
    uint32                  Depth          = 1;
    uint32                  MipLevels      = 1;
    vk::ImageTiling         Tiling         = vk::ImageTiling::eOptimal;
    vk::SampleCountFlagBits SampleCount    = vk::SampleCountFlagBits::e1;
    vk::ImageType           ImageType      = vk::ImageType::e2D;
    vk::MemoryPropertyFlags MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
};

class Image final : public ImageBase {
protected:
    struct Protected
    {};

public:
    typedef ImageBase Super;

    explicit Image(Protected, Ref<UniquePtr<LogicalDevice>> InDevice, const ImageCreateInfo& InCreateInfo);

    static SharedPtr<Image> CreateShared(Ref<UniquePtr<LogicalDevice>> InDevice, const ImageCreateInfo& InCreateInfo);

    ~Image() override;

    bool IsValid() const override;

    void Finialize();

private:
    Ref<UniquePtr<LogicalDevice>> mDevice;

    vk::DeviceMemory mImageMemory = nullptr;
};

RHI_VULKAN_NAMESPACE_END
