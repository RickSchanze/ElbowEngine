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
#include "Log/LogRecorder.h"
#include "Math/MathTypes.h"

WINDOW_NAMESPACE_BEGIN

class REFL ConsoleWindow : public WindowBase
{
    GENERATED_BODY(ConsoleWindow)
public:
    ConsoleWindow();


    void Draw(float delta_time) override;

private:
    void DrawLogConsoleHeader();
    void DrawLogConsoleFooter();
    void DrawSingleLog(const Log& log, bool even, Vector2 size);

    PROPERTY(Serialized)
    int32_t single_log_height_ = 80;

    PROPERTY(Serialized)
    Color even_color_ = Color(0.2f, 0.2f, 0.2f, 0.5f);

    PROPERTY(Serialized)
    Color odd_color_ = Color(0.1f, 0.1f, 0.1f, 0.5f);

    int32_t selected_index_ = -1;

    int32_t info_count_;
    int32_t warning_count_;
    int32_t error_count_;

    TList<Log>::const_iterator selected_log_;
};

WINDOW_NAMESPACE_END
