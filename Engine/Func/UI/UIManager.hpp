//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Manager/MManager.hpp"


class ViewportWindow;
class GlobalDockingWindow;
class ImGuiWindow;
class Window;
class UIManager : public Manager<UIManager> {
public:
    Float GetLevel() const override { return 14.1f; }
    StringView GetName() const override { return "UIManager"; }

    static void DrawAll();
    static void AddWindow(ImGuiWindow* w);
    static void RemoveWindow(ImGuiWindow* w);
    static ViewportWindow* GetActiveViewportWindow();
    static void ActivateViewportWindow();
    static bool HasActiveViewportWindow();

    void Startup() override;
    void Shutdown() override;

private:
    Map<StringView, ImGuiWindow *> windows_;
    GlobalDockingWindow* global_docking_window_ = nullptr;
    ViewportWindow* active_viewport_window_ = nullptr;
};
