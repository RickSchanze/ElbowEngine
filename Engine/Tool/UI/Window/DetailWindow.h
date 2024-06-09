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

namespace Function {
class Component;
}
namespace Function {
class GameObject;
}
namespace Tool::Window {
class OutlineWindow;
}
WINDOW_NAMESPACE_BEGIN

class REFL DetailWindow : public WindowBase {
    GENERATED_BODY(DetailWindow)

public:
    DetailWindow();
    void Draw(float InDeltaTime) override;

protected:
    void DrawSelectedObject(Function::GameObject* InGameObject);
    void DrawComponent(Function::Component* InComp);

    OutlineWindow* mOutlineWindow = nullptr;
    UInt32 mSelectedObjectID = -1;
};

WINDOW_NAMESPACE_END
