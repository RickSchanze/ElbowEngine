//
// Created by Echo on 2025/3/26.
//

#include "ImageView.hpp"

#include "Image.hpp"
RHI::ImageViewDesc::ImageViewDesc(Image *image_, ImageDimension type_, Format format_, const ImageSubresourceRange &subresource_range_,
                                  const ComponentMapping component_mapping_) :
    image(image_), type(type_), format(format_), subresource_range(subresource_range_), component_mapping(component_mapping_) {
    if (image == nullptr) {
        Log(Error) << "Image cannot be nullptr when creating a ImageView.";
        return;
    }
    if (type_ == ImageDimension::Count) {
        switch (image->GetDimension()) {
            case ImageDimension::D1:
                type = ImageDimension::D1;
                break;
            case ImageDimension::D2:
                type = ImageDimension::D2;
                break;
            case ImageDimension::D3:
                type = ImageDimension::D3;
                break;
            case ImageDimension::Cube:
                type = ImageDimension::Cube;
                break;
            case ImageDimension::Array1D:
                type = ImageDimension::Array1D;
                break;
            case ImageDimension::Array2D:
                type = ImageDimension::Array2D;
                break;
            case ImageDimension::ArrayCube:
                type = ImageDimension::ArrayCube;
                break;
            default: {
                Log(Error) << "Unknown image view type.";
                return;
            }
        }
    }
    if (format == Format::Count) {
        format = image->GetFormat();
    } else if (format == Format::R32G32_Float) {
        component_mapping.r = ComponentMappingElement::Identity;
        component_mapping.g = ComponentMappingElement::Zero;
        component_mapping.b = ComponentMappingElement::Zero;
        component_mapping.a = ComponentMappingElement::Zero;
    }
    {
        // layer_count

        if (type == ImageDimension::Cube) {
            if (subresource_range.layer_count != 6)
                subresource_range.layer_count = 6;
        } else if (type == ImageDimension::Array1D || type == ImageDimension::Array2D || type == ImageDimension::ArrayCube) {
            if (subresource_range.layer_count == -1) {
                Log(Error) << "layer_count must be specified when creating an array image view.";
                return;
            }
        } else {
            subresource_range.layer_count = 1;
        }
    }
    if (subresource_range.level_count == -1) {
        subresource_range.level_count = image->GetMipLevelCount();
    }
    if (subresource_range.aspect_mask == -1) {
        Log(Error) << "aspect_mask must be specified when creating an image view.";
        return;
    }
}

RHI::ImageViewDesc::ImageViewDesc(Image *image_) {
    image = image_;
    if (image_->GetDepthOrLayers() == 6) {
        type = ImageDimension::Cube;
    } else {
        type = ImageDimension::D2;
    }
    format = image->GetFormat();
    ImageAspect aspect = 0;
    ImageUsage usage = image->GetUsage();
    if (usage & IUB_RenderTarget)
        aspect |= IA_Color;
    if (usage & IUB_SwapChain)
        aspect |= IA_Color;
    if (usage & IUB_DepthStencil)
        aspect |= IA_Depth;
    if (usage & IUB_ShaderRead)
        aspect |= IA_Color;
    subresource_range.aspect_mask = aspect;
    subresource_range.layer_count = image->GetDepthOrLayers();
    subresource_range.base_array_layer = 0;
    subresource_range.level_count = image->GetMipLevelCount();
    subresource_range.base_mip_level = 0;
    if (format == Format::R8_SRGB) {
        component_mapping.a = ComponentMappingElement::R;
        component_mapping.g = ComponentMappingElement::One;
        component_mapping.b = ComponentMappingElement::One;
        component_mapping.r = ComponentMappingElement::One;
    } else {
        component_mapping.a = ComponentMappingElement::Identity;
        component_mapping.b = ComponentMappingElement::Identity;
        component_mapping.g = ComponentMappingElement::Identity;
        component_mapping.r = ComponentMappingElement::Identity;
    }
}
