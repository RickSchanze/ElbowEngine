//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiDrawWindow.hpp"

#include GEN_HEADER("InspectorWindow.generated.hpp")


class Actor;
class ECLASS() InspectorWindow : public ImGuiDrawWindow {
    GENERATED_BODY(InspectorWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "InspectorWindow"; }
    Actor *GetSelectedActor() const { return selected_actor_; }

private:
    Actor *selected_actor_ = nullptr;
};
