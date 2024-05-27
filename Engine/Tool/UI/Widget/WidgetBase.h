/**
 * @file WidgetBase.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "ToolCommon.h"

WIDGET_NAMESPACE_BEGIN

// 所有Widget的基类 没有名字
class WidgetBase {
public:
    // 绘制函数，自动包裹在ImGui::Begin和ImGui::End之间
    virtual void Draw(float InDeltaTime) = 0;

    void MarkDirty();

protected:
    bool mbDirty = false;
};

WIDGET_NAMESPACE_END
