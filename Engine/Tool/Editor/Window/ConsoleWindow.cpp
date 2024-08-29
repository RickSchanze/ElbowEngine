/**
 * @file ConsoleWindow.cpp
 * @author Echo 
 * @Date 24-8-28
 * @brief 
 */

#include "ConsoleWindow.h"

#include "IconsMaterialDesign.h"
#include "ImGui/ImGuiHelper.h"
#include "Log/LogRecorder.h"
#include "Utils/TimeUtils.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

ConsoleWindow::ConsoleWindow()
{
    name_        = L"Window_Console";
    window_name_ = L"控制台";
}

void ConsoleWindow::Draw(float delta_time)
{
    const auto& logs = g_log_recorder.GetLogs();

    DrawLogConsoleHeader();
    int     i    = 0;
    Vector2 size = {ImGuiHelper::GetContentRegionAvail().x, single_log_height_};
    ImGuiHelper::BeginChild("logs", Vector2{}, EImGuiCF_ResizeY);
    ;
    for (auto log_it = logs.begin(); log_it != logs.end(); ++log_it)
    {
        ImGuiHelper::PushID(10000 + i);
        DrawSingleLog(*log_it, i % 2 == 0, size);
        if (ImGuiHelper::IsItemClicked())
        {
            selected_index_ = i;
            selected_log_   = log_it;
        }
        ImGuiHelper::PopID();
        i++;
    }
    ImGuiHelper::EndChild();
    DrawLogConsoleFooter();
}

void ConsoleWindow::DrawLogConsoleHeader()
{
    ImGuiHelper::BeginChild("log_header", {0, 0}, EImGuiCF_AutoResizeY);

    {
        ImGui::PushStyleColor(ImGuiCol_Text, Color::Error());
        AnsiString error_count_str = ICON_MD_ERROR_OUTLINE + std::to_string(error_count_);
        if (ImGui::Button(error_count_str.c_str()))
        {
        }
        ImGui::PopStyleColor();
    }

    ImGui::SameLine(0, 10);

    {
        ImGui::PushStyleColor(ImGuiCol_Text, Color::Warning());
        AnsiString warning_count_str = ICON_MD_WARNING_AMBER + std::to_string(warning_count_);
        if (ImGui::Button(warning_count_str.c_str()))
        {
        }
        ImGui::PopStyleColor();
    }

    ImGui::SameLine(0, 10);

    {
        ImGui::PushStyleColor(ImGuiCol_Text, Color::Info());
        AnsiString info_count_str = ICON_MD_INFO_OUTLINE + std::to_string(info_count_);
        if (ImGui::Button(info_count_str.c_str()))
        {
        }
        ImGui::PopStyleColor();
    }

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
    if (even)
    {
        ImGuiHelper::PushChildWindowColor(even_color_);
    }
    else
    {
        ImGuiHelper::PushChildWindowColor(odd_color_);
    }
    ImGuiHelper::BeginChild("##log", size, 0);
    ImGuiHelper::PushFontScale(2.5f);
    auto font_size = ImGuiHelper::GetFontSize();
    ImGuiHelper::SetCursorPosY((size.y - font_size));
    // clang-format off
    switch (log.level)
    {
    case ELogLevel::Trace:
    case ELogLevel::Debug:
    case ELogLevel::Info:
        info_count_++;
        ImGuiHelper::TextColored(Color::White(), ICON_MD_INFO_OUTLINE);
        break;
    case ELogLevel::Warning:
        warning_count_++;
        ImGuiHelper::TextColored(Color::Warning(), ICON_MD_WARNING_AMBER);
        break;
    case ELogLevel::Error:
    case ELogLevel::Critical:
        error_count_++;
        ImGuiHelper::TextColored(Color::Error(), ICON_MD_ERROR_OUTLINE);
        break;
    case ELogLevel::MaxDefault:
        break;
    }
    ImGuiHelper::PopFontScale();
    ImGuiHelper::SameLine(0, 3);
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
