/**
 * @file DebugWindow.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "ToolCommon.h"
#include "WindowBase.h"

WINDOW_NAMESPACE_BEGIN

class DebugWindow : public WindowBase {
public:
    void Construct() override;
    void Draw(float InDeltaTime) override;
private:

};

WINDOW_NAMESPACE_END
