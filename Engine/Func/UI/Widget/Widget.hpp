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

constexpr Float DEFAULT_FONT_SIZE = 20.f; // 所有字体和图标的默认大小

/**
 * Widget就是UI Element
 */
class Widget : public Object {
    friend class Widget;
    friend class UIManager;
    REFLECTED_CLASS(Widget)
protected:
    Rect2Df abs_rect_{}; // 绝对位置, 由Rebuild时相对位置计算而来
    Rect2Df rel_rect_{}; // 相对位置, 人为设置, 如果size中的分量为0, 表示对应轴的size自适应, 一般是和父Widget一致
    ObjectPtr<Widget> parent_ = nullptr; // 父Widget
    bool visible_ = true; // visible为false仅仅不发送draw call
    bool receive_input_ = false;
    bool enable_ = true; // enable为false时删除对应的vertex/index缓冲区
    ObjectPtr<Material> material_ = nullptr;

    bool rebuild_dirty_ = true;

    virtual void OnVisibleChanged(bool old, bool now);
    virtual void OnEnabledChanged(bool old, bool now);

public:
    Widget();

    // 供Rebuild使用, 在Rebuild之外使用会Assert
    void SetAbsoluteLocation(Vector2f loc);
    void SetAbsoluteSize(Vector2f size);

    virtual Vector2f GetRebuildRequiredSize() const { return {}; };
    virtual void Rebuild();
    virtual void Draw(rhi::CommandBuffer &cmd) {}

    void SetRebuildDirty(bool dirty = true);
    bool IsRebuildDirty() const { return rebuild_dirty_; }
    // 输入相关虚函数
    virtual void OnMouseMove(Vector2f old, Vector2f now) {}
    virtual void OnMouseScroll(Vector2f value) {}
    virtual void OnMouseButtonPressed(MouseButton button, Vector2f pos) {}
    virtual void OnMouseButtonReleased(MouseButton button, Vector2f pos) {}
    virtual void OnMouseEnter() {}
    virtual void OnMouseLeave() {}

    void SetReceiveInput(bool receive_input);
    bool IsReceiveInput() const { return receive_input_; }

    void SetVisible(bool visible);
    bool IsVisible() const { return visible_; }

    void SetEnable(bool enable);
    bool IsEnable() const { return enable_; }

    // 获取世界空间绝对坐标
    Vector2f GetAbsoluteLocation() const { return abs_rect_.pos; }
    Vector2f GetRelativeLocation() const { return rel_rect_.pos; }
    // 设置相对位置, 你只能设置相对位置
    void SetLocation(Vector2f loc);

    // 获取世界空间绝对尺寸
    Vector2f GetAbsoluteSize() const { return abs_rect_.size; }
    // 获取相对尺寸
    Vector2f GetRelativeSize() const { return rel_rect_.size; }
    // 设置相对尺寸, 你只能设置相对尺寸
    void SetSize(Vector2f size);

    Rect2Df GetAbsoluteRect() const { return abs_rect_; }
    Rect2Df GetRelativeRect() const { return rel_rect_; }

    Material *GetMaterial();
    void SetMaterial(Material *mat);

    void SetParent(Widget *w);
    Widget *GetParent() const { return parent_; }
};

REGISTER_TYPE(Widget)
