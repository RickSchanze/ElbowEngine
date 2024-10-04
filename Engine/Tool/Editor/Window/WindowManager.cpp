/**
 * @file WindowManager.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "WindowManager.h"

#include "CoreGlobal.h"
#include "Object/ObjectManager.h"
#include "Utils/ContainerUtils.h"
#include "Utils/StringUtils.h"
#include "WindowBase.h"

namespace tool::window {

WindowBase* WindowManager::GetOrCreateWindow(const Type& type)
{
    auto mgr = Get();
    if (type)
    {
        if (mgr->singleton_windows_.contains(type))
        {
            return mgr->singleton_windows_[type];
        }
        auto new_window_instance = type.create();
        if (new_window_instance)
        {
            auto window = new_window_instance.get_value<WindowBase*>();
            if (window)
            {
                if (window->IsSingleton())
                {
                    ObjectManager::Get()->AddObject(window);
                    if (mgr->singleton_windows_.contains(type))
                    {
                        auto* old = mgr->singleton_windows_[type];
                        LOG_INFO_CATEGORY(Tool.Window, L"窗口 {} 已存在,执行替换", window->GetName());
                        Delete(old);
                    }
                    else
                    {
                        window->Construct();
                        LOG_INFO_CATEGORY(Tool.Window, L"创建窗口 {}", window->GetName());
                    }
                    mgr->singleton_windows_[type] = window;
                }
                else
                {
                    window->Construct();
                    mgr->duplicated_windows_.push_back(window);
                    LOG_INFO_CATEGORY(Tool.Window, L"创建窗口 {}", window->GetName());
                }
                return window;
            }
        }
    }
    LOG_ERROR_CATEGORY(Tool.Window, L"创建窗口失败");
    return nullptr;
}

void WindowManager::DestroyWindow(const Type& type)
{
    auto& mgr = *Get();
    if (mgr.singleton_windows_.contains(type))
    {
        ObjectManager::Get()->RemoveObject(mgr.singleton_windows_[type]->GetID());
        Delete(mgr.singleton_windows_[type]);
        mgr.singleton_windows_.erase(type);
    }
    else
    {
        LOG_WARNING_CATEGORY(UI, L"试图删除不存在{}类型的Window", StringUtils::FromAnsiStringView(type.get_name()));
    }
}

TArray<WindowBase*> WindowManager::GetWindows(const Type& type) const
{
    TArray<WindowBase*> rtn;
    for (auto* window: duplicated_windows_)
    {
        if (window->IsImplemented(type))
        {
            rtn.push_back(window);
        }
    }
    return rtn;
}

void WindowManager::DrawVisibleWindows(float delta_time) const
{
    for (const auto& visible_window: visible_windows_)
    {
        visible_window->Tick(delta_time);
    }
}

void WindowManager::AddVisibleWindow(WindowBase* window)
{
    if (ContainerUtils::ContainsValue(singleton_windows_, window) || ContainerUtils::Contains(duplicated_windows_, window))
    {
        visible_windows_.push_back(window);
    }
    else
    {
        LOG_ERROR_CATEGORY(UI, L"试图添加一个不存在的Window到可见窗口列表中");
    }
}

void WindowManager::RemoveVisibleWindow(WindowBase* window)
{
    if (ContainerUtils::Contains(visible_windows_, window))
    {
        ContainerUtils::Remove(visible_windows_, window);
    }
}

}
