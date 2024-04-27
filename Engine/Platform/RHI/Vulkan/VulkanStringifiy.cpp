/**
 * @file VulkanStringifiy.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "VulkanStringifiy.h"
String VulkanStringifiy::ToString(const vk::Extent2D& InExtent) {
    return std::format(L"(Width={}, Height={})", InExtent.width, InExtent.height);
}
