/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "Image.h"

#include "CommandProducer.h"
#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/PhysicalDevice.h"

RHI_VULKAN_NAMESPACE_BEGIN

ImageBase::~ImageBase() {
    mImageHandle = VK_NULL_HANDLE;
}

ImageCreateInfo::ImageCreateInfo(const vk::ImageCreateInfo& InVkImageInfo) {
    Format         = InVkImageInfo.format;
    Usage          = InVkImageInfo.usage;
    Width          = InVkImageInfo.extent.width;
    Height         = InVkImageInfo.extent.height;
    Depth          = InVkImageInfo.extent.depth;
    MipLevels      = InVkImageInfo.mipLevels;
    Tiling         = InVkImageInfo.tiling;
    SampleCount    = InVkImageInfo.samples;
    ImageType      = InVkImageInfo.imageType;
    MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
}

Image::Image(Protected, const Ref<LogicalDevice> InDevice, const ImageCreateInfo& InCreateInfo) :
    mDevice(InDevice), mCreateInfo(InCreateInfo) {
    CreateImage();
}

SharedPtr<Image> Image::CreateShared(Ref<LogicalDevice> InDevice, const ImageCreateInfo& InCreateInfo) {
    return MakeShared<Image>(Protected{}, InDevice, InCreateInfo);
}

Image::~Image() {
    Finialize();
}

bool Image::IsValid() const {
    return Super::IsValid() && mImageMemory != nullptr;
}

void Image::Finialize() {
    const auto& Device = mDevice.get();
    if (mImageHandle != nullptr) {
        Device.GetHandle().destroyImage(mImageHandle);
        mImageHandle = nullptr;
    }
    if (mImageMemory != nullptr) {
        Device.GetHandle().freeMemory(mImageMemory);
        mImageMemory = nullptr;
    }
}

void Image::CreateImage() {
    vk::ImageCreateInfo ImageCreateInfo{};
    const auto          DeviceHandle = mDevice.get().GetHandle();
    ImageCreateInfo.setImageType(mCreateInfo.ImageType);
    const vk::Extent3D Extent{
        static_cast<uint32>(mCreateInfo.Width),
        static_cast<uint32>(mCreateInfo.Height),
        static_cast<uint32>(mCreateInfo.ImageType == vk::ImageType::e3D ? 1 : mCreateInfo.Depth)
    };
    ImageCreateInfo.setExtent(Extent);
    ImageCreateInfo.setArrayLayers(1);
    ImageCreateInfo.setFormat(mCreateInfo.Format);
    ImageCreateInfo.setTiling(mCreateInfo.Tiling);
    ImageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    ImageCreateInfo.setUsage(mCreateInfo.Usage);
    ImageCreateInfo.setSamples(mCreateInfo.SampleCount);
    ImageCreateInfo.setMipLevels(mCreateInfo.MipLevels);
    // 只被一个队列族使用
    ImageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

    mImageHandle                              = DeviceHandle.createImage(ImageCreateInfo);
    // 为图像分配内存
    const auto             MemoryRequirements = DeviceHandle.getImageMemoryRequirements(mImageHandle);
    vk::MemoryAllocateInfo MemoryAllocateInfo{};
    MemoryAllocateInfo.setAllocationSize(MemoryRequirements.size);
    MemoryAllocateInfo.setMemoryTypeIndex(
        mDevice.get().GetAssociatedPhysicalDevice().FindMemoryType(MemoryRequirements.memoryTypeBits, mCreateInfo.MemoryProperty)
    );
    mImageMemory = DeviceHandle.allocateMemory(MemoryAllocateInfo);
    DeviceHandle.bindImageMemory(mImageHandle, mImageMemory, 0);
}

Texture::Texture(
    Protected, const Ref<LogicalDevice> InDevice, const CommandProducer& InCommandProducer, const SharedPtr<Resource::Texture>& InTexture
) : Image(InDevice) {
    vk::Buffer       StagingBuffer;
    vk::DeviceMemory StagingBufferMemory;

    if (!InTexture->IsValid()) {
        LOG_ERROR_CATEGORY(Vulkan, L"使用了无效的纹理资源请求创建纹理");
        mImageHandle = nullptr;
        mImageMemory = nullptr;
    }

    const vk::DeviceSize ImageSize = InTexture->GetWidth() * InTexture->GetHeight() * 4;
    const int32 MipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(InTexture->GetWidth(), InTexture->GetHeight())))) + 1;

    mDevice.get().CreateBuffer(
        ImageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        StagingBuffer,
        StagingBufferMemory
    );
    void*      Data;
    const auto Device = mDevice.get().GetHandle();
    const auto Result = Device.mapMemory(StagingBufferMemory, 0, ImageSize, vk::MemoryMapFlags(), &Data);
    if (Result != vk::Result::eSuccess) {
        LOG_ERROR_CATEGORY(Vulkan, L"映射内存失败");
        return;
    }
    std::memcpy(Data, InTexture->GetData(), ImageSize);
    Device.unmapMemory(StagingBufferMemory);
    mCreateInfo.Width     = InTexture->GetWidth();
    mCreateInfo.Height    = InTexture->GetHeight();
    mCreateInfo.MipLevels = MipLevel;
    mCreateInfo.Format    = vk::Format::eR8G8B8A8Unorm;
    // 这里设为Src是为了生成mipmap
    mCreateInfo.Usage     = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
    mCreateInfo.Tiling    = vk::ImageTiling::eOptimal;
    mCreateInfo.MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
    mCreateInfo.SampleCount    = vk::SampleCountFlagBits::e1;
    CreateImage();
    // 赋值暂存缓冲区数据到纹理图像
    // 1. 变换图像纹理到VK_IAMGE_LAYOUT_DST_OPTIMAL
    InCommandProducer.TrainsitionImageLayout(
        mImageHandle, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, MipLevel
    );
    // 2. 复制缓冲区到图像
    InCommandProducer.CopyBufferToImage(StagingBuffer, mImageHandle, mCreateInfo.Width, mCreateInfo.Height);
    // 3. 生成mipmap
    if (!InCommandProducer.GenerateMipmaps(mImageHandle, mCreateInfo.Format, mCreateInfo.Width, mCreateInfo.Height, MipLevel)) {
        // 无法生成mipmap就直接传给shader
        InCommandProducer.TrainsitionImageLayout(
            mImageHandle, mCreateInfo.Format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal
        );
    }
    Device.destroy(StagingBuffer);
    Device.freeMemory(StagingBufferMemory);
}

SharedPtr<Texture>
Texture::Create(Ref<LogicalDevice> InDevice, const CommandProducer& InCommandProducer, const SharedPtr<Resource::Texture>& InTexture) {
    return MakeShared<Texture>(Protected{}, InDevice, InCommandProducer, InTexture);
}

RHI_VULKAN_NAMESPACE_END
