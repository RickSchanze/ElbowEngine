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
    image_handle_ = VK_NULL_HANDLE;
}

TSharedPtr<ImageView> ImageBase::CreateImageViewShared(const ImageViewInfo& view_info) const
{
    return TSharedPtr<ImageView>{CreateImageView(view_info)};
}

TUniquePtr<ImageView> ImageBase::CreateImageViewUnique(const ImageViewInfo& view_info) const
{
    return TUniquePtr<ImageView>{CreateImageView(view_info)};
}

ImageView* SwapChainImage::CreateImageView(const ImageViewInfo& view_info) const
{
    VulkanContext&          context               = *VulkanContext::Get();
    vk::ImageViewCreateInfo view_info_create_info = {};
    view_info_create_info.setImage(image_handle_)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(view_info.format == vk::Format::eUndefined ? context.GetSwapChainImageFormat() : view_info.format)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(view_info.aspect_flags)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(view_info.mip_levels == 0 ? 1 : view_info.mip_levels)
                                 .setBaseArrayLayer(0)
                                 .setLayerCount(1))
        .setComponents(vk::ComponentMapping()
                           .setR(vk::ComponentSwizzle::eIdentity)
                           .setG(vk::ComponentSwizzle::eIdentity)
                           .setB(vk::ComponentSwizzle::eIdentity)
                           .setA(vk::ComponentSwizzle::eIdentity));
    return new ImageView(context.GetLogicalDevice()->GetHandle().createImageView(view_info_create_info));
}

ImageInfo::ImageInfo(const vk::ImageCreateInfo& image_info)
{
    format          = image_info.format;
    usage           = image_info.usage;
    width           = image_info.extent.width;
    height          = image_info.extent.height;
    depth           = image_info.extent.depth;
    mip_levels      = image_info.mipLevels;
    tiling          = image_info.tiling;
    sample_count    = image_info.samples;
    image_type      = image_info.imageType;
    memory_property = vk::MemoryPropertyFlagBits::eDeviceLocal;
}

Image::Image(Protected, const ImageInfo& create_info) : image_info_(create_info)
{
    CreateImage();
}

TSharedPtr<Image> Image::CreateShared(const ImageInfo& create_info)
{
    return MakeShared<Image>(Protected{}, create_info);
}

TUniquePtr<Image> Image::CreateUnique(const ImageInfo& create_info)
{
    return Move(MakeUnique<Image>(Protected{}, create_info));
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
    return Super::IsValid() && image_memory_ != nullptr;
}

void Image::Finialize()
{
    const auto& DeviceHandle = VulkanContext::Get()->GetLogicalDevice()->GetHandle();
    if (image_handle_ != nullptr)
    {
        DeviceHandle.destroyImage(image_handle_);
        image_handle_ = nullptr;
    }
    if (image_memory_ != nullptr)
    {
        DeviceHandle.freeMemory(image_memory_);
        image_memory_ = nullptr;
    }
}

void Image::CreateImage()
{
    VulkanContext&      context = *VulkanContext::Get();
    vk::ImageCreateInfo image_create_info{};
    const auto          device_handle = context.GetLogicalDevice()->GetHandle();
    image_create_info.setImageType(image_info_.image_type);
    const vk::Extent3D Extent{
        static_cast<uint32_t>(image_info_.width),
        static_cast<uint32_t>(image_info_.height),
        static_cast<uint32_t>(image_info_.image_type == vk::ImageType::e3D ? 1 : image_info_.depth)
    };
    image_create_info.setExtent(Extent);
    image_create_info.setArrayLayers(1);
    image_create_info.setFormat(image_info_.format);
    image_create_info.setTiling(image_info_.tiling);
    image_create_info.setInitialLayout(vk::ImageLayout::eUndefined);
    image_create_info.setUsage(image_info_.usage);
    image_create_info.setSamples(image_info_.sample_count);
    image_create_info.setMipLevels(image_info_.mip_levels);
    // 只被一个队列族使用
    image_create_info.setSharingMode(vk::SharingMode::eExclusive);

    image_handle_                             = device_handle.createImage(image_create_info);
    // 为图像分配内存
    const auto             MemoryRequirements = device_handle.getImageMemoryRequirements(image_handle_);
    vk::MemoryAllocateInfo MemoryAllocateInfo{};
    MemoryAllocateInfo.setAllocationSize(MemoryRequirements.size);
    MemoryAllocateInfo.setMemoryTypeIndex(
        context.GetLogicalDevice()->GetAssociatedPhysicalDevice().FindMemoryType(MemoryRequirements.memoryTypeBits, image_info_.memory_property)
    );
    image_memory_ = device_handle.allocateMemory(MemoryAllocateInfo);
    device_handle.bindImageMemory(image_handle_, image_memory_, 0);
}

ImageView* Image::CreateImageView(const ImageViewInfo& view_info) const
{
    VulkanContext&          context          = *VulkanContext::Get();
    vk::ImageViewCreateInfo view_create_info = {};
    view_create_info.setImage(image_handle_)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(view_info.format == vk::Format::eUndefined ? image_info_.format : view_info.format)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(view_info.aspect_flags)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(view_info.mip_levels == 0 ? image_info_.mip_levels : view_info.mip_levels)
                                 .setBaseArrayLayer(0)
                                 .setLayerCount(1))
        .setComponents(vk::ComponentMapping()
                           .setR(vk::ComponentSwizzle::eIdentity)
                           .setG(vk::ComponentSwizzle::eIdentity)
                           .setB(vk::ComponentSwizzle::eIdentity)
                           .setA(vk::ComponentSwizzle::eIdentity));
    return new ImageView(context.GetLogicalDevice()->GetHandle().createImageView(view_create_info));
}

TSharedPtr<Texture> Texture::CreateShared(const ImageInfo& image_info, const uint8_t* data)
{
    return MakeShared<Texture>(Protected{}, image_info, data);
}

TUniquePtr<Texture> Texture::CreateUnique(const ImageInfo& image_info, const uint8_t* data)
{
    return MakeUnique<Texture>(Protected{}, image_info, data);
}

Texture::Texture(Protected, const ImageInfo& image_info, const uint8_t* data) : Image(image_info)
{
    if (data == nullptr)
    {
        LOG_ERROR_CATEGORY(Vulkan, L"GPU创建Texture失败: Data为空");
        return;
    }
    if (image_info_.width <= 0 || image_info_.height <= 0)
    {
        LOG_ERROR_CATEGORY(Vulkan, L"GPU创建Texture失败: 宽或高不合法");
        return;
    }
    // TODO: 绑定TextureSampler
    vk::Buffer       staging_buffer;
    vk::DeviceMemory staging_buffer_memory;
    VulkanContext&   context       = *VulkanContext::Get();
    auto&            device        = context.GetLogicalDevice();
    auto             device_handle = device->GetHandle();
    auto&            command_pool  = context.GetCommandPool();

    auto width  = image_info_.width;
    auto height = image_info_.height;

    const vk::DeviceSize image_size = width * height * 4;
    image_info_.mip_levels          = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    device->CreateBuffer(
        image_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        staging_buffer,
        staging_buffer_memory
    );
    void*      map_data;
    const auto result = device_handle.mapMemory(staging_buffer_memory, 0, image_size, vk::MemoryMapFlags(), &map_data);
    if (result != vk::Result::eSuccess)
    {
        LOG_ERROR_CATEGORY(Vulkan, L"映射内存失败");
        return;
    }
    std::memcpy(map_data, data, image_size);
    device_handle.unmapMemory(staging_buffer_memory);
    image_info_.width  = width;
    image_info_.height = height;
    if (image_info_.format == vk::Format::eUndefined)
    {
        image_info_.format = vk::Format::eR8G8B8A8Unorm;
    }
    // 这里设为Src是为了生成mipmap
    image_info_.usage           = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
    image_info_.tiling          = vk::ImageTiling::eOptimal;
    image_info_.memory_property = vk::MemoryPropertyFlagBits::eDeviceLocal;
    image_info_.sample_count    = vk::SampleCountFlagBits::e1;
    CreateImage();
    // 赋值暂存缓冲区数据到纹理图像
    // 1. 变换图像纹理到VK_IAMGE_LAYOUT_DST_OPTIMAL
    command_pool->TrainsitionImageLayout(
        image_handle_, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, image_info_.mip_levels
    );
    // 2. 复制缓冲区到图像
    command_pool->CopyBufferToImage(staging_buffer, image_handle_, image_info_.width, image_info_.height);
    // 3. 生成mipmap
    if (!command_pool->GenerateMipmaps(image_handle_, image_info_.format, image_info_.width, image_info_.height, image_info_.mip_levels))
    {
        // 无法生成mipmap就直接传给shader
        command_pool->TrainsitionImageLayout(
            image_handle_, image_info_.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal
        );
    }
    device_handle.destroy(staging_buffer);
    device_handle.freeMemory(staging_buffer_memory);
}

void Texture::LoadDefaultTextures()
{
    if (!default_textures_loaded_)
    {
        Platform::OnRequestLoadDefaultLackTexture.Broadcast(&s_default_lack_texture_, &s_default_lack_texture_view_);
        default_textures_loaded_ = true;
        Platform::OnRequestLoadDefaultLackTexture.Clear();
    }
}

Texture& Texture::GetDefaultLackTexture()
{
    LoadDefaultTextures();
    return *s_default_lack_texture_;
}

ImageView& Texture::GetDefaultLackTextureView()
{
    LoadDefaultTextures();
    return *s_default_lack_texture_view_;
}

size_t SamplerInfo::GetHashCode() const
{
    std::size_t seed = 0;

    // 使用std::hash来计算每个成员变量的哈希值，并将其组合到最终的哈希值中
    seed ^= std::hash<int>{}(static_cast<int>(mag_filter)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(min_filter)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(address_mode_u)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(address_mode_v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(address_mode_w)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(enable_anisotropy) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(max_anisotropy) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(border_color)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(unnormalized_coordinates) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(enable_compare) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(static_cast<int>(mipmap_mode)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(mip_lod_bias) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(min_lod) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<float>{}(max_lod) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    return seed;
}

Sampler::Sampler(ResourceProtected, const SamplerInfo& info)
{
    vk::SamplerCreateInfo CreateInfo{};
    CreateInfo.anisotropyEnable        = info.enable_anisotropy;
    CreateInfo.borderColor             = info.border_color;
    CreateInfo.compareEnable           = info.enable_compare;
    CreateInfo.magFilter               = info.mag_filter;
    CreateInfo.maxAnisotropy           = info.max_anisotropy;
    CreateInfo.maxLod                  = info.max_lod;
    CreateInfo.minLod                  = info.min_lod;
    CreateInfo.mipmapMode              = info.mipmap_mode;
    CreateInfo.unnormalizedCoordinates = info.unnormalized_coordinates;
    CreateInfo.addressModeU            = info.address_mode_u;
    CreateInfo.addressModeV            = info.address_mode_v;
    CreateInfo.addressModeW            = info.address_mode_w;
    CreateInfo.mipLodBias              = info.mip_lod_bias;

    VulkanContext& context = *VulkanContext::Get();

    handle_        = context.GetLogicalDevice()->GetHandle().createSampler(CreateInfo);
    id_            = info.GetHashCode();
    samplers_[id_] = this;
}

Sampler* Sampler::Create(const SamplerInfo& info)
{
    const auto Id = info.GetHashCode();
    if (samplers_.contains(Id))
    {
        return samplers_[Id];
    }
    return new Sampler(ResourceProtected{}, info);
}

Sampler& Sampler::GetDefaultSampler()
{
    return *Create();
}

void Sampler::DestroyAllSamplers()
{
    while (!samplers_.empty())
    {
        samplers_.begin()->second->Destroy();
    }
    samplers_.clear();
}

void Sampler::InternalDestroy() const
{
    if (IsValid())
    {
        VulkanContext& context = *VulkanContext::Get();
        context.GetLogicalDevice()->DestroySampler(handle_);
        if (samplers_.contains(id_))
        {
            samplers_.erase(id_);
        }
    }
}

RHI_VULKAN_NAMESPACE_END
