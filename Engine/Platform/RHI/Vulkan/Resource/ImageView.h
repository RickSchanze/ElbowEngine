/**
 * @file ImageView.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "Core/Base/EString.h"
#include "Platform/RHI/Vulkan/Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
class LogicalDevice;

class ImageView : public IRHIResource
{
public:
    explicit ImageView() = default;

    explicit ImageView(const vk::ImageView& handle, const char* debug_name = nullptr);

    ~ImageView() override;

    bool IsValid() const { return static_cast<bool>(handle_); }

    vk::ImageView GetHandle() const { return handle_; }

    void InternalDestroy();

    void Destroy() override;

private:
    vk::ImageView handle_ = VK_NULL_HANDLE;

    core::String debug_name_;
};
}   // namespace rhi::vulkan
