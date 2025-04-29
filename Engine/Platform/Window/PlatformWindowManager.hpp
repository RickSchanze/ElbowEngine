//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Manager/Manager.hpp"

class PlatformWindow;
class PlatformWindowManager : public Manager<PlatformWindowManager> {
public:
    virtual Float GetLevel() const override { return 5; }
    virtual StringView GetName() const override
    {
        return "PlatformWindowManager";
    }

    virtual void Startup() override;
    virtual void Shutdown() override;

    void AddWindow(PlatformWindow *window);

    bool RemoveWindow(Int32 window_id);

    bool RemoveWindow(StringView window_title);

    PlatformWindow *_GetWindowByPtr(const void *ptr);

    static PlatformWindow *GetWindow(Int32 window_id) { return GetByRef().InternalGetWindow(window_id); }
    static PlatformWindow *GetWindow(StringView window_title) { return GetByRef().InternalGetWindow(window_title); }
    static PlatformWindow *GetMainWindow() { return GetWindow(0); }

    static void BeginImGuiFrame(Int32 window_id = 0);

protected:
    PlatformWindow *InternalGetWindow(Int32 window_id) const;

    PlatformWindow *InternalGetWindow(StringView window_title) const;

    PlatformWindow *InternalGetMainWindow() const { return InternalGetWindow(0); }

    static inline Int32 next_window_id_ = 0;

    Map<Int32, PlatformWindow *> windows_;
};

inline PlatformWindowManager &GetWindowManager() { return PlatformWindowManager::GetByRef(); }
