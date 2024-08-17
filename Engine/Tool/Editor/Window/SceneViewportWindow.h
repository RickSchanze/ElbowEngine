/**
 * @file SceneViewportWindow.h
 * @author Echo 
 * @Date 24-8-17
 * @brief 
 */

#pragma once
#include "ToolCommon.h"
#include "ViewportWindow.h"

#include "SceneViewportWindow.generated.h"

WINDOW_NAMESPACE_BEGIN

class REFL SceneViewportWindow : public ViewportWindow
{
    GENERATED_BODY(SceneViewportWindow)
public:
    SceneViewportWindow();

    void Draw(float delta_time) override;



private:
    int old_w_;
    int old_h_;
};

WINDOW_NAMESPACE_END
