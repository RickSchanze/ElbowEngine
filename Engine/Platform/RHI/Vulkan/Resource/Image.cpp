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
    auto view = new ImageView(context.GetLogicalDevice()->GetHandle().createImageView(view_info_create_info), view_info.debug_name);
    return view;
}

ImageInfo ImageInfo::CubemapInfo(
    vk::Format format, const AnsiString& name, vk::ImageCreateFlags create_flags, vk::ImageUsageFlags usage, int32_t width, int32_t height,
    int32_t mip_level, vk::SampleCountFlagBits sample_count, vk::ImageLayout initial_layout, vk::SharingMode sharing_mode,
    vk::MemoryPropertyFlags memory_property
)
{
    ImageInfo info{};
    info.format                  = format;
    info.usage                   = usage;
    info.width                   = width == 0 ? g_engine_statistics.window_size.width : width;
    info.height                  = height == 0 ? g_engine_statistics.window_size.height : height;
    info.mip_levels              = mip_level;
    info.array_layers            = 6;
    info.tiling                  = vk::ImageTiling::eOptimal;
    info.sample_count            = sample_count;
    info.image_type              = vk::ImageType::e2D;
    info.depth                   = 1;
    info.initial_layout          = initial_layout;
    info.sharing_mode            = sharing_mode;
    info.create_flags            = create_flags;
    info.memory_property         = memory_property;
    info.debug_name              = name;
    info.debug_image_name        = name + "Cubemap";
    info.debug_image_memory_name = name + "CubemapMemory";
    return info;
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

Image::Image(const ImageInfo& create_info) : image_info_(create_info)
{
    if (image_handle_ == nullptr)
    {
        CreateImage();
    }
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
    const auto& device_handle = VulkanContext::Get()->GetLogicalDevice()->GetHandle();
    if (image_handle_ != nullptr)
    {
        device_handle.destroyImage(image_handle_);
        image_handle_ = nullptr;
    }
    if (image_memory_ != nullptr)
    {
        device_handle.freeMemory(image_memory_);
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
    image_create_info.setFormat(image_info_.format == vk::Format::eUndefined ? vk::Format::eR8G8B8A8Srgb : image_info_.format);
    image_create_info.setTiling(image_info_.tiling);
    image_create_info.setInitialLayout(vk::ImageLayout::eUndefined);
    image_create_info.setUsage(image_info_.usage);
    image_create_info.setSamples(image_info_.sample_count);
    image_create_info.setMipLevels(image_info_.mip_levels);
    // 只被一个队列族使用
    image_create_info.setSharingMode(vk::SharingMode::eExclusive);

    image_handle_ = device_handle.createImage(image_create_info);

    if (!image_info_.debug_name.empty())
    {
        image_info_.debug_image_name = image_info_.debug_name + "_Image";
        context.GetLogicalDevice()->SetImageDebugName(image_handle_, image_info_.debug_image_name.c_str());
    }

    // 为图像分配内存
    const auto             memory_requirements = device_handle.getImageMemoryRequirements(image_handle_);
    vk::MemoryAllocateInfo memory_allocate_info{};
    memory_allocate_info.setAllocationSize(memory_requirements.size);
    memory_allocate_info.setMemoryTypeIndex(
        context.GetLogicalDevice()->GetAssociatedPhysicalDevice().FindMemoryType(memory_requirements.memoryTypeBits, image_info_.memory_property)
    );
    image_memory_ = device_handle.allocateMemory(memory_allocate_info);

    if (!image_info_.debug_name.empty())
    {
        image_info_.debug_image_memory_name = image_info_.debug_name + "_ImageMemory";
        context.GetLogicalDevice()->SetDeviceMemoryDebugName(image_memory_, image_info_.debug_image_memory_name.c_str());
    }

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
    return new ImageView(context.GetLogicalDevice()->GetHandle().createImageView(view_create_info), view_info.debug_name);
}

Cubemap::Cubemap(Protected, const ImageInfo& image_info) : Super(image_info)
{
    VulkanContext::Get()->GetCommandPool()->TrainsitionImageLayout(
        GetHandle(), GetFormat(), vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal, 6
    );
}

Cubemap::~Cubemap()
{
    for (auto& view: cubemap_image_views_)
    {
        delete view;
    }
    cubemap_image_views_.clear();
    cubemap_view_names_.clear();
}

void Cubemap::CreateCubemapImageViews(const AnsiString& name)
{
    VulkanContext&          context          = *VulkanContext::Get();
    vk::ImageViewCreateInfo view_create_info = {};
    view_create_info.image                   = GetHandle();
    view_create_info.viewType                = vk::ImageViewType::e2D;
    view_create_info.format                  = GetFormat();
    view_create_info.components              = {vk::ComponentSwizzle::eR};
    view_create_info.subresourceRange        = {vk::ImageAspectFlagBits::eColor, 0, GetMipLevel(), 0, 1};
    cubemap_image_views_.resize(6);
    cubemap_view_names_.resize(6);
    for (int i = 0; i < 6; i++)
    {
        // clang-format off
        view_create_info.subresourceRange.baseArrayLayer = i;
        cubemap_view_names_[i] = name + "_" + std::to_string(i);
        cubemap_image_views_[i] = new ImageView(context.GetLogicalDevice()->GetHandle().createImageView(view_create_info), cubemap_view_names_[i].c_str());
        // clang-format on
    }
}

ImageView* Cubemap::GetView(ECubemapFace face)
{
    if (cubemap_image_views_.empty())
    {
        CreateCubemapImageViews();
    }
    if (cubemap_image_views_.empty())
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan.Resource, "Create ImageView for cubemap failed");
        return nullptr;
    }
    return cubemap_image_views_[static_cast<int>(face)];
}

Texture::Texture(const ImageInfo& image_info, const uint8_t* data)
{
    image_info_ = image_info;
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
