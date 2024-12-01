/**
 * @file Image_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#include "Image_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/GfxContext.h"
platform::rhi::vulkan::Image_Vulkan::Image_Vulkan(const ImageDesc& desc) : Image(desc)
{
    auto ctx = static_cast<GfxContext_Vulkan*>(GetGfxContext());
}

const char* swapchain_image_names[] = {
    "SwapChainImage0",
    "SwapChainImage1",
    "SwapChainImage2",
    "SwapChainImage3",
    "SwapChainImage4",
    "SwapChainImage5",
    "SwapChainImage6",
    "SwapChainImage7",
    "SwapChainImage8",   // 开发设备最多支持八个
};

platform::rhi::vulkan::Image_Vulkan::Image_Vulkan(
    const VkImage handle_, const int32_t index, const uint32_t width_, const uint32_t height_, const Format format_
)
{
    image_handle_         = handle_;
    desc_.width           = width_;
    desc_.height          = height_;
    desc_.format          = format_;
    desc_.usage           = IU_SwapChain;
    desc_.depth_or_layers = 1;
    desc_.mip_levels      = 1;
    // TODO: samples应该为1吗？
    desc_.samples         = SC_1;
    desc_.initial_state   = ImageState::Undefined;
    desc_.dimension       = ImageDimension::D2;
    desc_.name            = swapchain_image_names[index];
}

platform::rhi::vulkan::Image_Vulkan::~Image_Vulkan(){
    if (desc_.usage & IU_SwapChain)
    {
        if (desc_.usage != IU_SwapChain)
        {
            LOGGER.Warn(logcat::Platform_RHI_Vulkan, "Image usage IU_SwapChain cannot combined with other usage.");
        }
        else
        {
            // 交换链的Image由交换链销毁
            return;
        }
    }
}
