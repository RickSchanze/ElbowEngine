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
class UIManager : public Manager<UIManager> {
public:
    Float GetLevel() const override { return 14.1f; }
    StringView GetName() const override { return "UIManager"; }

    static void DrawAll();
    static void AddWindow(ImGuiDrawWindow *w);
    static void RemoveWindow(ImGuiDrawWindow *w);
    static ViewportWindow *GetActiveViewportWindow();
    static void ActivateViewportWindow();
    static bool HasActiveViewportWindow();
    static ImGuiDrawWindow *CreateOrActivateWindow(const Type *t);
    static ImGuiDrawWindow *GetWindow(const Type *t);
    template<typename T>
    static T *GetWindow() {
        return static_cast<T *>(GetWindow(T::GetStaticType()));
    }

    template<typename T>
        requires IsBaseOf<ImGuiDrawWindow, T>
    static T *CreateOrActivateWindow() {
        auto &self = GetByRef();
        if constexpr (SameAs<T, ViewportWindow>) {
            if (self.active_viewport_window_) {
                self.active_viewport_window_->SetVisible(true);
            } else {
                self.active_viewport_window_ = CreateNewObject<ViewportWindow>();
            }
            return self.active_viewport_window_;
        } else {
            const Type *window_type = T::GetStaticType();
            for (auto &pair: self.windows_) {
                auto *w = pair.second;
                if (w->GetType() == window_type) {
                    w->SetVisible(true);
                    return static_cast<T *>(w);
                }
            }
            T *rtn = static_cast<T *>(CreateNewObject<T>());
            AddWindow(rtn);
            return rtn;
        }
    }

    void Startup() override;
    void Shutdown() override;

private:
    Map<StringView, ImGuiDrawWindow *> windows_;
    GlobalDockingWindow *global_docking_window_ = nullptr;
    ViewportWindow *active_viewport_window_ = nullptr;
};
