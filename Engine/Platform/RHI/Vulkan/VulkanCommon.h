/**
 * @file VulkanCommon.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Exception.h"
#define RHI_VULKAN_NAMESPACE_BEGIN namespace RHI::Vulkan {
#define RHI_VULKAN_NAMESPACE_END }

#undef THROW_IF_NOT
#define THROW_IF_NOT(Condition, Message) \
    if (!(Condition)) throw VulkanException(Message)

#define VULKAN_CHECK_PTR(Ptr) Ptr != nullptr && Ptr->IsValid()
