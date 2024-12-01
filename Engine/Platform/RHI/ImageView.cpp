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
    const core::StringView name_, Image* image_, const ImageViewType type_, const Format format_, const ImageSubresourceRange& subresource_range_,
    const ComponentMapping component_mapping_
) : image(image_), type(type_), format(format_), subresource_range(subresource_range_), component_mapping(component_mapping_), name(name_)
{
    if (image == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI, "Image cannot be nullptr when creating a ImageView.");
        return;
    }
    if (type_ == ImageViewType::Count)
    {
        switch (image->GetDimension())
        {
        case ImageDimension::D1: type = ImageViewType::D1; break;
        case ImageDimension::D2: type = ImageViewType::D2; break;
        case ImageDimension::D3: type = ImageViewType::D3; break;
        case ImageDimension::Cube: type = ImageViewType::Cube; break;
        case ImageDimension::Array1D: type = ImageViewType::Array1D; break;
        case ImageDimension::Array2D: type = ImageViewType::Array2D; break;
        case ImageDimension::ArrayCube: type = ImageViewType::ArrayCube; break;
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
        if (type == ImageViewType::Cube)
        {
            if (subresource_range.layer_count != 6) subresource_range.layer_count = 6;
        }
        else if (type == ImageViewType::Array1D || type == ImageViewType::Array2D || type == ImageViewType::ArrayCube)
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

platform::rhi::ImageViewDesc::ImageViewDesc(const core::StringView name_, Image* image_, const int32_t aspect_mask) :
    ImageViewDesc(name_, image_, ImageViewType::Count, Format::Count, ImageSubresourceRange{aspect_mask})
{
}
