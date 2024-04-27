/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "Image.h"

RHI::Vulkan::ImageBase::~ImageBase() {
    mImageHandle = VK_NULL_HANDLE;
}
