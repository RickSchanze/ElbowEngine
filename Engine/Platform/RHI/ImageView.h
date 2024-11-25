/**
 * @file ImageView.h
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#pragma once

#include "Core/Core.h"
#include "Enums.h"
#include "IResource.h"
#include "Platform/PlatformLogcat.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi
{
class Image;
}
namespace platform::rhi
{
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

struct ComponentMapping
{
    ComponentMappingElement r;
    ComponentMappingElement g;
    ComponentMappingElement b;
    ComponentMappingElement a;

    explicit ComponentMapping(
        const ComponentMappingElement r_ = ComponentMappingElement::Identity, const ComponentMappingElement g_ = ComponentMappingElement::Identity,
        const ComponentMappingElement b_ = ComponentMappingElement::Identity, const ComponentMappingElement a_ = ComponentMappingElement::Identity
    ) : r(r_), g(g_), b(b_), a(a_)
    {
    }
};

// 指示要访问图像的哪些"方面"
enum ImageAspect
{
    IA_Color   = 0b1,     // 颜色
    IA_Depth   = 0b10,    // 深度
    IA_Stencil = 0b100,   // 模板
    IA_Max,
};

/**
 * 对应VkImageSubresourceRange
 * 当创建ImageView时, -1代表自动填充, 所需信息会从Image里取
 */
struct ImageSubresourceRange
{
    /** ImageAspect */ int32_t aspectMask = -1;

    uint32_t base_mip_level   = 0;
    uint32_t level_count      = -1;
    uint32_t base_array_layer = 0;
    uint32_t layer_count      = -1;

    explicit ImageSubresourceRange(/** ImageAspect */ const int32_t aspect_mask_ = -1, const uint32_t base_mip_level_ = 0,
                                   const uint32_t level_count_ = -1, const uint32_t base_array_layer_ = 0, const uint32_t layer_count_ = -1) :
        aspectMask(aspect_mask_), base_mip_level(base_mip_level_), level_count(level_count_), base_array_layer(base_array_layer_),
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
    D1Array,
    D2Array,
    CubeArray,
    Count,
};

struct ImageViewDesc
{
    Image*                image;
    ImageViewType         type   = ImageViewType::Count;
    Format                format = Format::Count;
    ImageSubresourceRange subresource_range{};
    ComponentMapping      component_mapping{};

    explicit ImageViewDesc(
        Image* image_, const ImageViewType type_ = ImageViewType::Count, const Format format_ = Format::Count,
        const ImageSubresourceRange& subresource_range_ = ImageSubresourceRange{}, const ComponentMapping component_mapping_ = ComponentMapping{}
    ) : image(image_), type(type_), format(format_), subresource_range(subresource_range_), component_mapping(component_mapping_)
    {
    }
};

class ImageView : public IResource
{
public:
    explicit ImageView(const ImageViewDesc& decs_) : desc_(decs_) {}

    ~ImageView() override;

    [[nodiscard]] Image*                       GetImage() const { return desc_.image; }
    [[nodiscard]] ImageViewType                GetType() const { return desc_.type; }
    [[nodiscard]] Format                       GetFormat() const { return desc_.format; }
    [[nodiscard]] const ImageSubresourceRange& GetSubresourceRange() const { return desc_.subresource_range; }
    [[nodiscard]] const ComponentMapping&      GetComponentMapping() const { return desc_.component_mapping; }

protected:
    ImageViewDesc desc_;
};


}   // namespace platform::rhi
