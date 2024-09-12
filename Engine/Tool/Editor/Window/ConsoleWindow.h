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

namespace Tool::Widgets
{
class ToggleButton;
}

WINDOW_NAMESPACE_BEGIN

ECLASS()
class ConsoleWindow : public WindowBase
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
    void FilterLogsByLevel(const TList<Log, std::allocator<Log>>& logs);

    EPROPERTY()
    int32_t single_log_height_ = 60;

    EPROPERTY()
    Color even_color_;

    EPROPERTY()
    Color odd_color_;

    EPROPERTY()
    Color selected_color_;

    int32_t selected_index_ = -1;

    int32_t info_count_;
    int32_t warning_count_;
    int32_t error_count_;

    // 0: 没选中
    // 1 << 0: 选中info
    // 1 << 1: 选中warning
    // 1 << 2: 选中error
    int8_t selected_level_flags_ = 1 << 0 | 1 << 1 | 1 << 2;

    TList<Log>::const_iterator selected_log_;

    TArray<TList<Log>::const_iterator> filtered_logs_;
    int32_t                            filtered_logs_size_ = 0;

    TUniquePtr<Widgets::ToggleButton> button_filter_info_;
    TUniquePtr<Widgets::ToggleButton> button_filter_warning_;
    TUniquePtr<Widgets::ToggleButton> button_filter_error_;
    TUniquePtr<Widgets::Button>       button_filter_clear_;
};

WINDOW_NAMESPACE_END
