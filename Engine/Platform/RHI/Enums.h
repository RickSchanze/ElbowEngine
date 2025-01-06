/**
 * @file RHIEnums.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once
#include "Core/Core.h"

namespace platform::rhi
{
enum class ENUM() Format
{
    R32G32B32_Float,        // 每个元素由3个32位浮点数分量构成
    R16G16B16A16_UNorm,     // 每个元素由4个16位分量构成, 每个分量被映射到[0, 1]
    R32G32_UInt,            // 每个元素由2个32位无符号整数构成
    R8G8B8A8_UNorm,         // 每个元素由4个8位无符号数构成, 被映射到[0, 1]
    R8G8B8A8_SNorm,         // 每个元素由4个8位有符号数构成, 被映射到[-1, 1]
    R8G8B8A8_UInt,          // 每个元素由4个8位无符号数构成, 被映射到[0, 255]
    R8G8B8A8_SInt,          // 每个元素由4个8位有符号数构成, 被映射到[-128, 127]
    D32_Float_S8X24_UInt,   // 深度/模版: 64位 32位给深度, 8位给模版([0, 255]), 24位用作对齐
    D32_Float,              // 深度: 32位浮点数
    D24_UNorm_S8_UInt,      // 深度/模版: 24位给深度([0, 1]), 8位给模版([0, 255])
    D16_UNorm,              // 深度: 16位([0, 1])
    B8G8R8A8_SRGB,          // 每个元素由4个8位分量构成, 被映射到sRGB
    B8G8R8A8_UNorm,         // 每个元素由4个8位分量构成, 被映射到[0, 1]
    R16G16B16A16_Float,     // 每个元素由4个16位float分量构成
    A2B10G10R10_UNorm,      // a: 2位 b: 10位 g: 10位 r: 10位
    R32G32_SFloat,          // 每个元素由2个32位float分量构成
    Count,                  // 超出范围(Undefined)
};

enum class ENUM() ColorSpace
{
    sRGB,    // 非线性sRGB, 适用于大部分普通内容, 标准显示器
    HDR10,   // HDR10
    Count,   // 超出范围
};

enum class ENUM() PresentMode
{
    VSync,          // 垂直同步
    Immediate,      // 立即刷新
    TripleBuffer,   // 三重缓冲
    Count,          // 超出范围
};

enum class ENUM() GraphicsAPI
{
    Vulkan,
    D3D12,
    OpenGL,
    Null,
    Count,
};

enum class QueueFamilyType
{
    Graphics,
    Compute,
    Transfer,
    Present,
};

/**
 * MSAA采样次数
 * Vulkan API指示这里是个Flag Enum, Follow it.
 */
enum ENUM(Flag) SampleCountBits
{
    SC_1  = 0b0000001,   // 1次
    SC_2  = 0b0000010,   // 2次
    SC_4  = 0b0000100,   // 4次
    SC_8  = 0b0001000,   // 8次
    SC_16 = 0b0010000,   // 16次
    SC_32 = 0b0100000,   // 32次
    SC_64 = 0b1000000,   // 64次
    SC_Count,            // 超出范围
};
typedef uint32_t SampleCount;

// 指示要访问图像的哪些"方面"
enum ImageAspectBits
{
    IA_Color   = 0b1,     // 颜色
    IA_Depth   = 0b10,    // 深度
    IA_Stencil = 0b100,   // 模板
    IA_Max,
};
typedef uint32_t ImageAspect;

enum BufferUsageBits
{
    BUB_VertexBuffer  = 1,
    BUB_IndexBuffer   = 1 << 1,
    BUB_UniformBuffer = 1 << 2,
    BUB_TransferSrc   = 1 << 3,
    BUB_TransferDst   = 1 << 4,
};
typedef uint32_t BufferUsage;

enum BufferMemoryPropertyBits
{
    BMPB_DeviceLocal  = 1,        // GPU Only CPU无法访问(纹理、IndexBuffer、VertexBuffer)
    BMPB_HostVisible  = 1 << 1,   // CPU可以访问(UniformBuffer) 通常和 HostCoherent 一起使用
    BMPB_HostCoherent = 1 << 2,   // CPU可以访问, 但GPU会自动刷新(UniformBuffer)而不用手动同步
    BMPB_Max          = 0x7FFFFFFF,
};
typedef uint32_t BufferMemoryProperty;

/**
 * 对应VkImageSubresourceRange
 * 当创建ImageView时, -1代表自动填充, 所需信息会从Image里取
 */
struct ImageSubresourceRange
{
    /** ImageAspect */ int32_t aspect_mask = -1;

    uint32_t base_mip_level   = 0;
    uint32_t level_count      = -1;
    uint32_t base_array_layer = 0;
    uint32_t layer_count      = -1;

    explicit ImageSubresourceRange(/** ImageAspect */ const int32_t aspect_mask_ = -1, const uint32_t base_mip_level_ = 0,
                                   const uint32_t level_count_ = -1, const uint32_t base_array_layer_ = 0, const uint32_t layer_count_ = -1) :
        aspect_mask(aspect_mask_), base_mip_level(base_mip_level_), level_count(level_count_), base_array_layer(base_array_layer_),
        layer_count(layer_count_)
    {
    }
};

enum class ImageViewType
{
    D1,
    D2,
    D3,
    Cube,
    Array1D,
    Array2D,
    ArrayCube,
    Count,
};

enum class ComponentMappingElement
{
    Identity,
    Zero,
    One,
    R,
    G,
    B,
    A,
    Count,
};

enum class AttachmentLoadOperation
{
    Load,
    Clear,
    DontCare,
    Count,
};

enum class AttachmentStoreOperation
{
    Store,
    DontCare,
    Count,
};

enum class ImageLayout
{
    Undefined,
    General,
    ColorAttachment,
    DepthStencilAttachment,
    TransferSrc,
    TransferDst,
    ShaderReadOnly,
    PresentSrc,
    Count,
};

enum class PolygonMode
{
    Fill,
    Line,
    Point,
    Count
};

enum class CullMode
{
    None,
    Front,
    Back,
    FrontAndBack,
    Count,
};

enum class FrontFace
{
    Clockwise,
    CounterClockwise,
    Count,
};

enum class CompareOp
{
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Equal,
    NotEqual,
    Never,
    Always,
    Count,
};

enum ShaderStageBits
{
    Vertex   = 1,
    Fragment = 1 << 1,
    Compute  = 1 << 2,
};
typedef uint32_t ShaderStage;

enum class DescriptorType
{
    Sampler,
    UniformBuffer,
    UniformBufferDynamic,
    Count,
};

enum AccessFlagBits
{
    AFB_None                        = 0,
    AFB_ColorAttachmentRead         = 1,
    AFB_ColorAttachmentWrite        = 1 << 1,
    AFB_DepthStencilAttachmentRead = 1 << 2,
    AFB_DepthStencilAttachmentWrite = 1 << 3,
    AFB_TransferRead = 1 << 4,
    AFB_TransferWrite = 1 << 5,
};
using AccessFlags = uint32_t;

enum PipelineStageFlagBits
{
    PSFB_None = 0,
    PSFB_TopOfPipe = 1,
    PSFB_ColorAttachmentOutput = 1 << 1,
    PSFB_BottomOfPipe = 1 << 2,
};
using PipelineStageFlags = uint32_t;
}
