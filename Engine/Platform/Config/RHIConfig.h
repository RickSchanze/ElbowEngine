/**
 * @file RHIConfig.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Core/Config/IConfig.h"
#include "Core/Core.h"

namespace platform::rhi
{
class CLASS(Config = "Config/Platform/RHIConfig.cfg", Category = "Platform") RHIConfig : extends core::IConfig
{
    // 交换链图像数量
    DECLARE_CONFIG_ITEM(uint8_t, swapchain_count, SwapchainImageCount, 2);
};
}   // namespace platform::rhi
