//
// Created by Echo on 2025/3/25.
//

#include "Enums.hpp"

using namespace rhi;

// 实现
VkFormat RHIFormatToVkFormat(const Format format) {
    switch (format) {
        case Format::R32G32B32_Float:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32B32A32_Float:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::R16G16B16A16_UNorm:
            return VK_FORMAT_R16G16B16A16_UNORM;
        case Format::R32G32_UInt:
            return VK_FORMAT_R32G32_UINT;
        case Format::R8G8B8A8_UNorm:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNorm:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8_UInt:
            return VK_FORMAT_R8G8B8A8_UINT;
        case Format::R8G8B8A8_SInt:
            return VK_FORMAT_R8G8B8A8_SINT;
        case Format::D32_Float_S8X24_UInt:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Format::D32_Float:
            return VK_FORMAT_D32_SFLOAT;
        case Format::D24_UNorm_S8_UInt:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case Format::D16_UNorm:
            return VK_FORMAT_D16_UNORM;
        case Format::B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case Format::A2B10G10R10_UNorm:
            return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case Format::B8G8R8A8_UNorm:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::R16G16B16A16_Float:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Format::R32G32_Float:
            return VK_FORMAT_R32G32_SFLOAT;
        case Format::R8_UNorm:
            return VK_FORMAT_R8_UNORM;
        case Format::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::R8_SRGB:
            return VK_FORMAT_R8_SRGB;
        case Format::R8G8B8_UNorm:
            return VK_FORMAT_R8G8B8_UNORM;
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

Format VkFormatToRHIFormat(const VkFormat format) {
    switch (format) {
        case VK_FORMAT_R32G32B32_SFLOAT:
            return Format::R32G32B32_Float;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return Format::R32G32B32A32_Float;
        case VK_FORMAT_R16G16B16A16_UNORM:
            return Format::R16G16B16A16_UNorm;
        case VK_FORMAT_R32G32_UINT:
            return Format::R32G32_UInt;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Format::R8G8B8A8_UNorm;
        case VK_FORMAT_R8G8B8A8_SNORM:
            return Format::R8G8B8A8_SNorm;
        case VK_FORMAT_R8G8B8A8_UINT:
            return Format::R8G8B8A8_UInt;
        case VK_FORMAT_R8G8B8A8_SINT:
            return Format::R8G8B8A8_SInt;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return Format::D32_Float_S8X24_UInt;
        case VK_FORMAT_D32_SFLOAT:
            return Format::D32_Float;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return Format::D24_UNorm_S8_UInt;
        case VK_FORMAT_D16_UNORM:
            return Format::D16_UNorm;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return Format::B8G8R8A8_SRGB;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return Format::A2B10G10R10_UNorm;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return Format::B8G8R8A8_UNorm;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return Format::R16G16B16A16_Float;
        case VK_FORMAT_R32G32_SFLOAT:
            return Format::R32G32_Float;
        case VK_FORMAT_R8_UNORM:
            return Format::R8_UNorm;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return Format::R8G8B8A8_SRGB;
        case VK_FORMAT_R8_SRGB:
            return Format::R8_SRGB;
        case VK_FORMAT_R8G8B8_UNORM:
            return Format::R8G8B8_UNorm;
        default:
            return Format::Count;
    }
}
VkColorSpaceKHR RHIColorSpaceToVkColorSpace(ColorSpace color_space) {
    switch (color_space) {
        case ColorSpace::sRGB:
            return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        case ColorSpace::HDR10:
            return VK_COLOR_SPACE_HDR10_ST2084_EXT;
        case ColorSpace::Count:
            return VK_COLOR_SPACE_MAX_ENUM_KHR;
    }
    return VK_COLOR_SPACE_MAX_ENUM_KHR;
}

ColorSpace VkColorSpaceToRHIColorSpace(const VkColorSpaceKHR color_space) {
    switch (color_space) {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return ColorSpace::sRGB;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
            return ColorSpace::HDR10;
        default:
            return ColorSpace::Count;
    }
}

VkPresentModeKHR RHIPresentModeToVkPresentMode(PresentMode present_mode) {
    switch (present_mode) {
        case PresentMode::Immediate:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode::VSync:
            return VK_PRESENT_MODE_FIFO_KHR;
        case PresentMode::TripleBuffer:
            return VK_PRESENT_MODE_MAILBOX_KHR;
        case PresentMode::Count:
            return VK_PRESENT_MODE_MAX_ENUM_KHR;
    }
    return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

PresentMode VkPresentModeToRHIPresentMode(const VkPresentModeKHR present_mode) {
    switch (present_mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return PresentMode::Immediate;
        case VK_PRESENT_MODE_FIFO_KHR:
            return PresentMode::VSync;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return PresentMode::TripleBuffer;
        default:
            return PresentMode::Count;
    }
}

VkSampleCountFlagBits RHISampleCountToVkSampleCount(SampleCount sample_count) {
    switch (sample_count) {
        case SampleCount::SC_1:
            return VK_SAMPLE_COUNT_1_BIT;
        case SampleCount::SC_2:
            return VK_SAMPLE_COUNT_2_BIT;
        case SampleCount::SC_4:
            return VK_SAMPLE_COUNT_4_BIT;
        case SampleCount::SC_8:
            return VK_SAMPLE_COUNT_8_BIT;
        case SampleCount::SC_16:
            return VK_SAMPLE_COUNT_16_BIT;
        case SampleCount::SC_32:
            return VK_SAMPLE_COUNT_32_BIT;
        case SampleCount::SC_64:
            return VK_SAMPLE_COUNT_64_BIT;
        case SampleCount::SC_Count:
            return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
        default:
            return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    }
}

SampleCount VkSampleCountToRHISampleCount(const VkSampleCountFlagBits sample_count) {
    switch (sample_count) {
        case VK_SAMPLE_COUNT_1_BIT:
            return SampleCount::SC_1;
        case VK_SAMPLE_COUNT_2_BIT:
            return SampleCount::SC_2;
        case VK_SAMPLE_COUNT_4_BIT:
            return SampleCount::SC_4;
        case VK_SAMPLE_COUNT_8_BIT:
            return SampleCount::SC_8;
        case VK_SAMPLE_COUNT_16_BIT:
            return SampleCount::SC_16;
        case VK_SAMPLE_COUNT_32_BIT:
            return SampleCount::SC_32;
        case VK_SAMPLE_COUNT_64_BIT:
            return SampleCount::SC_64;
        case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM:
            return SampleCount::SC_Count;
    }
    return SampleCount::SC_Count;
}

VkImageAspectFlags RHIImageAspectToVkImageAspect(int aspect) {
    VkImageAspectFlags flags = 0;
    if (aspect & ImageAspectBits::IA_Color) {
        flags |= VK_IMAGE_ASPECT_COLOR_BIT;
    }
    if (aspect & ImageAspectBits::IA_Depth) {
        flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    if (aspect & ImageAspectBits::IA_Stencil) {
        flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    return flags;
}

int VkImageAspectToRHIImageAspect(VkImageAspectFlags aspect) {
    int rtn = 0;
    if (rtn & VK_IMAGE_ASPECT_COLOR_BIT) {
        rtn |= ImageAspectBits::IA_Color;
    }
    if (rtn & VK_IMAGE_ASPECT_DEPTH_BIT) {
        rtn |= ImageAspectBits::IA_Depth;
    }
    if (rtn & VK_IMAGE_ASPECT_STENCIL_BIT) {
        rtn |= ImageAspectBits::IA_Stencil;
    }
    return rtn;
}

ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(const VkComponentSwizzle swizzle) {
    switch (swizzle) {
        case VK_COMPONENT_SWIZZLE_IDENTITY:
            return ComponentMappingElement::Identity;
        case VK_COMPONENT_SWIZZLE_ZERO:
            return ComponentMappingElement::Zero;
        case VK_COMPONENT_SWIZZLE_ONE:
            return ComponentMappingElement::One;
        case VK_COMPONENT_SWIZZLE_R:
            return ComponentMappingElement::R;
        case VK_COMPONENT_SWIZZLE_G:
            return ComponentMappingElement::G;
        case VK_COMPONENT_SWIZZLE_B:
            return ComponentMappingElement::B;
        case VK_COMPONENT_SWIZZLE_A:
            return ComponentMappingElement::A;
        case VK_COMPONENT_SWIZZLE_MAX_ENUM:
            return ComponentMappingElement::Count;
    }
    return ComponentMappingElement::Count;
}

VkComponentSwizzle RHIComponentMappingElementToVkComponentSwizzle(ComponentMappingElement swizzle) {
    switch (swizzle) {
        case ComponentMappingElement::Identity:
            return VK_COMPONENT_SWIZZLE_IDENTITY;
        case ComponentMappingElement::Zero:
            return VK_COMPONENT_SWIZZLE_ZERO;
        case ComponentMappingElement::One:
            return VK_COMPONENT_SWIZZLE_ONE;
        case ComponentMappingElement::R:
            return VK_COMPONENT_SWIZZLE_R;
        case ComponentMappingElement::G:
            return VK_COMPONENT_SWIZZLE_G;
        case ComponentMappingElement::B:
            return VK_COMPONENT_SWIZZLE_B;
        case ComponentMappingElement::A:
            return VK_COMPONENT_SWIZZLE_A;
        case ComponentMappingElement::Count:
            return VK_COMPONENT_SWIZZLE_MAX_ENUM;
    }
    return VK_COMPONENT_SWIZZLE_MAX_ENUM;
}

VkImageViewType RHIImageDimensionToVkImageViewType(ImageDimension type) {
    switch (type) {
        case ImageDimension::D1:
            return VK_IMAGE_VIEW_TYPE_1D;
        case ImageDimension::D2:
            return VK_IMAGE_VIEW_TYPE_2D;
        case ImageDimension::D3:
            return VK_IMAGE_VIEW_TYPE_3D;
        case ImageDimension::Cube:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        case ImageDimension::Array1D:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        case ImageDimension::Array2D:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case ImageDimension::ArrayCube:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        case ImageDimension::Count:
            return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

ImageDimension VkImageViewTypeToRHIImageDimension(const VkImageViewType type) {
    switch (type) {
        case VK_IMAGE_VIEW_TYPE_1D:
            return ImageDimension::D1;
        case VK_IMAGE_VIEW_TYPE_2D:
            return ImageDimension::D2;
        case VK_IMAGE_VIEW_TYPE_3D:
            return ImageDimension::D3;
        case VK_IMAGE_VIEW_TYPE_CUBE:
            return ImageDimension::Cube;
        case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
            return ImageDimension::Array1D;
        case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
            return ImageDimension::Array2D;
        case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
            return ImageDimension::ArrayCube;
        case VK_IMAGE_VIEW_TYPE_MAX_ENUM:
            return ImageDimension::Count;
    }
    return ImageDimension::Count;
}

VkBufferUsageFlags RHIBufferUsageToVkBufferUsage(BufferUsage usage) {
    VkBufferUsageFlags flags = 0;
    if (usage & BUB_VertexBuffer) {
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (usage & BUB_IndexBuffer) {
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if (usage & BUB_UniformBuffer) {
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (usage & BUB_TransferSrc) {
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if (usage & BUB_TransferDst) {
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    return flags;
}

BufferUsage VkBufferUsageToRHIBufferUsage(const VkBufferUsageFlags usage) {
    BufferUsage flags = 0;
    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        flags |= BUB_VertexBuffer;
    }
    if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        flags |= BUB_IndexBuffer;
    }
    if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        flags |= BUB_UniformBuffer;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
        flags |= BUB_TransferSrc;
    }
    if (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) {
        flags |= BUB_TransferDst;
    }
    return flags;
}

VkMemoryPropertyFlags RHIMemoryPropertyToVkMemoryProperty(BufferMemoryProperty property) {
    VkMemoryPropertyFlags flags = 0;
    if (property & BMPB_DeviceLocal) {
        flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    if (property & BMPB_HostVisible) {
        flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }
    if (property & BMPB_HostCoherent) {
        flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    return flags;
}

BufferMemoryProperty VkMemoryPropertyToRHIMemoryProperty(const VkMemoryPropertyFlags property) {
    BufferMemoryProperty flags = 0;
    if (property & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        flags |= BMPB_DeviceLocal;
    }
    if (property & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        flags |= BMPB_HostVisible;
    }
    if (property & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        flags |= BMPB_HostCoherent;
    }
    return flags;
}

VkImageLayout RHIImageLayoutToVkImageLayout(ImageLayout layout) {
    switch (layout) {
        case ImageLayout::Undefined:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageLayout::General:
            return VK_IMAGE_LAYOUT_GENERAL;
        case ImageLayout::ColorAttachment:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageLayout::DepthStencilAttachment:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ImageLayout::TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageLayout::TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case ImageLayout::ShaderReadOnly:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageLayout::PresentSrc:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        case ImageLayout::Count:
            return VK_IMAGE_LAYOUT_MAX_ENUM;
        default:
            return VK_IMAGE_LAYOUT_MAX_ENUM;
    }
}

ImageLayout VkImageLayoutToRHIImageLayout(const VkImageLayout layout) {
    switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return ImageLayout::Undefined;
        case VK_IMAGE_LAYOUT_GENERAL:
            return ImageLayout::General;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return ImageLayout::ColorAttachment;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return ImageLayout::DepthStencilAttachment;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return ImageLayout::TransferSrc;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return ImageLayout::TransferDst;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return ImageLayout::ShaderReadOnly;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return ImageLayout::PresentSrc;
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            return ImageLayout::Count;
        default:
            return ImageLayout::Count;
    }
}

VkAttachmentLoadOp RHIAttachmentLoadOpToVkAttachmentLoadOp(AttachmentLoadOperation load_op) {
    switch (load_op) {
        case AttachmentLoadOperation::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case AttachmentLoadOperation::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case AttachmentLoadOperation::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
    }
}

AttachmentLoadOperation VkAttachmentLoadOpToRHIAttachmentLoadOp(const VkAttachmentLoadOp load_op) {
    switch (load_op) {
        case VK_ATTACHMENT_LOAD_OP_LOAD:
            return AttachmentLoadOperation::Load;
        case VK_ATTACHMENT_LOAD_OP_CLEAR:
            return AttachmentLoadOperation::Clear;
        case VK_ATTACHMENT_LOAD_OP_DONT_CARE:
            return AttachmentLoadOperation::DontCare;
        default:
            return AttachmentLoadOperation::DontCare;
    }
}

VkAttachmentStoreOp RHIAttachmentStoreOpToVkAttachmentStoreOp(AttachmentStoreOperation store_op) {
    switch (store_op) {
        case AttachmentStoreOperation::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case AttachmentStoreOperation::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
    }
}

AttachmentStoreOperation VkAttachmentStoreOpToRHIAttachmentStoreOp(const VkAttachmentStoreOp store_op) {
    switch (store_op) {
        case VK_ATTACHMENT_STORE_OP_STORE:
            return AttachmentStoreOperation::Store;
        case VK_ATTACHMENT_STORE_OP_DONT_CARE:
            return AttachmentStoreOperation::DontCare;
        default:
            return AttachmentStoreOperation::DontCare;
    }
}

VkPolygonMode RHIPolygonModeToVkPolygonMode(rhi::PolygonMode polygon_mode) {
    switch (polygon_mode) {
        case PolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line:
            return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point:
            return VK_POLYGON_MODE_POINT;
        default:
            return VK_POLYGON_MODE_MAX_ENUM;
    }
}

PolygonMode VkPolygonModeToRHIPolygonMode(const VkPolygonMode polygon_mode) {
    switch (polygon_mode) {
        case VK_POLYGON_MODE_FILL:
            return PolygonMode::Fill;
        case VK_POLYGON_MODE_LINE:
            return PolygonMode::Line;
        case VK_POLYGON_MODE_POINT:
            return PolygonMode::Point;
        default:
            return PolygonMode::Count;
    }
}

VkFrontFace RHIFrontFaceToVkFrontFace(rhi::FrontFace front_face) {
    switch (front_face) {
        case FrontFace::Clockwise:
            return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default:
            return VK_FRONT_FACE_MAX_ENUM;
    }
}

rhi::FrontFace VkFrontFaceToRHIFrontFace(const VkFrontFace front_face) {
    switch (front_face) {
        case VK_FRONT_FACE_CLOCKWISE:
            return FrontFace::Clockwise;
        case VK_FRONT_FACE_COUNTER_CLOCKWISE:
            return FrontFace::CounterClockwise;
        default:
            return FrontFace::Count;
    }
}

VkCullModeFlags RHICullModeToVkCullMode(rhi::CullMode cull_mode) {
    switch (cull_mode) {
        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;
        case CullMode::None:
            return VK_CULL_MODE_NONE;
        default:
            return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
    }
}

rhi::CullMode VkCullModeToRHICullMode(const VkCullModeFlags cull_mode) {
    switch (cull_mode) {
        case VK_CULL_MODE_BACK_BIT:
            return CullMode::Back;
        case VK_CULL_MODE_FRONT_BIT:
            return CullMode::Front;
        case VK_CULL_MODE_FRONT_AND_BACK:
            return CullMode::FrontAndBack;
        case VK_CULL_MODE_NONE:
            return CullMode::None;
        default:
            return CullMode::Count;
    }
}

VkCompareOp RHICompareOpToVkCompareOp(rhi::CompareOp compare_op) {
    switch (compare_op) {
        case CompareOp::Never:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::Less:
            return VK_COMPARE_OP_LESS;
        case CompareOp::Equal:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::LessOrEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::Greater:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GreaterOrEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::Always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_MAX_ENUM;
    }
}

rhi::CompareOp VkCompareOpToRHICompareOp(const VkCompareOp compare_op) {
    switch (compare_op) {
        case VK_COMPARE_OP_NEVER:
            return CompareOp::Never;
        case VK_COMPARE_OP_LESS:
            return CompareOp::Less;
        case VK_COMPARE_OP_EQUAL:
            return CompareOp::Equal;
        case VK_COMPARE_OP_LESS_OR_EQUAL:
            return CompareOp::LessOrEqual;
        case VK_COMPARE_OP_GREATER:
            return CompareOp::Greater;
        case VK_COMPARE_OP_NOT_EQUAL:
            return CompareOp::NotEqual;
        case VK_COMPARE_OP_GREATER_OR_EQUAL:
            return CompareOp::GreaterOrEqual;
        case VK_COMPARE_OP_ALWAYS:
            return CompareOp::Always;
        default:
            return CompareOp::Count;
    }
}

VkShaderStageFlags RHIShaderStageToVkShaderStage(ShaderStage stage) {
    VkShaderStageFlags flags = 0;
    if (stage & ShaderStageBits::Vertex) {
        flags |= VK_SHADER_STAGE_VERTEX_BIT;
    }
    if (stage & ShaderStageBits::Fragment) {
        flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    if (stage & ShaderStageBits::Compute) {
        flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    }
    return flags;
}

ShaderStage VkShaderStageToRHIShaderStage(const VkShaderStageFlags stage) {
    ShaderStage flags = 0;
    if (stage & VK_SHADER_STAGE_VERTEX_BIT) {
        flags |= ShaderStageBits::Vertex;
    }
    if (stage & VK_SHADER_STAGE_FRAGMENT_BIT) {
        flags |= ShaderStageBits::Fragment;
    }
    if (stage & VK_SHADER_STAGE_COMPUTE_BIT) {
        flags |= ShaderStageBits::Compute;
    }
    return flags;
}

VkDescriptorType RHIDescriptorTypeToVkDescriptorType(rhi::DescriptorType type) {
    switch (type) {
        case DescriptorType::Sampler:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        // case DescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::SampledImage:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        // case DescriptorType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        // case DescriptorType::UniformTexelBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        // case DescriptorType::StorageTexelBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case DescriptorType::UniformBuffer:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // case DescriptorType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        // case DescriptorType::StorageBufferDynamic: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        default:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

rhi::DescriptorType VkDescriptorTypeToRHIDescriptorType(const VkDescriptorType type) {
    switch (type) {
        case VK_DESCRIPTOR_TYPE_SAMPLER:
            return DescriptorType::Sampler;
        // case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return DescriptorType::CombinedImageSampler;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return DescriptorType::SampledImage;
        // case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: return DescriptorType::StorageImage;
        // case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return DescriptorType::UniformTexelBuffer;
        // case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return DescriptorType::StorageTexelBuffer;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return DescriptorType::UniformBuffer;
        // case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return DescriptorType::StorageBuffer;
        // case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return DescriptorType::StorageBufferDynamic;
        default:
            return DescriptorType::Count;
    }
}

VkAccessFlags RHIAccessFlagToVkAccessFlag(rhi::AccessFlags access_flag) {
    if (access_flag == 0)
        return 0;
    VkAccessFlags flags = 0;
    if (access_flag & AFB_ColorAttachmentRead)
        flags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    if (access_flag & AFB_ColorAttachmentWrite)
        flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if (access_flag & AFB_DepthStencilAttachmentRead)
        flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    if (access_flag & AFB_DepthStencilAttachmentWrite)
        flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    if (access_flag & AFB_TransferRead)
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
    if (access_flag & AFB_TransferWrite)
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    if (access_flag & AFB_ShaderRead)
        flags |= VK_ACCESS_SHADER_READ_BIT;
    return flags;
}

rhi::AccessFlags VkAccessFlagToRHIAccessFlag(const VkAccessFlags access_flag) {
    if (access_flag == 0)
        return 0;
    rhi::AccessFlags flags = 0;
    if (access_flag & VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
        flags |= AFB_ColorAttachmentRead;
    if (access_flag & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
        flags |= AFB_ColorAttachmentWrite;
    if (access_flag & VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT)
        flags |= AFB_DepthStencilAttachmentRead;
    if (access_flag & VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
        flags |= AFB_DepthStencilAttachmentWrite;
    if (access_flag & VK_ACCESS_TRANSFER_READ_BIT)
        flags |= AFB_TransferRead;
    if (access_flag & VK_ACCESS_TRANSFER_WRITE_BIT)
        flags |= AFB_TransferWrite;
    if (access_flag & VK_ACCESS_SHADER_READ_BIT)
        flags |= AFB_ShaderRead;
    return flags;
}

VkPipelineStageFlags RHIPipelineStageToVkPipelineStage(rhi::PipelineStageFlags pipeline_stage) {
    if (pipeline_stage == 0)
        return 0;
    VkPipelineStageFlags flags = 0;
    if (pipeline_stage & PSFB_ColorAttachmentOutput)
        flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    if (pipeline_stage & PSFB_TopOfPipe)
        flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    if (pipeline_stage & PSFB_BottomOfPipe)
        flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    if (pipeline_stage & PSFB_DepthStencilAttachment)
        flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    if (pipeline_stage & PSFB_Transfer)
        flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    if (pipeline_stage & PSFB_FragmentShader)
        flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    return flags;
}

PipelineStageFlags VkPipelineStageToRHIPipelineStage(const VkPipelineStageFlags pipeline_stage) {
    if (pipeline_stage == 0)
        return 0;
    PipelineStageFlags flags = 0;
    if (pipeline_stage & VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        flags |= PSFB_ColorAttachmentOutput;
    if (pipeline_stage & VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
        flags |= PSFB_TopOfPipe;
    if (pipeline_stage & VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
        flags |= PSFB_BottomOfPipe;
    if (pipeline_stage & VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
        flags |= PSFB_DepthStencilAttachment;
    if (pipeline_stage & VK_PIPELINE_STAGE_TRANSFER_BIT)
        flags |= PSFB_Transfer;
    if (pipeline_stage & VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
        flags |= PSFB_FragmentShader;
    return flags;
}

VkVertexInputRate RHIVertexInputRateToVkVertexInputRate(rhi::VertexInputRate rate) {
    switch (rate) {
        case rhi::VertexInputRate::Vertex:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case rhi::VertexInputRate::Instance:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
        default:
            return VK_VERTEX_INPUT_RATE_MAX_ENUM;
    }
}

rhi::VertexInputRate VkVertexInputRateToRHIVertexInputRate(const VkVertexInputRate rate) {
    switch (rate) {
        case VK_VERTEX_INPUT_RATE_VERTEX:
            return rhi::VertexInputRate::Vertex;
        case VK_VERTEX_INPUT_RATE_INSTANCE:
            return rhi::VertexInputRate::Instance;
        default:
            return rhi::VertexInputRate::Count;
    }
}

VkImageType RHIImageDimensionToVkImageType(rhi::ImageDimension dimension) {
    switch (dimension) {
        case ImageDimension::D1:
            return VK_IMAGE_TYPE_1D;
        case ImageDimension::D2:
            return VK_IMAGE_TYPE_2D;
        case ImageDimension::D3:
            return VK_IMAGE_TYPE_3D;
        default:
            return VK_IMAGE_TYPE_MAX_ENUM;
    }
}

rhi::ImageDimension VkImageTypeToRHIImageDimension(const VkImageType type) {
    switch (type) {
        case VK_IMAGE_TYPE_1D:
            return ImageDimension::D1;
        case VK_IMAGE_TYPE_2D:
            return ImageDimension::D2;
        case VK_IMAGE_TYPE_3D:
            return ImageDimension::D3;
        default:
            return ImageDimension::Count;
    }
}

VkImageUsageFlags RHIImageUsageToVkImageUsageFlags(ImageUsage usage_flag) {
    VkImageUsageFlags flags = 0;
    if (usage_flag & IUB_TransferSrc)
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (usage_flag & IUB_TransferDst)
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (usage_flag & IUB_DepthStencil)
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (usage_flag & IUB_Transient)
        flags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    if (usage_flag & IUB_RenderTarget)
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (usage_flag & IUB_ShaderRead)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    return flags;
}

VkFilter RHIFilterToVkFilter(rhi::FilterMode filter) {
    switch (filter) {
        case rhi::FilterMode::Linear:
            return VK_FILTER_LINEAR;
        case rhi::FilterMode::Nearest:
            return VK_FILTER_NEAREST;
        default:
            return VK_FILTER_MAX_ENUM;
    }
}

rhi::FilterMode VkFilterToRHIFilter(const VkFilter filter) {
    switch (filter) {
        case VK_FILTER_LINEAR:
            return rhi::FilterMode::Linear;
        case VK_FILTER_NEAREST:
            return rhi::FilterMode::Nearest;
        default:
            return rhi::FilterMode::Count;
    }
}
VkSamplerAddressMode RHISamplerAddressModeToVkSamplerAddressMode(rhi::SamplerAddressMode address_mode) {
    switch (address_mode) {
        case rhi::SamplerAddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case rhi::SamplerAddressMode::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case rhi::SamplerAddressMode::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        default:
            return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
    }
}

rhi::SamplerAddressMode VkSamplerAddressModeToRHISamplerAddressMode(const VkSamplerAddressMode address_mode) {
    switch (address_mode) {
        case VK_SAMPLER_ADDRESS_MODE_REPEAT:
            return rhi::SamplerAddressMode::Repeat;
        case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
            return rhi::SamplerAddressMode::MirroredRepeat;
        case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
            return rhi::SamplerAddressMode::ClampToEdge;
        default:
            return rhi::SamplerAddressMode::Count;
    }
}

VkBlendFactor RHIBlendFactorToVkBlendFactor(rhi::BlendFactor blend_factor) {
    switch (blend_factor) {
        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        default:
            return VK_BLEND_FACTOR_MAX_ENUM;
    }
}

rhi::BlendFactor VkBlendFactorToRHIBlendFactor(const VkBlendFactor blend_factor) {
    switch (blend_factor) {
        case VK_BLEND_FACTOR_ZERO:
            return BlendFactor::Zero;
        case VK_BLEND_FACTOR_ONE:
            return BlendFactor::One;
        case VK_BLEND_FACTOR_SRC_ALPHA:
            return BlendFactor::SrcAlpha;
        case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
            return BlendFactor::OneMinusSrcAlpha;
        default:
            return BlendFactor::Count;
    }
}

ImageUsage VkImageUsageToRHIImageUsageFlags(const VkImageUsageFlags usage_flag) {
    ImageUsage flags = 0;
    if (usage_flag & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        flags |= IUB_TransferSrc;
    if (usage_flag & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        flags |= IUB_TransferDst;
    if (usage_flag & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        flags |= IUB_DepthStencil;
    if (usage_flag & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
        flags |= IUB_Transient;
    if (usage_flag & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        flags |= IUB_RenderTarget;
    if (usage_flag & VK_IMAGE_USAGE_SAMPLED_BIT)
        flags |= IUB_ShaderRead;
    return flags;
}

VkBlendOp RHIBlendOpToVkBlendOp(rhi::BlendOp op) {
    switch (op) {
        case BlendOp::Add:
            return VK_BLEND_OP_ADD;
        default:
            return VK_BLEND_OP_MAX_ENUM;
    }
}

rhi::BlendOp VKBlendOpToRHIBlendOp(const VkBlendOp op) {
    switch (op) {
        case VK_BLEND_OP_ADD:
            return BlendOp::Add;
        default:
            return BlendOp::Count;
    }
}
