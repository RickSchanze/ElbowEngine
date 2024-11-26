/**
 * @file IResource.cpp
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#include "IResource.h"
bool platform::rhi::IResource::IsValid() const
{
    return GetNativeHandle() != nullptr;
}
