//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Event/Event.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Platform/Window/PlatformWindow.hpp"

namespace rhi {
    class CommandBuffer;
}
class Material;
/**
 * Widget就是UI Element
 */
class Widget : public Object {
    friend class UIManager;
    REFLECTED_CLASS(Widget)
protected:
    Rect2Df ui_rect_{}; // 这里记录的pos是左下角
    bool visible_ = true; // visible为false仅仅不发送draw call
    bool receive_input_ = false;
    bool enable_ = true; // enable为false时删除对应的vertex/index缓冲区
    ObjectPtr<Material> material_ = nullptr;

    bool rebuild_dirty_ = true;

    virtual void OnVisibleChanged(bool old, bool now);
    virtual void OnEnabledChanged(bool old, bool now);

    void SetRebuildDirty(bool dirty = true);
    bool IsRebuildDirty() const { return rebuild_dirty_; }


public:
    virtual Vector2f GetRebuildRequiredSize() { return {}; };
    Rect2Df GetUIRect() const { return ui_rect_; }
    virtual void Rebuild();
    virtual void Draw(rhi::CommandBuffer &cmd) {}


    // 输入相关虚函数
    virtual void OnMouseMove(Vector2f old, Vector2f now) {}
    virtual void OnMouseScroll(Vector2f value) {}
    virtual void OnMouseButtonDown(MouseButton button, Vector2f pos) {}
    virtual void OnMouseButtonUp(MouseButton button, Vector2f pos) {}

    void SetReceiveInput(bool receive_input);
    bool IsReceiveInput() const { return receive_input_; }

    void SetVisible(bool visible);
    bool IsVisible() const { return visible_; }

    void SetEnable(bool enable);
    bool IsEnable() const { return enable_; }

    void SetLocation(Vector2f loc);
    void SetSize(Vector2f size);

    Material *GetMaterial();
    void SetMaterial(Material *mat);
};

REGISTER_TYPE(Widget)
