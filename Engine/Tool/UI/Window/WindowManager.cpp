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
    if (Mgr->windows_map_.contains(type)) {
        return Mgr->windows_map_[type];
    }
    auto NewWindowInstanceVar = type.create();
    if (NewWindowInstanceVar) {
        auto window = NewWindowInstanceVar.get_value<Tool::Window::WindowBase*>();
        window->Construct();
        ObjectManager::Get()->AddObject(window);
        Mgr->windows_map_[type] = window;
        return window;
    } else {
        LOG_ERROR_CATEGORY(UI, L"新建一个类型为{}的Window失败", StringUtils::FromAnsiStringView(type.get_name()));
        return nullptr;
    }
}

void WindowManager::DestroyWindow(const Type& type) {
    auto& mgr = *Get();
    if (mgr.windows_map_.contains(type)) {
        ObjectManager::Get()->RemoveObject(mgr.windows_map_[type]->GetID());
        delete mgr.windows_map_[type];
        mgr.windows_map_.erase(type);
    } else {
        LOG_WARNING_CATEGORY(UI, L"试图删除不存在{}类型的Window", StringUtils::FromAnsiStringView(type.get_name()));
    }
}
