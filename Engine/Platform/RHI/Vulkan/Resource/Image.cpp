/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "Image.h"

#include "CoreGlobal.h"
#include "RHI/Vulkan/PhysicalDevice.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/Render/LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

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

Image::Image(Protected, const ImageCreateInfo& InCreateInfo) : mCreateInfo(InCreateInfo) {
    CreateImage();
}

TSharedPtr<Image> Image::CreateShared(const ImageCreateInfo& InCreateInfo) {
    return MakeShared<Image>(Protected{}, InCreateInfo);
}

TUniquePtr<Image> Image::CreateUnique(const ImageCreateInfo& InCreateInfo) {
    return Move(MakeUnique<Image>(Protected{}, InCreateInfo));
}

void Image::Destroy() {
    Finialize();
}

Image::~Image() {
    Finialize();
}

bool Image::IsValid() const {
    return Super::IsValid() && mImageMemory != nullptr;
}

void Image::Finialize() {
    const auto& DeviceHandle = VulkanContext::Get().GetLogicalDevice()->GetHandle();
    if (mImageHandle != nullptr) {
        DeviceHandle.destroyImage(mImageHandle);
        mImageHandle = nullptr;
    }
    if (mImageMemory != nullptr) {
        DeviceHandle.freeMemory(mImageMemory);
        mImageMemory = nullptr;
    }
}

void Image::CreateImage() {
    VulkanContext&      Context = VulkanContext::Get();
    vk::ImageCreateInfo ImageCreateInfo{};
    const auto          DeviceHandle = Context.GetLogicalDevice()->GetHandle();
    ImageCreateInfo.setImageType(mCreateInfo.ImageType);
    const vk::Extent3D Extent{
        static_cast<UInt32>(mCreateInfo.Width),
        static_cast<UInt32>(mCreateInfo.Height),
        static_cast<UInt32>(mCreateInfo.ImageType == vk::ImageType::e3D ? 1 : mCreateInfo.Depth)
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
    MemoryAllocateInfo.setMemoryTypeIndex(Context.GetLogicalDevice()->GetAssociatedPhysicalDevice().FindMemoryType(
        MemoryRequirements.memoryTypeBits, mCreateInfo.MemoryProperty
    ));
    mImageMemory = DeviceHandle.allocateMemory(MemoryAllocateInfo);
    DeviceHandle.bindImageMemory(mImageHandle, mImageMemory, 0);
}

TSharedPtr<Texture> Texture::CreateShared(Int32 InWidth, Int32 InHeight, UInt8* InData) {
    return MakeShared<Texture>(Protected{}, InWidth, InHeight, InData);
}

TUniquePtr<Texture> Texture::CreateUnique(Int32 InWidth, Int32 InHeight, UInt8* InData){
    return MakeUnique<Texture>(Protected{}, InWidth, InHeight, InData);
}

Texture* Texture::Create(Int32 InWidth, Int32 InHeight, UInt8* InData){
    return new Texture(Protected{}, InWidth, InHeight, InData);
}

Texture::Texture(Protected, Int32 InWidth, Int32 InHeight, UInt8* InData) : Image() {
    if (InData == nullptr) {
        LOG_ERROR_CATEGORY(Vulkan, L"GPU创建Texture失败: Data为空");
        return;
    }
    if (InWidth <= 0 || InHeight <= 0) {
        LOG_ERROR_CATEGORY(Vulkan, L"GPU创建Texture失败: 宽或高不合法");
        return;
    }
    // TODO: 绑定TextureSampler
    vk::Buffer       StagingBuffer;
    vk::DeviceMemory StagingBufferMemory;
    VulkanContext&   Context      = VulkanContext::Get();
    auto&            Device       = Context.GetLogicalDevice();
    auto             DeviceHandle = Device->GetHandle();
    auto&            CommandPool  = Context.GetCommandPool();

    const vk::DeviceSize ImageSize = InWidth * InHeight * 4;
    mMipLevel                      = static_cast<uint32_t>(std::floor(std::log2(std::max(InWidth, InHeight)))) + 1;

    Device->CreateBuffer(
        ImageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        StagingBuffer,
        StagingBufferMemory
    );
    void*      Data;
    const auto Result = DeviceHandle.mapMemory(StagingBufferMemory, 0, ImageSize, vk::MemoryMapFlags(), &Data);
    if (Result != vk::Result::eSuccess) {
        LOG_ERROR_CATEGORY(Vulkan, L"映射内存失败");
        return;
    }
    std::memcpy(Data, InData, ImageSize);
    DeviceHandle.unmapMemory(StagingBufferMemory);
    mCreateInfo.Width     = InWidth;
    mCreateInfo.Height    = InHeight;
    mCreateInfo.MipLevels = mMipLevel;
    mCreateInfo.Format    = vk::Format::eR8G8B8A8Unorm;
    // 这里设为Src是为了生成mipmap
    mCreateInfo.Usage     = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
    mCreateInfo.Tiling    = vk::ImageTiling::eOptimal;
    mCreateInfo.MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
    mCreateInfo.SampleCount    = vk::SampleCountFlagBits::e1;
    CreateImage();
    // 赋值暂存缓冲区数据到纹理图像
    // 1. 变换图像纹理到VK_IAMGE_LAYOUT_DST_OPTIMAL
    CommandPool->TrainsitionImageLayout(
        mImageHandle, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, mMipLevel
    );
    // 2. 复制缓冲区到图像
    CommandPool->CopyBufferToImage(StagingBuffer, mImageHandle, mCreateInfo.Width, mCreateInfo.Height);
    // 3. 生成mipmap
    if (!CommandPool->GenerateMipmaps(mImageHandle, mCreateInfo.Format, mCreateInfo.Width, mCreateInfo.Height, mMipLevel)) {
        // 无法生成mipmap就直接传给shader
        CommandPool->TrainsitionImageLayout(
            mImageHandle, mCreateInfo.Format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal
        );
    }
    DeviceHandle.destroy(StagingBuffer);
    DeviceHandle.freeMemory(StagingBufferMemory);
}

RHI_VULKAN_NAMESPACE_END
