/**
 * @file WindowRegistry.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "WindowManager.h"

#include "CoreGlobal.h"
#include "Object/ObjectManager.h"
#include "Utils/StringUtils.h"
#include "WindowBase.h"

Tool::Window::WindowBase* WindowManager::GetWindow(Type type) {
    auto Mgr = Get();
    if (Mgr->mWindowMap.contains(type)) {
        return Mgr->mWindowMap[type];
    }
    auto NewWindowInstanceVar = type.create();
    if (NewWindowInstanceVar) {
        auto window = NewWindowInstanceVar.get_value<Tool::Window::WindowBase*>();
        window->Construct();
        ObjectManager::Get()->AddObject(window);
        Mgr->mWindowMap[type] = window;
        return window;
    } else {
        LOG_ERROR_CATEGORY(UI, L"新建一个类型为{}的Window失败", StringUtils::FromAnsiStringView(type.get_name()));
        return nullptr;
    }
}

void WindowManager::DestroyWindow(Type type) {
    auto& Mgr = *Get();
    if (Mgr.mWindowMap.contains(type)) {
        delete Mgr.mWindowMap[type];
        Mgr.mWindowMap.erase(type);
    } else {
        LOG_WARNING_CATEGORY(UI, L"试图删除不存在{}类型的Window", StringUtils::FromAnsiStringView(type.get_name()));
    }
}
