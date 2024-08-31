/**
 * @file ImGuiHelper.cpp
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#include "ImGuiHelper.h"

#include "CachedString.h"
#include "IconsMaterialDesign.h"
#include "Math/Math.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Texture.h"
#include "Window/WindowCommon.h"

#include <imgui_impl_vulkan.h>
#include <ranges>

void ImGuiHelper::Text(CachedString& str)
{
    ImGui::Text(str.ToCStyleString());
}

bool ImGuiHelper::CollapsingHeader(const char* label)
{
    return ImGui::CollapsingHeader(label);
}

void ImGuiHelper::Separator()
{
    ImGui::Separator();
}

void ImGuiHelper::Image(Resource::Texture* texture, int32_t width, int32_t height, int32_t max_width, int32_t max_height)
{
    if (texture == nullptr)
    {
        ErrorBox(U8("传入纹理为nullptr"));
        return;
    }
    if (!texture->IsValid())
    {
        CachedString errors = std::format(L"纹理 {} 无效", texture->GetPath().ToRelativeString());
        ErrorBox(errors.ToCStyleString());
        return;
    }
    Image(texture->GetTextureView(), texture->GetSampler(), width, height, max_width, max_height);
}

void ImGuiHelper::Image(Resource::TextureCube* texture_cube, int max_wdith, int max_height)
{
    if (texture_cube == nullptr)
    {
        ErrorBox(U8("立方体贴图为空!"));
        return;
    }
    if (!texture_cube->IsValid())
    {
        CachedString texture = std::format(L"立方体贴图 {} 失效", texture_cube->GetPath().ToRelativeString());
        ErrorBox(texture.ToCStyleString());
        return;
    }
    int32_t w      = texture_cube->GetWidth();
    int32_t h      = texture_cube->GetHeight();
    float   ratio  = static_cast<float>(w) / h;
    float   width  = ImGui::GetContentRegionAvail().x;
    float   height = width / ratio;
    Text(U8("右:"));
    Image(texture_cube->GetFaceView(0), texture_cube->GetSampler(), width, height, max_wdith, max_height);
    Text(U8("左:"));
    Image(texture_cube->GetFaceView(1), texture_cube->GetSampler(), width, height, max_wdith, max_height);
    Text(U8("上:"));
    Image(texture_cube->GetFaceView(2), texture_cube->GetSampler(), width, height, max_wdith, max_height);
    Text(U8("下:"));
    Image(texture_cube->GetFaceView(3), texture_cube->GetSampler(), width, height, max_wdith, max_height);
    Text(U8("前:"));
    Image(texture_cube->GetFaceView(4), texture_cube->GetSampler(), width, height, max_wdith, max_height);
    Text(U8("后:"));
    Image(texture_cube->GetFaceView(5), texture_cube->GetSampler(), width, height, max_wdith, max_height);
}

void ImGuiHelper::Image(RHI::Vulkan::ImageView* view, RHI::Vulkan::Sampler* sampler, int width, int height, int max_width, int max_height)
{
    if (view == nullptr)
    {
        return;
    }
    if (imgui_textuers_.empty())
    {
        RHI::Vulkan::VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&ImGuiHelper::RemoveAllImGuiTextures);
    }
    VkDescriptorSet set;
    if (imgui_textuers_.contains(view))
    {
        set = imgui_textuers_[view];
    }
    else
    {
        set                   = ImGui_ImplVulkan_AddTexture(sampler->GetHandle(), view->GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        imgui_textuers_[view] = set;
    }
    // 宽度超标了 不能再加了
    if (width > max_width)
    {
        float ratio = (width * 1.f) / height;
        width       = max_width;
        height      = static_cast<int32_t>(width / ratio);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
    }
    ImGui::Image(set, {static_cast<float>(width), static_cast<float>(height)});
}

void ImGuiHelper::Image(Resource::Texture* texture, int max_width, int max_height)
{
    float ratio = (float)texture->GetWidth() / (float)texture->GetHeight();
    float width = ImGui::GetContentRegionAvail().x;
    Image(texture, static_cast<int32_t>(width), static_cast<int32_t>(width / ratio), max_width, max_height);
}

void ImGuiHelper::SameLine(float offset_from_start_x, float spacing)
{
    ImGui::SameLine(offset_from_start_x, spacing);
}

void ImGuiHelper::SeparatorText(const char* label)
{
    ImGui::SeparatorText(label);
}

void ImGuiHelper::ShowDemoWindow()
{
    ImGui::ShowDemoWindow();
}

void ImGuiHelper::WarningBox(const char* text)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.1, 0.1, 0.1, 0.1});
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.f);
    ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)Color::Warning());
    auto window_width = ImGui::GetWindowWidth();
    auto size         = ImGui::CalcTextSize(text, nullptr, false, window_width);
    if (size.x < window_width)
    {
        size.x = window_width - 20;
    }
    size.y += ImGui::GetTextLineHeight() * 2;
    ImGui::BeginChild("##warning", size, ImGuiChildFlags_Border);

    PushFontScale(1.2f);
    TextColored(Color::Warning(), ICON_MD_WARNING);
    ImGui::SameLine();
    TextColored(Color::Warning(), U8("警告"));
    PopFontScale();

    TextWrapped(text);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void ImGuiHelper::ErrorBox(const char* text)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.1, 0.1, 0.1, 0.1});
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.f);
    ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)Color::Error());
    auto window_width = ImGui::GetWindowWidth();
    auto size         = ImGui::CalcTextSize(text, nullptr, false, window_width);
    if (size.x < window_width)
    {
        size.x = window_width - 20;
    }
    size.y += ImGui::GetTextLineHeight() * 2;
    ImGui::BeginChild("##error", size, ImGuiChildFlags_Border);

    PushFontScale(1.2f);
    TextColored(Color::Error(), ICON_MD_ERROR);
    ImGui::SameLine();
    TextColored(Color::Error(), U8("错误"));
    PopFontScale();

    TextWrapped(text);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

bool ImGuiHelper::IsItemClicked()
{
    return ImGui::IsItemClicked();
}

bool ImGuiHelper::IsLeftMouseDown()
{
    return ImGui::IsMouseDown(ImGuiMouseButton_Left);
}

bool ImGuiHelper::IsLeftMouseReleased()
{
    return ImGui::IsMouseReleased(ImGuiMouseButton_Left);
}

void ImGuiHelper::PushFontScale(float scale)
{
    old_font_scale_ = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= scale;
    ImGui::PushFont(ImGui::GetFont());
}

void ImGuiHelper::PopFontScale()
{
    ImGui::GetFont()->Scale = old_font_scale_;
    ImGui::PopFont();
}

void ImGuiHelper::ImageBackbuffer(int32_t width, int32_t height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }
    if (back_image_texture_.empty())
    {
        back_image_texture_.resize(g_engine_statistics.graphics.swapchain_image_count);
    }
    if (back_image_texture_[g_engine_statistics.current_image_index] == nullptr)
    {
        back_image_texture_[g_engine_statistics.current_image_index] = ImGui_ImplVulkan_AddTexture(
            RHI::Vulkan::Sampler::GetDefaultSampler().GetHandle(),
            RHI::Vulkan::VulkanContext::Get()->GetBackbufferView(g_engine_statistics.current_image_index)->GetHandle(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }
    float uv1x = (float)width / (float)RHI::Vulkan::VulkanContext::Get()->GetActualBackbufferWidth();
    float uv1y = (float)height / (float)RHI::Vulkan::VulkanContext::Get()->GetActualBackbufferHeight();
    ImGui::Image(
        back_image_texture_[g_engine_statistics.current_image_index], {static_cast<float>(width), static_cast<float>(height)}, {0, 0}, {uv1x, uv1y}
    );
}

void ImGuiHelper::ClearBackbufferDescriptorSets()
{
    for (auto& set: back_image_texture_)
    {
        ImGui_ImplVulkan_RemoveTexture(set);
        set = nullptr;
    }
}

void ImGuiHelper::BeginChild(const char* id, Vector2 size, int32_t child_flags, int32_t window_flags)
{
    ImGui::BeginChild(id, {size.x, size.y}, child_flags, window_flags);
}

void ImGuiHelper::EndChild()
{
    ImGui::EndChild();
}

Vector2 ImGuiHelper::GetContentRegionAvail()
{
    auto size = ImGui::GetContentRegionAvail();
    return {size.x, size.y};
}

void ImGuiHelper::PushID(int id)
{
    ImGui::PushID(id);
}

void ImGuiHelper::PopID()
{
    ImGui::PopID();
}

void ImGuiHelper::PushChildWindowColor(Color col)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)col);
}

void ImGuiHelper::PopChildWindowColor()
{
    ImGui::PopStyleColor();
}

void ImGuiHelper::PushTextColor(Color col)
{
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)col);
}

void ImGuiHelper::PushTextBackgroundColor(Color col)
{
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, (ImVec4)col);
}

void ImGuiHelper::PopColor()
{
    ImGui::PopStyleColor();
}

float ImGuiHelper::GetFontSize()
{
    return ImGui::GetFontSize();
}

void ImGuiHelper::SetCursorPosY(float y)
{
    ImGui::SetCursorPosY(y);
}

void ImGuiHelper::SetCursorPosX(float x)
{
    ImGui::SetCursorPosY(x);
}

float ImGuiHelper::GetCursorPosX()
{
    return ImGui::GetCursorPosX();
}

void ImGuiHelper::SetCursorScreenPos(float x, float y)
{
    ImGui::SetCursorScreenPos({x, y});
}

void ImGuiHelper::BeginGroup()
{
    ImGui::BeginGroup();
}

void ImGuiHelper::EndGroup()
{
    ImGui::EndGroup();
}

bool ImGuiHelper::Button(const char* label, Vector2 size)
{
    return ImGui::Button(label, {size.x, size.y});
}

void ImGuiHelper::SetItemTooltip(const AnsiString& tooltip)
{
    ImGui::SetItemTooltip(tooltip.c_str());
}

Vector2 ImGuiHelper::CalcTextSize(const AnsiString& str)
{
    return CalcTextSize(str.c_str());
}

Vector2 ImGuiHelper::CalcTextSize(const char* str)
{
    ImVec2 vec     = ImGui::CalcTextSize(str);
    auto   padding = GetFramePadding();
    return Vector2{vec.x, vec.y} + Math::Multiply(padding, 2);
}

Vector2 ImGuiHelper::GetFramePadding()
{
    auto& style = ImGui::GetStyle();
    return {style.FramePadding.x, style.FramePadding.y};
}

void ImGuiHelper::RemoveAllImGuiTextures()
{
    for (auto set: imgui_textuers_ | std::views::values)
    {
        ImGui_ImplVulkan_RemoveTexture(set);
    }
    imgui_textuers_.clear();
}

Vector2 ImGuiHelper::GetCursorScreenPos()
{
    auto pos = ImGui::GetCursorScreenPos();
    return {pos.x, pos.y};
}

void ImGuiHelper::DrawRectFilled(Vector2 min, Vector2 max, Color color, float rounding)
{
    ImGui::GetWindowDrawList()->AddRectFilled(min, max, static_cast<ImU32>(color), rounding);
}

bool ImGuiHelper::IsItemHovered()
{
    return ImGui::IsItemHovered();
}

bool ImGuiHelper::IsItemActive()
{
    return ImGui::IsItemActive();
}
Color ImGuiHelper::GetButtonNormalColor()
{
    return ImGui::GetStyle().Colors[ImGuiCol_Button];
}

Color ImGuiHelper::GetButtonHoveredColor()
{
    return ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
}

Color ImGuiHelper::GetButtonPressedColor()
{
    return ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
}

Color ImGuiHelper::GetWindowBackgroundColor()
{
    return ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
}
