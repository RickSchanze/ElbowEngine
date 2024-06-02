/**
 * @file WindowRegistry.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "WindowManager.h"

#include "CoreGlobal.h"
#include "Utils/StringUtils.h"
#include "WindowBase.h"

Tool::Window::WindowBase* WindowManager::GetWindow(Type InType) {
    auto& Mgr = Get();
    if (Mgr.mWindowMap.contains(InType)) {
        return Mgr.mWindowMap[InType];
    }
    auto NewWindowInstanceVar = InType.create();
    if (NewWindowInstanceVar) {
        auto Window = NewWindowInstanceVar.get_value<Tool::Window::WindowBase*>();
        Mgr.mWindowMap[InType] = Window;
        return Window;
    } else {
        LOG_ERROR_CATEGORY(UI, L"新建一个类型为{}的Window失败", StringUtils::FromAnsiStringView(InType.get_name()));
        return nullptr;
    }
}

void WindowManager::DestroyWindow(Type InType) {
    auto& Mgr = Get();
    if (Mgr.mWindowMap.contains(InType)) {
        delete Mgr.mWindowMap[InType];
        Mgr.mWindowMap.erase(InType);
    } else {
        LOG_WARNING_CATEGORY(UI, L"试图删除不存在{}类型的Window", StringUtils::FromAnsiStringView(InType.get_name()));
    }
}

WindowManager::~WindowManager() {
    for (const auto& Pair: mWindowMap) {
        delete Pair.second;
    }
}
