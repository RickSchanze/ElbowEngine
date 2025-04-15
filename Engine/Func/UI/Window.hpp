//
// Created by Echo on 2025/4/7.
//

#pragma once
#include <imgui.h>


#include "Core/Math/Types.hpp"
#include "Core/Object/Object.hpp"


struct ImVec2;
class Window : public Object {
    REFLECTED_CLASS(Window)
public:
    virtual StringView GetWindowIdentity();
    bool IsVisible() const { return mVisible; }
    void SetVisible(bool visible) { mVisible = visible; }

protected:
    bool mVisible = true;
};

REGISTER_TYPE(Window)

// 一些通用扩展
namespace impl {
    struct Ext_ToVector2f {};
} // namespace impl
inline constexpr impl::Ext_ToVector2f ToVector2f;
inline Vector2f operator|(const ImVec2 &vec, impl::Ext_ToVector2f) { return {vec.x, vec.y}; }
