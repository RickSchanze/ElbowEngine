//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/ImageView.hpp"
#include "vulkan/vulkan.h"

namespace NRHI {

VkComponentMapping FromComponentMapping(const ComponentMapping &mapping);

class ImageView_Vulkan : public ImageView {
  friend class Image;

public:
  explicit ImageView_Vulkan(const ImageViewDesc& desc);

    virtual ~ImageView_Vulkan() override;

    virtual void *GetNativeHandle() const override { return handle_; }

private:
  VkImageView handle_ = VK_NULL_HANDLE;
};
} // namespace rhi
