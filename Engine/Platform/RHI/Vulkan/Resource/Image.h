/**
 * @file Image.h
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#pragma once
#include "ImageView.h"
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace Resource {
class Texture;
}
namespace RHI::Vulkan {
class CommandPool;
}

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
    vk::Format              Format{};
    vk::ImageUsageFlags     Usage{};
    Int32                   Width          = 0;
    Int32                   Height         = 0;
    Int32                   Depth          = 1;
    UInt32                  MipLevels      = 1;
    vk::ImageTiling         Tiling         = vk::ImageTiling::eOptimal;
    vk::SampleCountFlagBits SampleCount    = vk::SampleCountFlagBits::e1;
    vk::ImageType           ImageType      = vk::ImageType::e2D;
    vk::MemoryPropertyFlags MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;

             ImageCreateInfo() = default;
    explicit ImageCreateInfo(const vk::ImageCreateInfo& InVkImageInfo);
};

class Image : public ImageBase, public IRHIResource {
protected:
    struct Protected
    {};

public:
    typedef ImageBase Super;

    explicit Image(Protected, Ref<LogicalDevice> InDevice, const ImageCreateInfo& InCreateInfo);

    static TSharedPtr<Image> CreateShared(Ref<LogicalDevice> InDevice, const ImageCreateInfo& InCreateInfo);

    void Destroy() override;

    ~Image() override;

    bool IsValid() const override;
    void Finialize();

    UInt32 GetWidth() const { return mCreateInfo.Width; }
    UInt32 GetHeight() const { return mCreateInfo.Height; }
    UInt32 GetDepth() const { return mCreateInfo.Depth; }

protected:
    void CreateImage();

protected:
    explicit Image(const Ref<LogicalDevice> InDevice) : mDevice(InDevice) {}

    Ref<LogicalDevice> mDevice;

    vk::DeviceMemory mImageMemory = nullptr;
    ImageCreateInfo  mCreateInfo{};
};

class Texture : public Image {
public:
    Texture(Protected, Ref<LogicalDevice> InDevice, const CommandPool& InCommandProducer, Resource::Texture* InTexture);

    static TSharedPtr<Texture> Create(Ref<LogicalDevice> InDevice, const CommandPool& InCommandProducer, Resource::Texture* InTexture);

    Int32 GetMipLevel() const { return mMipLevel; }

protected:
    Int32 mMipLevel;
};

RHI_VULKAN_NAMESPACE_END
