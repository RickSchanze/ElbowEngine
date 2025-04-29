//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/TypeTraits.hpp"
#include "ImGuiDrawWindow.hpp"
#include "ViewportWindow.hpp"

struct Type;
class ViewportWindow;
class GlobalDockingWindow;
class ImGuiDrawWindow;
class Window;
class UIManager : public Manager<UIManager>
{
public:
    virtual Float GetLevel() const override
    {
        return 14.1f;
    }
    virtual StringView GetName() const override
    {
        return "UIManager";
    }

    static void DrawAll();
    static void AddWindow(ImGuiDrawWindow* w);
    static void RemoveWindow(ImGuiDrawWindow* w);
    static ViewportWindow* GetActiveViewportWindow();
    static void ActivateViewportWindow();
    static bool HasActiveViewportWindow();
    // 这里可见默认为true
    static ImGuiDrawWindow* CreateOrActivateWindow(const Type* t);
    /**
     * @param t
     * @param silent 创建窗口时是否可见?
     * @return
     */
    static ImGuiDrawWindow* CreateOrActivateWindow(const Type* t, bool silent);
    static ImGuiDrawWindow* GetWindow(const Type* t);
    template <typename T>
    static T* GetWindow()
    {
        return static_cast<T*>(GetWindow(T::GetStaticType()));
    }

    template <typename T>
        requires Traits::IsBaseOf<ImGuiDrawWindow, T>
    static T* CreateOrActivateWindow(bool silent = false)
    {
        auto& self = GetByRef();
        if constexpr (Traits::SameAs<T, ViewportWindow>)
        {
            if (self.active_viewport_window_)
            {
                self.active_viewport_window_->SetVisible(true);
            }
            else
            {
                self.active_viewport_window_ = CreateNewObject<ViewportWindow>();
            }
            return self.active_viewport_window_;
        }
        else
        {
            const Type* window_type = T::GetStaticType();
            for (auto& Window : self.windows_ | std::views::values)
            {
                if (Window->GetType() == window_type)
                {
                    if (!silent)
                    {
                        Window->SetVisible(true);
                    }
                    return static_cast<T*>(Window);
                }
            }
            T* rtn = static_cast<T*>(CreateNewObject<T>());
            if (silent)
                rtn->SetVisible(false);
            AddWindow(rtn);
            return rtn;
        }
    }

    virtual void Startup() override;
    virtual void Shutdown() override;

private:
    Map<StringView, ImGuiDrawWindow*> windows_;
    GlobalDockingWindow* global_docking_window_ = nullptr;
    ViewportWindow* active_viewport_window_ = nullptr;
};
