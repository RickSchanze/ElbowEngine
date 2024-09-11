/**
 * @file LightSettingWindow.h
 * @author Echo 
 * @Date 24-8-25
 * @brief 
 */

#pragma once
#include "ToolCommon.h"
#include "WindowBase.h"

#include "LightSettingWindow.generated.h"

WINDOW_NAMESPACE_BEGIN

ECLASS()
class LightSettingWindow : public WindowBase
{
    GENERATED_BODY(LightSettingWindow)
public:
    LightSettingWindow();
    void Draw(float delta_time) override;
};

WINDOW_NAMESPACE_END
