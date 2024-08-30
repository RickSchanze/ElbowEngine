/**
 * @file ConsoleWindow.h
 * @author Echo 
 * @Date 24-8-28
 * @brief 
 */

#pragma once
#include "ToolCommon.h"
#include "WindowBase.h"

#include "ConsoleWindow.generated.h"
#include "Editor/Widgets/Button.h"
#include "Log/LogRecorder.h"
#include "Math/MathTypes.h"

WINDOW_NAMESPACE_BEGIN

class REFL ConsoleWindow : public WindowBase
{
    GENERATED_BODY(ConsoleWindow)
public:
    ConsoleWindow();

    void Construct() override;


    void Draw(float delta_time) override;

    bool IsLevelLogSelected() const;
    bool IsLevelWarningSelected() const;
    bool IsLevelErrorSelected() const;

private:
    void DrawLogConsoleHeader();
    void DrawLogConsoleFooter();
    void DrawSingleLog(const Log& log, bool even, Vector2 size);

    PROPERTY(Serialized)
    int32_t single_log_height_ = 60;

    PROPERTY(Serialized)
    Color even_color_ = Color(0.2f, 0.2f, 0.2f, 0.5f);

    PROPERTY(Serialized)
    Color odd_color_ = Color(0.1f, 0.1f, 0.1f, 0.5f);

    int32_t selected_index_ = -1;

    int32_t info_count_;
    int32_t warning_count_;
    int32_t error_count_;

    // 0: 没选中
    // 1 << 0: 选中info
    // 1 << 1: 选中warning
    // 1 << 2: 选中error
    // 1 << 3: 全选
    int8_t selected_level_flags_ = 1 << 3;

    TList<Log>::const_iterator selected_log_;

    Widget::Button btn_;
    Widget::Button btn2_;
};

WINDOW_NAMESPACE_END
