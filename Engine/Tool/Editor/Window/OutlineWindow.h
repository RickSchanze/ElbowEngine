/**
 * @file OutlineWindow.h
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#pragma once
#include "WindowBase.h"

#include "Component/Script/Autorotation.h"
#include "OutlineWindow.generated.h"

namespace Function {
class GameObject;
}

namespace tool::window {

// 世界大纲窗口
ECLASS()
class OutlineWindow : public WindowBase  {
    GENERATED_BODY(OutlineWindow)

public:
    OutlineWindow();
    void Draw(float InDeltaTime) override;

protected:
    void DrawGameObject(function::GameObject* game_object);

public:
   int32_t selected_object_id = 0;
};

}
