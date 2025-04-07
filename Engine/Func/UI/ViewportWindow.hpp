//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "ImGuiWindow.hpp"


class ViewportWindow : public ImGuiWindow {
    REFLECTED_CLASS(ViewportWindow)
public:
    void Draw() override;
    Vector2f GetPosition() const { return pos_; }
    Vector2f GetSize() const { return size_; }

protected:
    Vector2f pos_;
    Vector2f size_;
};

REGISTER_TYPE(ViewportWindow)
