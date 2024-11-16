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

namespace tool::window {

ECLASS()
class LightSettingWindow : public WindowBase
{
    GENERATED_CLASS(LightSettingWindow)
public:
    LightSettingWindow();
    void Draw(float delta_time) override;
};

}
