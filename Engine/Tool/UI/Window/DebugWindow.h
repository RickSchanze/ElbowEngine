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

class REFL StatisticsWindow : public WindowBase {
    GENERATED_BODY(StatisticsWindow)

public:
    StatisticsWindow();
    void Draw(float InDeltaTime) override;

protected:
    Float mFpsRefreshTime = 0;
    Int32 mRecordedFps = 0;
    Float mRecordedDeltaTime = 0.f;
};


WINDOW_NAMESPACE_END

