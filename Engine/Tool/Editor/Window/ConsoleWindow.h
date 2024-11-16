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
#include "Core/Math/MathTypes.h"

namespace tool::widget
{
class ToggleButton;
}

namespace tool::window {

ECLASS()
class ConsoleWindow : public WindowBase
{
    GENERATED_CLASS(ConsoleWindow)
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
    void FilterLogsByLevel(const List<Log, std::allocator<Log>>& logs);

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

    List<Log>::const_iterator selected_log_;

    Array<List<Log>::const_iterator> filtered_logs_;
    int32_t                            filtered_logs_size_ = 0;

    UniquePtr<widget::ToggleButton> button_filter_info_;
    UniquePtr<widget::ToggleButton> button_filter_warning_;
    UniquePtr<widget::ToggleButton> button_filter_error_;
    UniquePtr<widget::Button>       button_filter_clear_;
};

}
