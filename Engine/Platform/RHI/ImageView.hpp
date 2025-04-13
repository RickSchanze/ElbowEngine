//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Enums.hpp"
#include "IResource.hpp"

namespace RHI {
    class Image;
    struct ComponentMapping {
        ComponentMappingElement r;
        ComponentMappingElement g;
        ComponentMappingElement b;
        ComponentMappingElement a;

        explicit ComponentMapping(const ComponentMappingElement r_ = ComponentMappingElement::Identity,
                                  const ComponentMappingElement g_ = ComponentMappingElement::Identity,
                                  const ComponentMappingElement b_ = ComponentMappingElement::Identity,
                                  const ComponentMappingElement a_ = ComponentMappingElement::Identity) : r(r_), g(g_), b(b_), a(a_) {}
    };

    struct ImageViewDesc {
        Image *image;
        ImageDimension type = ImageDimension::Count;
        Format format = Format::Count;
        ImageSubresourceRange subresource_range{};
        ComponentMapping component_mapping{};

        /**
         * 参数完全指定的版本
         * @param image_
         * @param type_
         * @param format_
         * @param subresource_range_
         * @param component_mapping_
         */
        explicit ImageViewDesc(Image *image_, ImageDimension type_, Format format_ = Format::Count,
                               const ImageSubresourceRange &subresource_range_ = ImageSubresourceRange{},
                               const ComponentMapping component_mapping_ = ComponentMapping{});

        /**
         * 只指定aspect_mask的版本
         * @param image_
         */
        explicit ImageViewDesc(Image *image_);
    };

    class ImageView : public IResource {
    public:
        explicit ImageView(const ImageViewDesc &decs_) : desc_(decs_) {}

        [[nodiscard]] Image *GetImage() const { return desc_.image; }
        [[nodiscard]] ImageDimension GetType() const { return desc_.type; }
        [[nodiscard]] Format GetFormat() const { return desc_.format; }
        [[nodiscard]] const ImageSubresourceRange &GetSubresourceRange() const { return desc_.subresource_range; }
        [[nodiscard]] const ComponentMapping &GetComponentMapping() const { return desc_.component_mapping; }


    protected:
        ImageViewDesc desc_;
    };
} // namespace rhi
