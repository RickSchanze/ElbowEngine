//
// Created by Echo on 2025/3/25.
//

#include "SystemInfo.hpp"
#include "Core/PlatformDef.hpp"
#ifdef ELBOW_PLATFORM_WINDOWS
#include "Windows.h"
#endif

UInt32 SystemInfo::GetCPUCoreCount() {
#ifdef ELBOW_PLATFORM_WINDOWS
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return sys_info.dwNumberOfProcessors;
#endif
    return 0;
}
