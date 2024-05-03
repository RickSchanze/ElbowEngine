/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "Image.h"

#include "LogicalDevice.h"
#include "RHI/Vulkan/PhysicalDevice.h"

RHI_VULKAN_NAMESPACE_BEGIN

ImageBase::~ImageBase() {
    mImageHandle = VK_NULL_HANDLE;
}

Image::Image(Protected, Ref<UniquePtr<LogicalDevice>> InDevice, const ImageCreateInfo& InCreateInfo) : mDevice(InDevice) {
    vk::ImageCreateInfo ImageCreateInfo{};
    const auto          DeviceHandle = InDevice.get()->GetHandle();
    ImageCreateInfo.setImageType(InCreateInfo.ImageType);
    const vk::Extent3D Extent{
        InCreateInfo.Width, InCreateInfo.Height, InCreateInfo.ImageType == vk::ImageType::e3D ? 1 : InCreateInfo.Depth
    };
    ImageCreateInfo.setExtent(Extent);
    ImageCreateInfo.setArrayLayers(1);
    ImageCreateInfo.setFormat(InCreateInfo.Format);
    ImageCreateInfo.setTiling(InCreateInfo.Tiling);
    ImageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    ImageCreateInfo.setUsage(InCreateInfo.Usage);
    ImageCreateInfo.setSamples(InCreateInfo.SampleCount);
    ImageCreateInfo.setMipLevels(InCreateInfo.MipLevels);
    // 只被一个队列族使用
    ImageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

    mImageHandle                              = DeviceHandle.createImage(ImageCreateInfo);
    // 为图像分配内存
    const auto             MemoryRequirements = DeviceHandle.getImageMemoryRequirements(mImageHandle);
    vk::MemoryAllocateInfo MemoryAllocateInfo{};
    MemoryAllocateInfo.setAllocationSize(MemoryRequirements.size);
    MemoryAllocateInfo.setMemoryTypeIndex(
        InDevice.get()->GetAssociatedPhysicalDevice().FindMemoryType(MemoryRequirements.memoryTypeBits, InCreateInfo.MemoryProperty)
    );
    mImageMemory = DeviceHandle.allocateMemory(MemoryAllocateInfo);
    DeviceHandle.bindImageMemory(mImageHandle, mImageMemory, 0);
}

SharedPtr<Image> Image::CreateShared(Ref<UniquePtr<LogicalDevice>> InDevice, const ImageCreateInfo& InCreateInfo) {
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
        Device->GetHandle().destroyImage(mImageHandle);
        mImageHandle = nullptr;
    }
    if (mImageMemory != nullptr) {
        Device->GetHandle().freeMemory(mImageMemory);
        mImageMemory = nullptr;
    }
}

RHI_VULKAN_NAMESPACE_END
