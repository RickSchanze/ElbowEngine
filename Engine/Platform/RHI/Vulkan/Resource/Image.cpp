/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "Image.h"

#include "CoreGlobal.h"
#include "PlatformEvents.h"
#include "RHI/Vulkan/PhysicalDevice.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/Render/LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

ImageBase::~ImageBase()
{
    mImageHandle = VK_NULL_HANDLE;
}

TSharedPtr<ImageView> ImageBase::CreateImageViewShared(const ImageViewInfo& InViewInfo) const
{
    return TSharedPtr<ImageView>{CreateImageView(InViewInfo)};
}

TUniquePtr<ImageView> ImageBase::CreateImageViewUnique(const ImageViewInfo& InViewInfo) const
{
    return TUniquePtr<ImageView>{CreateImageView(InViewInfo)};
}

ImageView* SwapChainImage::CreateImageView(const ImageViewInfo& InViewInfo) const
{
    VulkanContext&          Context  = VulkanContext::Get();
    vk::ImageViewCreateInfo ViewInfo = {};
    ViewInfo.setImage(mImageHandle)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(Context.GetSwapChainImageFormat())
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(InViewInfo.AspectFlags)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(InViewInfo.MipLevels)
                                 .setBaseArrayLayer(0)
                                 .setLayerCount(1))
        .setComponents(vk::ComponentMapping()
                           .setR(vk::ComponentSwizzle::eIdentity)
                           .setG(vk::ComponentSwizzle::eIdentity)
                           .setB(vk::ComponentSwizzle::eIdentity)
                           .setA(vk::ComponentSwizzle::eIdentity));
    return new ImageView(Context.GetLogicalDevice()->GetHandle().createImageView(ViewInfo));
}

ImageInfo::ImageInfo(const vk::ImageCreateInfo& InVkImageInfo)
{
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

Image::Image(Protected, const ImageInfo& InCreateInfo) : mImageInfo(InCreateInfo)
{
    CreateImage();
}

TSharedPtr<Image> Image::CreateShared(const ImageInfo& InCreateInfo)
{
    return MakeShared<Image>(Protected{}, InCreateInfo);
}

TUniquePtr<Image> Image::CreateUnique(const ImageInfo& InCreateInfo)
{
    return Move(MakeUnique<Image>(Protected{}, InCreateInfo));
}

void Image::Destroy()
{
    Finialize();
}

Image::~Image()
{
    Finialize();
}

bool Image::IsValid() const
{
    return Super::IsValid() && mImageMemory != nullptr;
}

void Image::Finialize()
{
    const auto& DeviceHandle = VulkanContext::Get().GetLogicalDevice()->GetHandle();
    if (mImageHandle != nullptr)
    {
        DeviceHandle.destroyImage(mImageHandle);
        mImageHandle = nullptr;
    }
    if (mImageMemory != nullptr)
    {
        DeviceHandle.freeMemory(mImageMemory);
        mImageMemory = nullptr;
    }
}

void Image::CreateImage()
{
    VulkanContext&      Context = VulkanContext::Get();
    vk::ImageCreateInfo ImageCreateInfo{};
    const auto          DeviceHandle = Context.GetLogicalDevice()->GetHandle();
    ImageCreateInfo.setImageType(mImageInfo.ImageType);
    const vk::Extent3D Extent{
        static_cast<UInt32>(mImageInfo.Width),
        static_cast<UInt32>(mImageInfo.Height),
        static_cast<UInt32>(mImageInfo.ImageType == vk::ImageType::e3D ? 1 : mImageInfo.Depth)
    };
    ImageCreateInfo.setExtent(Extent);
    ImageCreateInfo.setArrayLayers(1);
    ImageCreateInfo.setFormat(mImageInfo.Format);
    ImageCreateInfo.setTiling(mImageInfo.Tiling);
    ImageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    ImageCreateInfo.setUsage(mImageInfo.Usage);
    ImageCreateInfo.setSamples(mImageInfo.SampleCount);
    ImageCreateInfo.setMipLevels(mImageInfo.MipLevels);
    // 只被一个队列族使用
    ImageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

    mImageHandle                  = DeviceHandle.createImage(ImageCreateInfo);
    // 为图像分配内存
    const auto MemoryRequirements = DeviceHandle.getImageMemoryRequirements(mImageHandle);
    vk::MemoryAllocateInfo MemoryAllocateInfo{};
    MemoryAllocateInfo.setAllocationSize(MemoryRequirements.size);
    MemoryAllocateInfo.setMemoryTypeIndex(
        Context.GetLogicalDevice()->GetAssociatedPhysicalDevice().FindMemoryType(
            MemoryRequirements.memoryTypeBits, mImageInfo.MemoryProperty
        )
    );
    mImageMemory = DeviceHandle.allocateMemory(MemoryAllocateInfo);
    DeviceHandle.bindImageMemory(mImageHandle, mImageMemory, 0);
}

ImageView* Image::CreateImageView(const ImageViewInfo& InViewInfo) const
{
    VulkanContext&          Context  = VulkanContext::Get();
    vk::ImageViewCreateInfo ViewInfo = {};
    ViewInfo.setImage(mImageHandle)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(
            InViewInfo.Format == vk::Format::eUndefined ? mImageInfo.Format : InViewInfo.Format
        )
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(InViewInfo.AspectFlags)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(
                                     InViewInfo.MipLevels == 0 ? mImageInfo.MipLevels
                                                               : InViewInfo.MipLevels
                                 )
                                 .setBaseArrayLayer(0)
                                 .setLayerCount(1))
        .setComponents(vk::ComponentMapping()
                           .setR(vk::ComponentSwizzle::eIdentity)
                           .setG(vk::ComponentSwizzle::eIdentity)
                           .setB(vk::ComponentSwizzle::eIdentity)
                           .setA(vk::ComponentSwizzle::eIdentity));
    return new ImageView(Context.GetLogicalDevice()->GetHandle().createImageView(ViewInfo));
}

TSharedPtr<Texture> Texture::CreateShared(const ImageInfo& InImageInfo, const UInt8* InData)
{
    return MakeShared<Texture>(Protected{}, InImageInfo, InData);
}

TUniquePtr<Texture> Texture::CreateUnique(const ImageInfo& InImageInfo, const UInt8* InData)
{
    return MakeUnique<Texture>(Protected{}, InImageInfo, InData);
}

Texture::Texture(Protected, const ImageInfo& InImageInfo, const UInt8* InData) : Image(InImageInfo)
{
    if (InData == nullptr)
    {
        LOG_ERROR_CATEGORY(Vulkan, L"GPU创建Texture失败: Data为空");
        return;
    }
    if (mImageInfo.Width <= 0 || mImageInfo.Height <= 0)
    {
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

    auto Width  = mImageInfo.Width;
    auto Height = mImageInfo.Height;

    const vk::DeviceSize ImageSize = Width * Height * 4;
    mMipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;

    Device->CreateBuffer(
        ImageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        StagingBuffer,
        StagingBufferMemory
    );
    void*      Data;
    const auto Result =
        DeviceHandle.mapMemory(StagingBufferMemory, 0, ImageSize, vk::MemoryMapFlags(), &Data);
    if (Result != vk::Result::eSuccess)
    {
        LOG_ERROR_CATEGORY(Vulkan, L"映射内存失败");
        return;
    }
    std::memcpy(Data, InData, ImageSize);
    DeviceHandle.unmapMemory(StagingBufferMemory);
    // TODO: 这里不应该硬编码
    mImageInfo.Width  = Width;
    mImageInfo.Height = Height;
    if (mImageInfo.Format == vk::Format::eUndefined)
    {
        mImageInfo.Format = vk::Format::eR8G8B8A8Unorm;
    }
    // 这里设为Src是为了生成mipmap
    mImageInfo.Usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled |
                       vk::ImageUsageFlagBits::eTransferSrc;
    mImageInfo.Tiling         = vk::ImageTiling::eOptimal;
    mImageInfo.MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
    mImageInfo.SampleCount    = vk::SampleCountFlagBits::e1;
    CreateImage();
    // 赋值暂存缓冲区数据到纹理图像
    // 1. 变换图像纹理到VK_IAMGE_LAYOUT_DST_OPTIMAL
    CommandPool->TrainsitionImageLayout(
        mImageHandle,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        mMipLevel
    );
    // 2. 复制缓冲区到图像
    CommandPool->CopyBufferToImage(
        StagingBuffer, mImageHandle, mImageInfo.Width, mImageInfo.Height
    );
    // 3. 生成mipmap
    if (!CommandPool->GenerateMipmaps(
            mImageHandle, mImageInfo.Format, mImageInfo.Width, mImageInfo.Height, mMipLevel
        ))
    {
        // 无法生成mipmap就直接传给shader
        CommandPool->TrainsitionImageLayout(
            mImageHandle,
            mImageInfo.Format,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal
        );
    }
    DeviceHandle.destroy(StagingBuffer);
    DeviceHandle.freeMemory(StagingBufferMemory);
}

void Texture::LoadDefaultTextures()
{
    if (!bDefaultTexturesLoaded)
    {
        Platform::OnRequestLoadDefaultLackTexture.Broadcast(
            &sDefaultLackTexture, &sDefaultLackTextureView
        );
        bDefaultTexturesLoaded = true;
        Platform::OnRequestLoadDefaultLackTexture.Clear();
    }
}

Texture& Texture::GetDefaultLackTexture()
{
    LoadDefaultTextures();
    return *sDefaultLackTexture;
}

ImageView& Texture::GetDefaultLackTextureView()
{
    LoadDefaultTextures();
    return *sDefaultLackTextureView;
}

size_t SamplerInfo::GetHashCode() const
{
    std::size_t seed = 0;

    // 使用std::hash来计算每个成员变量的哈希值，并将其组合到最终的哈希值中
    seed ^= std::hash<int>{}(static_cast<int>(MagFilter)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(MinFilter)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^=
        std::hash<int>{}(static_cast<int>(AddressModeU)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^=
        std::hash<int>{}(static_cast<int>(AddressModeV)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^=
        std::hash<int>{}(static_cast<int>(AddressModeW)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(bEnableAnisotropy) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(MaxAnisotropy) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^=
        std::hash<int>{}(static_cast<int>(BorderColor)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(bUnnormalizedCoordinates) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(bEnableCompare) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(MipmapMode)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(MipLodBias) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(MinLod) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(MaxLod) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    return seed;
}

Sampler::Sampler(ResourceProtected, const SamplerInfo& InInitializer)
{
    vk::SamplerCreateInfo CreateInfo{};
    CreateInfo.anisotropyEnable        = InInitializer.bEnableAnisotropy;
    CreateInfo.borderColor             = InInitializer.BorderColor;
    CreateInfo.compareEnable           = InInitializer.bEnableCompare;
    CreateInfo.magFilter               = InInitializer.MagFilter;
    CreateInfo.maxAnisotropy           = InInitializer.MaxAnisotropy;
    CreateInfo.maxLod                  = InInitializer.MaxLod;
    CreateInfo.minLod                  = InInitializer.MinLod;
    CreateInfo.mipmapMode              = InInitializer.MipmapMode;
    CreateInfo.unnormalizedCoordinates = InInitializer.bUnnormalizedCoordinates;
    CreateInfo.addressModeU            = InInitializer.AddressModeU;
    CreateInfo.addressModeV            = InInitializer.AddressModeV;
    CreateInfo.addressModeW            = InInitializer.AddressModeW;
    CreateInfo.mipLodBias              = InInitializer.MipLodBias;

    VulkanContext& Context = VulkanContext::Get();

    mHandle        = Context.GetLogicalDevice()->GetHandle().createSampler(CreateInfo);
    mId            = InInitializer.GetHashCode();
    sSamplers[mId] = this;
}

Sampler* Sampler::Create(const SamplerInfo& InInitializer)
{
    const auto Id = InInitializer.GetHashCode();
    if (sSamplers.contains(Id))
    {
        return sSamplers[Id];
    }
    return new Sampler(ResourceProtected{}, InInitializer);
}

Sampler& Sampler::GetDefaultSampler(){
    return *Create();
}

void Sampler::DestroyAllSamplers()
{
    while (!sSamplers.empty())
    {
        sSamplers.begin()->second->Destroy();
    }
    sSamplers.clear();
}

void Sampler::InternalDestroy() const
{
    if (IsValid())
    {
        VulkanContext& Context = VulkanContext::Get();
        Context.GetLogicalDevice()->GetHandle().destroySampler(mHandle);
        if (sSamplers.contains(mId))
        {
            sSamplers.erase(mId);
        }
    }
}

RHI_VULKAN_NAMESPACE_END
