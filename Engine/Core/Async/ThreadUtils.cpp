//
// Created by Echo on 24-12-12.
//

#include "ThreadUtils.h"
#include "Core/CoreDef.h"
#include "ThreadManager.h"

#ifdef PLATFORM_WINDOWS
#include "Windows.h"
#endif

void core::ThreadUtils::SetThreadName(std::thread& thread, core::StringView name)
{
#ifdef PLATFORM_WINDOWS
    HANDLE       handle = static_cast<HANDLE>(thread.native_handle());
    std::wstring w_name = {name.GetStdStringView().begin(), name.GetStdStringView().end()};
    SetThreadDescription(handle, w_name.c_str());
#endif
}

bool core::ThreadUtils::IsCurrentMainThread()
{
    return std::this_thread::get_id() == ThreadManager::GetMainThreadId();
}

void core::ThreadUtils::YieldThread()
{
    std::this_thread::yield();
}
