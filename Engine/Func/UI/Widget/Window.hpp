//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Object/ObjectPtr.hpp"
#include "Widget.hpp"


class Text;
class Window : public Widget {
    REFLECTED_CLASS(Window)
public:
    explicit Window();
    ~Window() override;

    void Rebuild() override;

protected:
    ObjectPtr<Text> title_text_;
    ObjectPtr<Widget> slot_;
};

REGISTER_TYPE(Window)
