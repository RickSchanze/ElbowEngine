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

namespace tool::window {

ECLASS()
class StatisticsWindow : public WindowBase {
    GENERATED_BODY(StatisticsWindow)

public:
    StatisticsWindow();
    void Draw(float InDeltaTime) override;

protected:
    float fps_refresh_time_ = 0;
    int32_t recorded_fps_ = 0;
    float recorded_delta_time_ = 0.f;
};


}

