/**
 * @file CommandList.cpp
 * @author Echo 
 * @Date 24-6-6
 * @brief 
 */

#include "CommandList.h"

#include "CommandPool.h"
#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

// 命令列表执行命令的辅助函数，所有调用均立即执行
struct CommandExecutor
{
    static void TransitionImageLayout(
        CommandPool& InPool, Image& InImage, vk::Format InFormat, vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout,
        UInt32 InMipLevel = 1
    ) {
        InPool.TrainsitionImageLayout(InImage.GetHandle(), InFormat, InOldLayout, InNewLayout, InMipLevel);
    }
};

void CommandList::TransitionImageLayout(
    Image& InImage, vk::Format InFormat, vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout, UInt32 InMipLevel, ECommandExecuteTime InExecuteTime
) {
    Initialize();
    switch (InExecuteTime) {
    case ECommandExecuteTime::Immediate:
        CommandExecutor::TransitionImageLayout(
            *mContext->GetCommandPool(), InImage, InFormat, InOldLayout, InNewLayout, InMipLevel
        );
        break;
    case ECommandExecuteTime::Deferred: break;
    }
}

void CommandList::Initialize() {
    if (mContext == nullptr) {
        mContext = &VulkanContext::Get();
    }
}

RHI_VULKAN_NAMESPACE_END
