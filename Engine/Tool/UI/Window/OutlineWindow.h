/**
 * @file OutlineWindow.h
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#pragma once
#include "WindowBase.h"

#include "OutlineWindow.generated.h"

namespace Function {
class GameObject;
}

WINDOW_NAMESPACE_BEGIN

// 世界大纲窗口
class REFL OutlineWindow : public WindowBase  {
    GENERATED_BODY(OutlineWindow)

public:
    OutlineWindow();
    void Draw(float InDeltaTime) override;

protected:
    void DrawGameObject(Function::GameObject* game_object);

public:
   int32_t selected_object_id = 0;
};

WINDOW_NAMESPACE_END
