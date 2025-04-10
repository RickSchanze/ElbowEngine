//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiDrawWindow.hpp"


class Actor;
class InspectorWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(InspectorWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "InspectorWindow"; }
    Actor *GetSelectedActor() const { return selected_actor_; }

private:
    Actor *selected_actor_ = nullptr;
};

REGISTER_TYPE(InspectorWindow)
