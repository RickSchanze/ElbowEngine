/**
 * @file DebugWindow.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "ToolCommon.h"
#include "WindowBase.h"

#include "DebugWindow.generated.h"

WINDOW_NAMESPACE_BEGIN

class REFL DebugWindow : public WindowBase {
    GENERATED_BODY(DebugWindow)

public:
    DebugWindow();
    void Draw(float InDeltaTime) override;
};


WINDOW_NAMESPACE_END

