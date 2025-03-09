//
// Created by Echo on 25-3-7.
//

#include "SystemInfo.h"

#include "Core/CoreDef.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

using namespace core;

UInt32 SystemInfo::GetCPUCoreCount() {
#ifdef PLATFORM_WINDOWS
  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  return sys_info.dwNumberOfProcessors;
#endif
  return 0;
}