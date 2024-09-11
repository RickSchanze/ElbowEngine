/**
 * @file DetailWindow.h
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#pragma once
#include "ToolCommon.h"
#include "WindowBase.h"

#include "DetailWindow.generated.h"

namespace Function
{
class GameObject;
namespace Comp
{
class Component;
}
}   // namespace Function

namespace Tool::Window
{
class OutlineWindow;
}

WINDOW_NAMESPACE_BEGIN

ECLASS()
class DetailWindow : public WindowBase
{
    GENERATED_BODY(DetailWindow)

public:
    DetailWindow();

    void Draw(float delta_time) override;

protected:
    void DrawSelectedObject(Function::GameObject* game_object);
    void DrawComponent(Function::Comp::Component* comp);

    OutlineWindow* outline_window_     = nullptr;
    int32_t        selected_object_id_ = -1;
};

WINDOW_NAMESPACE_END
