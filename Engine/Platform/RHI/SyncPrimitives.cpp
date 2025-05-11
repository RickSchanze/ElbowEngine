//
// Created by Echo on 2025/3/26.
//
#include "SyncPrimitives.hpp"

#include "Core/Logger/Logger.hpp"

bool NRHI::Semaphore::Vulkan_IsTimelineSemaphore()  {
    Log(Fatal) << "此函数只能有Vulkan后端调用";
    return false;
}