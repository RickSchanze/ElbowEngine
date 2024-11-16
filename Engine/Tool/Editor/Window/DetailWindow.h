/**
 * @file DetailWindow.h
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#pragma once
#include "WindowBase.h"

#include "DetailWindow.generated.h"
#include "Render/Materials/Material.h"

namespace function
{
class GameObject;
namespace comp
{
class Component;
}
}   // namespace Function

namespace tool::window
{
class OutlineWindow;

ECLASS()
class DetailWindow : public WindowBase
{
    GENERATED_CLASS(DetailWindow)

public:
    DetailWindow();

    void Draw(float delta_time) override;

protected:
    void DrawSelectedObject(function::GameObject* game_object);
    void DrawComponent(function::comp::Component* comp);

    OutlineWindow* outline_window_     = nullptr;
    int32_t        selected_object_id_ = -1;
};

}
