/**
 * @file ConsoleWindow.cpp
 * @author Echo 
 * @Date 24-8-28
 * @brief 
 */

#include "ConsoleWindow.h"

#include "Editor/Widgets/ToggleButton.h"
#include "IconsMaterialDesign.h"
#include "ImGui/ImGuiHelper.h"
#include "Log/LogRecorder.h"
#include "Utils/TimeUtils.h"
#include "Window/WindowCommon.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

ConsoleWindow::ConsoleWindow()
{
    name_        = L"Window_Console";
    window_name_ = L"控制台";
}

void ConsoleWindow::Construct()
{
    Super::Construct();
    // clang-format off
    button_filter_error_ = MakeUnique<Widgets::ToggleButton>(true);
    button_filter_error_->SetEventOnToggleOn([this] { selected_level_flags_ |= 1 << 2; })
        .SetEventOnToggleOff([this] { selected_level_flags_ ^= 1 << 2; })
        .SetTooltipText(L"筛选等级大于等于Error的日志");

    button_filter_warning_ = MakeUnique<Widgets::ToggleButton>(true);
    button_filter_warning_->SetEventOnToggleOn([this] { selected_level_flags_ |= 1 << 1; })
        .SetEventOnToggleOff([this] { selected_level_flags_ ^= 1 << 1; })
        .SetTooltipText(L"筛选等级等于Warning的日志");

    button_filter_info_ = MakeUnique<Widgets::ToggleButton>(true);
    button_filter_info_->SetEventOnToggleOn([this] { selected_level_flags_ |= 1; })
        .SetEventOnToggleOff([this] { selected_level_flags_ ^= 1; })
        .SetTooltipText(L"筛选等级等于Info的日志");
    // clang-format on

    button_filter_clear_ = MakeUnique<Widgets::Button>();
    button_filter_clear_->SetTooltipText(L"清空日志").SetText(ICON_MD_CLEAR).SetEventOnClick([this] {
        g_log_recorder.Clear();
        selected_index_ = -1;
    });

    even_color_     = ImGuiHelper::GetWindowBackgroundColor();
    odd_color_      = even_color_ * 1.5f;
    selected_color_ = {0.1, 0.2, 0.3};
}

static TArray<TList<Log>::const_iterator> FilterLogByLevel(const TList<Log>& logs, int level_flags)
{
    TArray<TList<Log>::const_iterator> filtered_logs;
    auto                               log_it = logs.begin();
    for (; log_it != logs.end(); ++log_it)
    {
        auto log = *log_it;
        if (log.level <= ELogLevel::Info && (level_flags & 1 << 0))
        {
            filtered_logs.push_back(log_it);
        }
        if (log.level == ELogLevel::Warning && (level_flags & 1 << 1))
        {
            filtered_logs.push_back(log_it);
        }
        if (log.level >= ELogLevel::Error && (level_flags & 1 << 2))
        {
            filtered_logs.push_back(log_it);
        }
    }
    return filtered_logs;
}

void ConsoleWindow::Draw(float delta_time)
{
    DrawLogConsoleHeader();
    const auto& logs = g_log_recorder.GetLogs();

    // 计算各级别log数量
    for (auto& log: logs)
    {
        if (log.level <= ELogLevel::Info)
        {
            info_count_++;
        }
        else if (log.level == ELogLevel::Warning)
        {
            warning_count_++;
        }
        else
        {
            error_count_++;
        }
    }

    auto filter_logs = FilterLogByLevel(logs, selected_level_flags_);

    int     i    = 0;
    Vector2 size = {ImGuiHelper::GetContentRegionAvail().x, WINDOW_SCALE(single_log_height_)};

    ImGuiHelper::BeginChild("logs", Vector2{}, EImGuiCF_ResizeY);
    for (auto log_it = filter_logs.begin(); log_it != filter_logs.end(); ++log_it)
    {
        ImGuiHelper::PushID(10000 + i);
        DrawSingleLog(**log_it, i % 2 == 0, size);
        if (ImGuiHelper::IsItemClicked())
        {
            selected_index_ = (*log_it)->index;
            selected_log_   = *log_it;
        }
        ImGuiHelper::PopID();
        i++;
    }
    ImGuiHelper::EndChild();

    DrawLogConsoleFooter();
}

bool ConsoleWindow::IsLevelLogSelected() const
{
    return (selected_level_flags_ & 1 << 0) != 0;
}

bool ConsoleWindow::IsLevelWarningSelected() const
{
    return (selected_level_flags_ & 1 << 1) != 0;
}

bool ConsoleWindow::IsLevelErrorSelected() const
{
    return (selected_level_flags_ & 1 << 2) != 0;
}

void ConsoleWindow::DrawLogConsoleHeader()
{
    ImGuiHelper::BeginChild("log_header", {}, EImGuiCF_AutoResizeY);

    AnsiString info_count_str = ICON_MD_INFO_OUTLINE + std::to_string(info_count_);
    button_filter_info_->SetText(info_count_str).SetTextColor(Color::Info());
    button_filter_info_->Draw();

    ImGuiHelper::SameLine(0, WINDOW_SCALE(10));

    AnsiString warning_count_str = ICON_MD_WARNING_AMBER + std::to_string(warning_count_);
    button_filter_warning_->SetText(warning_count_str).SetTextColor(Color::Warning());
    button_filter_warning_->Draw();

    ImGuiHelper::SameLine(0, WINDOW_SCALE(10));

    AnsiString error_count_str = ICON_MD_ERROR_OUTLINE + std::to_string(error_count_);
    button_filter_error_->SetText(error_count_str).SetTextColor(Color::Error());
    button_filter_error_->Draw();

    ImGuiHelper::SameLine(0, WINDOW_SCALE(10));

    button_filter_clear_->Draw();

    ImGuiHelper::EndChild();

    warning_count_ = 0;
    error_count_   = 0;
    info_count_    = 0;
}

void ConsoleWindow::DrawLogConsoleFooter()
{
    ImGuiHelper::BeginChild("log_detail", {0, 0});
    if (selected_index_ == -1)
    {
        ImGuiHelper::Text("选择一条日志以查看它的详细信息");
    }
    else
    {
        const Log& log = *selected_log_;
        ImGuiHelper::TextColored(Color::Yellow(), "详细信息: ");
        auto time_str = TimeUtils::ToAnsiString(log.time);
        ImGuiHelper::Text(U8("线程id: %d"), log.thread_id);
        ImGuiHelper::Text(U8("时间: %s"), time_str.c_str());
        ImGuiHelper::TextColored(Color::Yellow(), "消息: ");
        ImGuiHelper::TextWrapped(log.message.c_str());
        ImGuiHelper::TextColored(Color::Yellow(), U8("调用栈: "));
        int i = 1;
        for (auto& stack: log.call_stack)
        {
            ImGuiHelper::TextColored(Color::SkyBlue(), "#%d in  %s  at  %s:%d", i++, stack.function.c_str(), stack.file.c_str(), stack.line);
        }
    }
    ImGuiHelper::EndChild();
}

void ConsoleWindow::DrawSingleLog(const Log& log, bool even, Vector2 size)
{
    if (log.index == selected_index_)
    {
        ImGuiHelper::PushChildWindowColor(selected_color_);
    }
    else
    {
        if (even)
        {
            ImGuiHelper::PushChildWindowColor(even_color_);
        }
        else
        {
            ImGuiHelper::PushChildWindowColor(odd_color_);
        }
    }
    ImGuiHelper::BeginChild("##log", size, 0);
    ImGuiHelper::PushFontScale(2.5f);
    auto font_size = ImGuiHelper::GetFontSize();
    // clang-format off
    switch (log.level)
    {
    case ELogLevel::Trace:
    case ELogLevel::Debug:
    case ELogLevel::Info:
        ImGuiHelper::TextColored(Color::White(), ICON_MD_INFO_OUTLINE);
        break;
    case ELogLevel::Warning:
        ImGuiHelper::TextColored(Color::Warning(), ICON_MD_WARNING_AMBER);
        break;
    case ELogLevel::Error:
    case ELogLevel::Critical:
        ImGuiHelper::TextColored(Color::Error(), ICON_MD_ERROR_OUTLINE);
        break;
    case ELogLevel::MaxDefault:
        break;
    }
    ImGuiHelper::PopFontScale();
    ImGuiHelper::SameLine(0, 3);
    ImGuiHelper::SetCursorPosY((size.y - font_size));
    ImGuiHelper::BeginGroup();
    ImGuiHelper::Text(log.message.c_str());
    ImGuiHelper::PushFontScale(0.8f);
    ImGuiHelper::TextColored(Color::Green(), U8("线程id: %d"), log.thread_id);
    ImGuiHelper::SameLine(0, 30);
    auto time_str = TimeUtils::ToAnsiString(log.time);
    ImGuiHelper::TextColored(Color::Green(), U8("时间: %s"), time_str.c_str());
    ImGuiHelper::PopFontScale();
    ImGuiHelper::EndGroup();
    ImGuiHelper::EndChild();
    ImGuiHelper::PopChildWindowColor();
}

WINDOW_NAMESPACE_END
