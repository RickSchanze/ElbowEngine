/**
 * @file CommandList.h
 * @author Echo 
 * @Date 24-6-6
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan {
class Image;
}
namespace RHI::Vulkan {
class VulkanContext;
}
RHI_VULKAN_NAMESPACE_BEGIN

enum class ECommandExecuteTime {
    Immediate,   // 立即执行此命令
    Deferred     // 延迟执行此命令（记录完所有后执行）
};

class CommandList {
public:
    // 变换图像布局 默认是立即调用
    void TransitionImageLayout(
        Image& InImage, vk::Format InImageFormat, vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout, UInt32 InMipLevel = 1,
        ECommandExecuteTime InExecuteTime = ECommandExecuteTime::Immediate
    );

protected:
    void Initialize();

    // 记录使用的Context 将在任意命令调用时初始化
    VulkanContext* mContext = nullptr;
};

RHI_VULKAN_NAMESPACE_END
