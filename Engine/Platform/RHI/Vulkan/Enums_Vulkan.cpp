/**
 * @file Enums_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#include "Enums_Vulkan.h"

using namespace platform::rhi;

VkFormat RHIFormatToVkFormat(const Format format)
{
    switch (format)
    {
    case Format::R32G32B32_Float: return VK_FORMAT_R32G32B32_SFLOAT;
    case Format::R16G16B16A16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM;
    case Format::R32G32_UInt: return VK_FORMAT_R32G32_UINT;
    case Format::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case Format::R8G8B8A8_SNorm: return VK_FORMAT_R8G8B8A8_SNORM;
    case Format::R8G8B8A8_UInt: return VK_FORMAT_R8G8B8A8_UINT;
    case Format::R8G8B8A8_SInt: return VK_FORMAT_R8G8B8A8_SINT;
    case Format::D32_Float_S8X24_UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case Format::D32_Float: return VK_FORMAT_D32_SFLOAT;
    case Format::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
    case Format::D16_UNorm: return VK_FORMAT_D16_UNORM;
    case Format::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
    case Format::A2B10G10R10_UNorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case Format::B8G8R8A8_UNorm: return VK_FORMAT_B8G8R8A8_UNORM;
    case Format::R16G16B16A16_Float: return VK_FORMAT_R16G16B16A16_SFLOAT;
    default: return VK_FORMAT_UNDEFINED;
    }
}

Format VkFormatToRHIFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R32G32B32_SFLOAT: return Format::R32G32B32_Float;
    case VK_FORMAT_R16G16B16A16_UNORM: return Format::R16G16B16A16_UNorm;
    case VK_FORMAT_R32G32_UINT: return Format::R32G32_UInt;
    case VK_FORMAT_R8G8B8A8_UNORM: return Format::R8G8B8A8_UNorm;
    case VK_FORMAT_R8G8B8A8_SNORM: return Format::R8G8B8A8_SNorm;
    case VK_FORMAT_R8G8B8A8_UINT: return Format::R8G8B8A8_UInt;
    case VK_FORMAT_R8G8B8A8_SINT: return Format::R8G8B8A8_SInt;
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return Format::D32_Float_S8X24_UInt;
    case VK_FORMAT_D32_SFLOAT: return Format::D32_Float;
    case VK_FORMAT_D24_UNORM_S8_UINT: return Format::D24_UNorm_S8_UInt;
    case VK_FORMAT_D16_UNORM: return Format::D16_UNorm;
    case VK_FORMAT_B8G8R8A8_SRGB: return Format::B8G8R8A8_SRGB;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return Format::A2B10G10R10_UNorm;
    case VK_FORMAT_B8G8R8A8_UNORM: return Format::B8G8R8A8_UNorm;
    case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::R16G16B16A16_Float;
    default: return Format::Count;
    }
}
VkColorSpaceKHR RHIColorSpaceToVkColorSpace(ColorSpace color_space)
{
    switch (color_space)
    {
    case ColorSpace::sRGB: return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    case ColorSpace::HDR10: return VK_COLOR_SPACE_HDR10_ST2084_EXT;
    case ColorSpace::Count: return VK_COLOR_SPACE_MAX_ENUM_KHR;
    }
    return VK_COLOR_SPACE_MAX_ENUM_KHR;
}

ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space)
{
    switch (color_space)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return ColorSpace::sRGB;
    case VK_COLOR_SPACE_HDR10_ST2084_EXT: return ColorSpace::HDR10;
    default: return ColorSpace::Count;
    }
}

VkPresentModeKHR RHIPresentModeToVkPresentMode(PresentMode present_mode)
{
    switch (present_mode)
    {
    case PresentMode::Immediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
    case PresentMode::VSync: return VK_PRESENT_MODE_FIFO_KHR;
    case PresentMode::TripleBuffer: return VK_PRESENT_MODE_MAILBOX_KHR;
    case PresentMode::Count: return VK_PRESENT_MODE_MAX_ENUM_KHR;
    }
    return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode)
{
    switch (present_mode)
    {
    case VK_PRESENT_MODE_IMMEDIATE_KHR: return PresentMode::Immediate;
    case VK_PRESENT_MODE_FIFO_KHR: return PresentMode::VSync;
    case VK_PRESENT_MODE_MAILBOX_KHR: return PresentMode::TripleBuffer;
    default: return PresentMode::Count;
    }
}

VkSampleCountFlagBits RHISampleCountToVkSampleCount(SampleCount sample_count)
{
    switch (sample_count)
    {
    case SampleCountBits::SC_1: return VK_SAMPLE_COUNT_1_BIT;
    case SampleCountBits::SC_2: return VK_SAMPLE_COUNT_2_BIT;
    case SampleCountBits::SC_4: return VK_SAMPLE_COUNT_4_BIT;
    case SampleCountBits::SC_8: return VK_SAMPLE_COUNT_8_BIT;
    case SampleCountBits::SC_16: return VK_SAMPLE_COUNT_16_BIT;
    case SampleCountBits::SC_32: return VK_SAMPLE_COUNT_32_BIT;
    case SampleCountBits::SC_64: return VK_SAMPLE_COUNT_64_BIT;
    case SampleCountBits::SC_Count: return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    }
    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

SampleCountBits VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count)
{
    switch (sample_count)
    {
    case VK_SAMPLE_COUNT_1_BIT: return SampleCountBits::SC_1;
    case VK_SAMPLE_COUNT_2_BIT: return SampleCountBits::SC_2;
    case VK_SAMPLE_COUNT_4_BIT: return SampleCountBits::SC_4;
    case VK_SAMPLE_COUNT_8_BIT: return SampleCountBits::SC_8;
    case VK_SAMPLE_COUNT_16_BIT: return SampleCountBits::SC_16;
    case VK_SAMPLE_COUNT_32_BIT: return SampleCountBits::SC_32;
    case VK_SAMPLE_COUNT_64_BIT: return SampleCountBits::SC_64;
    case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: return SampleCountBits::SC_Count;
    }
    return SampleCountBits::SC_Count;
}

VkImageAspectFlags RHIImageAspectToVkImageAspect(int aspect)
{
    VkImageAspectFlags flags = 0;
    if (aspect & ImageAspectBits::IA_Color)
    {
        flags |= VK_IMAGE_ASPECT_COLOR_BIT;
    }
    if (aspect & ImageAspectBits::IA_Depth)
    {
        flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    if (aspect & ImageAspectBits::IA_Stencil)
    {
        flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    return flags;
}

int VkImageAspectToRHIImageAspect(VkImageAspectFlags aspect)
{
    int rtn = 0;
    if (rtn & VK_IMAGE_ASPECT_COLOR_BIT)
    {
        rtn |= ImageAspectBits::IA_Color;
    }
    if (rtn & VK_IMAGE_ASPECT_DEPTH_BIT)
    {
        rtn |= ImageAspectBits::IA_Depth;
    }
    if (rtn & VK_IMAGE_ASPECT_STENCIL_BIT)
    {
        rtn |= ImageAspectBits::IA_Stencil;
    }
    return rtn;
}

ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(VkComponentSwizzle swizzle)
{
    switch (swizzle)
    {
    case VK_COMPONENT_SWIZZLE_IDENTITY: return ComponentMappingElement::Identity;
    case VK_COMPONENT_SWIZZLE_ZERO: return ComponentMappingElement::Zero;
    case VK_COMPONENT_SWIZZLE_ONE: return ComponentMappingElement::One;
    case VK_COMPONENT_SWIZZLE_R: return ComponentMappingElement::R;
    case VK_COMPONENT_SWIZZLE_G: return ComponentMappingElement::G;
    case VK_COMPONENT_SWIZZLE_B: return ComponentMappingElement::B;
    case VK_COMPONENT_SWIZZLE_A: return ComponentMappingElement::A;
    case VK_COMPONENT_SWIZZLE_MAX_ENUM: return ComponentMappingElement::Count;
    }
    return ComponentMappingElement::Count;
}

VkComponentSwizzle RHIComponentMappingElementToVkComponentSwizzle(ComponentMappingElement swizzle)
{
    switch (swizzle)
    {
    case ComponentMappingElement::Identity: return VK_COMPONENT_SWIZZLE_IDENTITY;
    case ComponentMappingElement::Zero: return VK_COMPONENT_SWIZZLE_ZERO;
    case ComponentMappingElement::One: return VK_COMPONENT_SWIZZLE_ONE;
    case ComponentMappingElement::R: return VK_COMPONENT_SWIZZLE_R;
    case ComponentMappingElement::G: return VK_COMPONENT_SWIZZLE_G;
    case ComponentMappingElement::B: return VK_COMPONENT_SWIZZLE_B;
    case ComponentMappingElement::A: return VK_COMPONENT_SWIZZLE_A;
    case ComponentMappingElement::Count: return VK_COMPONENT_SWIZZLE_MAX_ENUM;
    }
    return VK_COMPONENT_SWIZZLE_MAX_ENUM;
}

VkImageViewType RHIImageViewTypeToVkImageViewType(ImageViewType type)
{
    switch (type)
    {
    case ImageViewType::D1: return VK_IMAGE_VIEW_TYPE_1D;
    case ImageViewType::D2: return VK_IMAGE_VIEW_TYPE_2D;
    case ImageViewType::D3: return VK_IMAGE_VIEW_TYPE_3D;
    case ImageViewType::Cube: return VK_IMAGE_VIEW_TYPE_CUBE;
    case ImageViewType::Array1D: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case ImageViewType::Array2D: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case ImageViewType::ArrayCube: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    case ImageViewType::Count: return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

ImageViewType VkImageViewTypeToRHIImageViewType(VkImageViewType type)
{
    switch (type)
    {
    case VK_IMAGE_VIEW_TYPE_1D: return ImageViewType::D1;
    case VK_IMAGE_VIEW_TYPE_2D: return ImageViewType::D2;
    case VK_IMAGE_VIEW_TYPE_3D: return ImageViewType::D3;
    case VK_IMAGE_VIEW_TYPE_CUBE: return ImageViewType::Cube;
    case VK_IMAGE_VIEW_TYPE_1D_ARRAY: return ImageViewType::Array1D;
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY: return ImageViewType::Array2D;
    case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: return ImageViewType::ArrayCube;
    case VK_IMAGE_VIEW_TYPE_MAX_ENUM: return ImageViewType::Count;
    }
    return ImageViewType::Count;
}

VkBufferUsageFlags RHIBufferUsageToVkBufferUsage(BufferUsage usage)
{
    VkBufferUsageFlags flags = 0;
    if (usage & BUB_VertexBuffer)
    {
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (usage & BUB_IndexBuffer)
    {
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if (usage & BUB_UniformBuffer)
    {
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (usage & BUB_TransferSrc)
    {
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if (usage & BUB_TransferDst)
    {
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    return flags;
}

BufferUsage VkBufferUsageToRHIBufferUsage(VkBufferUsageFlags usage)
{
    BufferUsage flags = 0;
    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
    {
        flags |= BUB_VertexBuffer;
    }
    if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
    {
        flags |= BUB_IndexBuffer;
    }
    if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {
        flags |= BUB_UniformBuffer;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
    {
        flags |= BUB_TransferSrc;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)
    {
        flags |= BUB_TransferDst;
    }
    return flags;
}

VkMemoryPropertyFlags RHIMemoryPropertyToVkMemoryProperty(BufferMemoryProperty property)
{
    VkMemoryPropertyFlags flags = 0;
    if (property & BMPB_DeviceLocal)
    {
        flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    if (property & BMPB_HostVisible)
    {
        flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }
    if (property & BMPB_HostCoherent)
    {
        flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    return flags;
}

BufferMemoryProperty VkMemoryPropertyToRHIMemoryProperty(VkMemoryPropertyFlags property)
{
    BufferMemoryProperty flags = 0;
    if (property & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        flags |= BMPB_DeviceLocal;
    }
    if (property & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        flags |= BMPB_HostVisible;
    }
    if (property & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    {
        flags |= BMPB_HostCoherent;
    }
    return flags;
}

VkImageLayout RHIImageLayoutToVkImageLayout(ImageLayout layout)
{
    switch (layout)
    {
    case ImageLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
    case ImageLayout::General: return VK_IMAGE_LAYOUT_GENERAL;
    case ImageLayout::ColorAttachment: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case ImageLayout::DepthStencilAttachment: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case ImageLayout::TransferSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case ImageLayout::TransferDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case ImageLayout::ShaderReadOnly: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case ImageLayout::PresentSrc: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case ImageLayout::Count: return VK_IMAGE_LAYOUT_MAX_ENUM;
    default: return VK_IMAGE_LAYOUT_MAX_ENUM;
    }
}

ImageLayout VkImageLayoutToRHIImageLayout(VkImageLayout layout)
{
    switch (layout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED: return ImageLayout::Undefined;
    case VK_IMAGE_LAYOUT_GENERAL: return ImageLayout::General;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return ImageLayout::ColorAttachment;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return ImageLayout::DepthStencilAttachment;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return ImageLayout::TransferSrc;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return ImageLayout::TransferDst;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return ImageLayout::ShaderReadOnly;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return ImageLayout::PresentSrc;
    case VK_IMAGE_LAYOUT_MAX_ENUM: return ImageLayout::Count;
    default: return ImageLayout::Count;
    }
}

VkAttachmentLoadOp RHIAttachmentLoadOpToVkAttachmentLoadOp(AttachmentLoadOperation load_op)
{
    switch (load_op)
    {
    case AttachmentLoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
    case AttachmentLoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case AttachmentLoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    default: return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
    }
}

AttachmentLoadOperation VkAttachmentLoadOpToRHIAttachmentLoadOp(VkAttachmentLoadOp load_op)
{
    VK_POLYGON_MODE_FILL
    switch (load_op)
    {
    case VK_ATTACHMENT_LOAD_OP_LOAD: return AttachmentLoadOperation::Load;
    case VK_ATTACHMENT_LOAD_OP_CLEAR: return AttachmentLoadOperation::Clear;
    case VK_ATTACHMENT_LOAD_OP_DONT_CARE: return AttachmentLoadOperation::DontCare;
    default: return AttachmentLoadOperation::DontCare;
    }
}

VkAttachmentStoreOp RHIAttachmentStoreOpToVkAttachmentStoreOp(AttachmentStoreOperation store_op)
{
    switch (store_op)
    {
    case AttachmentStoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
    case AttachmentStoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    default: return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
    }
}

AttachmentStoreOperation VkAttachmentStoreOpToRHIAttachmentStoreOp(VkAttachmentStoreOp store_op)
{
    switch (store_op)
    {
    case VK_ATTACHMENT_STORE_OP_STORE: return AttachmentStoreOperation::Store;
    case VK_ATTACHMENT_STORE_OP_DONT_CARE: return AttachmentStoreOperation::DontCare;
    default: return AttachmentStoreOperation::DontCare;
    }
}

VkPolygonMode RHIPolygonModeToVkPolygonMode(platform::rhi::PolygonMode polygon_mode)
{
    switch (polygon_mode)
    {
    case PolygonMode::Fill: return VK_POLYGON_MODE_FILL;
    case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
    case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
    default: return VK_POLYGON_MODE_MAX_ENUM;
    }
}

PolygonMode VkPolygonModeToRHIPolygonMode(VkPolygonMode polygon_mode)
{
    switch (polygon_mode)
    {
    case VK_POLYGON_MODE_FILL: return PolygonMode::Fill;
    case VK_POLYGON_MODE_LINE: return PolygonMode::Line;
    case VK_POLYGON_MODE_POINT: return PolygonMode::Point;
    default: return PolygonMode::Count;
    }
}

VkFrontFace RHIFrontFaceToVkFrontFace(platform::rhi::FrontFace front_face)
{
    switch (front_face)
    {
    case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
    case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    default: return VK_FRONT_FACE_MAX_ENUM;
    }
}

platform::rhi::FrontFace VkFrontFaceToRHIFrontFace(VkFrontFace front_face)
{
    switch (front_face)
    {
    case VK_FRONT_FACE_CLOCKWISE: return FrontFace::Clockwise;
    case VK_FRONT_FACE_COUNTER_CLOCKWISE: return FrontFace::CounterClockwise;
    default: return FrontFace::Count;
    }
}

VkCullModeFlags RHICullModeToVkCullMode(platform::rhi::CullMode cull_mode)
{
    switch (cull_mode)
    {
    case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
    case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
    case CullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
    case CullMode::None: return VK_CULL_MODE_NONE;
    default: return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
    }
}

platform::rhi::CullMode VkCullModeToRHICullMode(VkCullModeFlags cull_mode)
{
    switch (cull_mode)
    {
    case VK_CULL_MODE_BACK_BIT: return CullMode::Back;
    case VK_CULL_MODE_FRONT_BIT: return CullMode::Front;
    case VK_CULL_MODE_FRONT_AND_BACK: return CullMode::FrontAndBack;
    case VK_CULL_MODE_NONE: return CullMode::None;
    default: return CullMode::Count;
    }
}

VkCompareOp RHICompareOpToVkCompareOp(platform::rhi::CompareOp compare_op)
{
    switch (compare_op)
    {
    case CompareOp::Never: return VK_COMPARE_OP_NEVER;
    case CompareOp::Less: return VK_COMPARE_OP_LESS;
    case CompareOp::Equal: return VK_COMPARE_OP_EQUAL;
    case CompareOp::LessOrEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
    case CompareOp::Greater: return VK_COMPARE_OP_GREATER;
    case CompareOp::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
    case CompareOp::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case CompareOp::Always: return VK_COMPARE_OP_ALWAYS;
    default: return VK_COMPARE_OP_MAX_ENUM;
    }
}

platform::rhi::CompareOp VkCompareOpToRHICompareOp(VkCompareOp compare_op)
{
    switch (compare_op)
    {
    case VK_COMPARE_OP_NEVER: return CompareOp::Never;
    case VK_COMPARE_OP_LESS: return CompareOp::Less;
    case VK_COMPARE_OP_EQUAL: return CompareOp::Equal;
    case VK_COMPARE_OP_LESS_OR_EQUAL: return CompareOp::LessOrEqual;
    case VK_COMPARE_OP_GREATER: return CompareOp::Greater;
    case VK_COMPARE_OP_NOT_EQUAL: return CompareOp::NotEqual;
    case VK_COMPARE_OP_GREATER_OR_EQUAL: return CompareOp::GreaterOrEqual;
    case VK_COMPARE_OP_ALWAYS: return CompareOp::Always;
    default: return CompareOp::Count;
    }
}

VkShaderStageFlagBits RHIStageToVkShaderStage(platform::rhi::ShaderStage stage)
{
    switch (stage)
    {
    case ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
    // case ShaderStage::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    // case ShaderStage::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    // case ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
    case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
    default: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }
}

platform::rhi::ShaderStage VkShaderStageToRHIStage(VkShaderStageFlagBits stage)
{
    switch (stage)
    {
    case VK_SHADER_STAGE_VERTEX_BIT: return ShaderStage::Vertex;
    // case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT: return ShaderStage::TessellationControl;
    // case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return ShaderStage::TessellationEvaluation;
    // case VK_SHADER_STAGE_GEOMETRY_BIT: return ShaderStage::Geometry;
    case VK_SHADER_STAGE_FRAGMENT_BIT: return ShaderStage::Fragment;
    case VK_SHADER_STAGE_COMPUTE_BIT: return ShaderStage::Compute;
    default: return ShaderStage::Count;
    }
}

VkDescriptorType RHIDescriptorTypeToVkDescriptorType(platform::rhi::DescriptorType type)
{
    switch (type)
    {
    case DescriptorType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
    // case DescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // case DescriptorType::SampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    // case DescriptorType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    // case DescriptorType::UniformTexelBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    // case DescriptorType::StorageTexelBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case DescriptorType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // case DescriptorType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case DescriptorType::UniformBufferDynamic: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    // case DescriptorType::StorageBufferDynamic: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

platform::rhi::DescriptorType VkDescriptorTypeToRHIDescriptorType(VkDescriptorType type)
{
    switch (type)
    {
        case VK_DESCRIPTOR_TYPE_SAMPLER: return DescriptorType::Sampler;
        // case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return DescriptorType::CombinedImageSampler;
        // case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return DescriptorType::SampledImage;
        // case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: return DescriptorType::StorageImage;
        // case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return DescriptorType::UniformTexelBuffer;
        // case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return DescriptorType::StorageTexelBuffer;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return DescriptorType::UniformBuffer;
        // case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return DescriptorType::StorageBuffer;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return DescriptorType::UniformBufferDynamic;
        // case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return DescriptorType::StorageBufferDynamic;
        default: return DescriptorType::Count;
    }
}
