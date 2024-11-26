/**
 * @file ImageView.h
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#pragma once

#include "Core/Core.h"
#include "Enums.h"
#include "Image.h"
#include "IResource.h"

namespace platform::rhi
{
class Image;
}
namespace platform::rhi
{


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

struct ImageViewDesc
{
    Image*                image;
    ImageViewType         type   = ImageViewType::Count;
    Format                format = Format::Count;
    ImageSubresourceRange subresource_range{};
    ComponentMapping      component_mapping{};
    core::StringView      name;   // 此Desc并不拥有name的生命周期, 其生命周期和父Image是一样的

    explicit ImageViewDesc(
        core::StringView name_, Image* image_, const ImageViewType type_ = ImageViewType::Count, const Format format_ = Format::Count,
        const ImageSubresourceRange& subresource_range_ = ImageSubresourceRange{}, const ComponentMapping component_mapping_ = ComponentMapping{}
    );
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
