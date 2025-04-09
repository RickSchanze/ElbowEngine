//
// Created by Echo on 2025/4/9.
//

#pragma once
#include "Func/UI/ImGuiDrawWindow.hpp"


namespace rhi {
    class ImageView;
}
class TestFunctionWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(TestFunctionWindow)

public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "TestFunctionWindow"; }
};

REGISTER_TYPE(TestFunctionWindow)
