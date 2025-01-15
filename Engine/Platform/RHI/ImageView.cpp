/**
 * @file ImageView.cpp
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#include "ImageView.h"

#include "Image.h"
#include "Platform/PlatformLogcat.h"
platform::rhi::ImageViewDesc::ImageViewDesc(
    Image* image_, const ImageDimension type_, const Format format_, const ImageSubresourceRange& subresource_range_,
    const ComponentMapping component_mapping_
) : image(image_), type(type_), format(format_), subresource_range(subresource_range_), component_mapping(component_mapping_)
{
    if (image == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI, "Image cannot be nullptr when creating a ImageView.");
        return;
    }
    if (type_ == ImageDimension::Count)
    {
        switch (image->GetDimension())
        {
        case ImageDimension::D1: type = ImageDimension::D1; break;
        case ImageDimension::D2: type = ImageDimension::D2; break;
        case ImageDimension::D3: type = ImageDimension::D3; break;
        case ImageDimension::Cube: type = ImageDimension::Cube; break;
        case ImageDimension::Array1D: type = ImageDimension::Array1D; break;
        case ImageDimension::Array2D: type = ImageDimension::Array2D; break;
        case ImageDimension::ArrayCube: type = ImageDimension::ArrayCube; break;
        default: {
            LOGGER.Error(logcat::Platform_RHI, "Unknown image view type.");
            return;
        }
        }
    }
    if (format == Format::Count)
    {
        format = image->GetFormat();
    }
    {
        // layer_count
        if (type == ImageDimension::Cube)
        {
            if (subresource_range.layer_count != 6) subresource_range.layer_count = 6;
        }
        else if (type == ImageDimension::Array1D || type == ImageDimension::Array2D || type == ImageDimension::ArrayCube)
        {
            if (subresource_range.layer_count == -1)
            {
                LOGGER.Error(logcat::Platform_RHI, "layer_count must be specified when creating an array image view.");
                return;
            }
        }
        else
        {
            subresource_range.layer_count = 1;
        }
    }
    if (subresource_range.level_count == -1)
    {
        subresource_range.level_count = image->GetMipLevels();
    }
    if (subresource_range.aspect_mask == -1)
    {
        LOGGER.Error(logcat::Platform_RHI, "aspect_mask must be specified when creating an image view.");
        return;
    }
}

platform::rhi::ImageViewDesc::ImageViewDesc(Image* image_)
{
    image              = image_;
    type               = image->GetDimension();
    format             = image->GetFormat();
    ImageAspect aspect = 0;
    ImageUsage  usage  = image->GetUsage();
    if (usage & IUB_RenderTarget) aspect |= IA_Color;
    if (usage & IUB_SwapChain) aspect |= IA_Color;
    if (usage & IUB_DepthStencil) aspect |= IA_Depth;
    if (usage & IUB_ShaderRead) aspect |= IA_Color;
    subresource_range.aspect_mask      = aspect;
    subresource_range.layer_count      = image->GetDepthOrLayers();
    subresource_range.base_array_layer = 0;
    subresource_range.level_count      = image->GetMipLevels();
    subresource_range.base_mip_level   = 0;
    component_mapping.a                = ComponentMappingElement::Identity;
    component_mapping.b                = ComponentMappingElement::Identity;
    component_mapping.g                = ComponentMappingElement::Identity;
    component_mapping.r                = ComponentMappingElement::Identity;
}
