/**
 * @file ImGuiHelper.cpp
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#include "ImGuiHelper.h"

#ifdef USE_IMGUI
#include "CachedString.h"
#include "IconsMaterialDesign.h"
#include "Math/Math.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Texture.h"
#include <imgui_impl_vulkan.h>
#include <ranges>
#endif

void ImGuiHelper::Text(CachedString& str)
{
#ifdef USE_IMGUI
    ImGui::Text(str.ToCStyleString());
#endif
}

bool ImGuiHelper::CollapsingHeader(const char* label)
{
#ifdef USE_IMGUI
    return ImGui::CollapsingHeader(label);
#else
    return true;
#endif
}

void ImGuiHelper::Separator()
{
#ifdef USE_IMGUI
    ImGui::Separator();
#endif
}

void ImGuiHelper::Image(resource::Texture* texture, int32_t width, int32_t height, int32_t max_width, int32_t max_height)
{
#ifdef USE_IMGUI
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
#endif
}

void ImGuiHelper::Image(resource::TextureCube* texture_cube, int max_wdith, int max_height)
{
#ifdef USE_IMGUI
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
#endif
}

void ImGuiHelper::Image(rhi::vulkan::ImageView* view, rhi::vulkan::Sampler* sampler, int width, int height, int max_width, int max_height)
{
#ifdef USE_IMGUI
    if (view == nullptr)
    {
        return;
    }
    if (imgui_textuers_.empty())
    {
        rhi::vulkan::VulkanContext::Get()->OnPreVulkanDeviceDestroyed.AddBind(&ImGuiHelper::RemoveAllImGuiTextures);
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
#endif
}

void ImGuiHelper::Image(resource::Texture* texture, int max_width, int max_height)
{
#ifdef USE_IMGUI
    float ratio = (float)texture->GetWidth() / (float)texture->GetHeight();
    float width = ImGui::GetContentRegionAvail().x;
    Image(texture, static_cast<int32_t>(width), static_cast<int32_t>(width / ratio), max_width, max_height);
#endif
}

void ImGuiHelper::SameLine(float offset_from_start_x, float spacing)
{
#ifdef USE_IMGUI
    ImGui::SameLine(offset_from_start_x, spacing);
#endif
}

void ImGuiHelper::SeparatorText(const char* label)
{
#ifdef USE_IMGUI
    ImGui::SeparatorText(label);
#endif
}

void ImGuiHelper::ShowDemoWindow()
{
#ifdef USE_IMGUI
    ImGui::ShowDemoWindow();
#endif
}

bool ImGuiHelper::TreeNodeEx(const char* label, int flags)
{
#ifdef USE_IMGUI
    return ImGui::TreeNodeEx(label, flags);
#else
    return false;
#endif
}

void ImGuiHelper::TreePop()
{
#ifdef USE_IMGUI
    return ImGui::TreePop();
#endif
}

bool ImGuiHelper::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, int flags)
{
#ifdef USE_IMGUI
    return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format, flags);
#else
    return false;
#endif
}

void ImGuiHelper::WarningBox(const char* text)
{
#ifdef USE_IMGUI
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
#endif
}

void ImGuiHelper::ErrorBox(const char* text)
{
#ifdef USE_IMGUI
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
#endif
}

bool ImGuiHelper::IsItemClicked()
{
#ifdef USE_IMGUI
    return ImGui::IsItemClicked();
#else
    return true;
#endif
}

bool ImGuiHelper::IsLeftMouseDown()
{
#ifdef USE_IMGUI
    return ImGui::IsMouseDown(ImGuiMouseButton_Left);
#else
    return true;
#endif
}

bool ImGuiHelper::IsLeftMouseReleased()
{
#ifdef USE_IMGUI
    return ImGui::IsMouseReleased(ImGuiMouseButton_Left);
#else
    return true;
#endif
}

void ImGuiHelper::PushFontScale(float scale)
{
#ifdef USE_IMGUI
    old_font_scale_ = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= scale;
    ImGui::PushFont(ImGui::GetFont());
#endif
}

void ImGuiHelper::PopFontScale()
{
#ifdef USE_IMGUI
    ImGui::GetFont()->Scale = old_font_scale_;
    ImGui::PopFont();
#endif
}

void ImGuiHelper::ImageBackbuffer(int32_t width, int32_t height)
{
#ifdef USE_IMGUI
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
            rhi::vulkan::Sampler::GetDefaultSampler().GetHandle(),
            rhi::vulkan::VulkanContext::Get()->GetBackbufferView(g_engine_statistics.current_image_index)->GetHandle(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }
    float uv1x = (float)width / (float)rhi::vulkan::VulkanContext::Get()->GetActualBackbufferWidth();
    float uv1y = (float)height / (float)rhi::vulkan::VulkanContext::Get()->GetActualBackbufferHeight();
    ImGui::Image(
        back_image_texture_[g_engine_statistics.current_image_index], {static_cast<float>(width), static_cast<float>(height)}, {0, 0}, {uv1x, uv1y}
    );
#endif
}

void ImGuiHelper::ClearBackbufferDescriptorSets()
{
#ifdef USE_IMGUI
    for (auto& set: back_image_texture_)
    {
        ImGui_ImplVulkan_RemoveTexture(set);
        set = nullptr;
    }
#endif
}

void ImGuiHelper::BeginChild(const char* id, Vector2 size, int32_t child_flags, int32_t window_flags)
{
#ifdef USE_IMGUI
    ImGui::BeginChild(id, {size.x, size.y}, child_flags, window_flags);
#endif
}

void ImGuiHelper::EndChild()
{
#ifdef USE_IMGUI
    ImGui::EndChild();
#endif
}

Vector2 ImGuiHelper::GetContentRegionAvail()
{
#ifdef USE_IMGUI
    auto size = ImGui::GetContentRegionAvail();
    return {size.x, size.y};
#else
    return {};
#endif
}

void ImGuiHelper::PushID(int id)
{
#ifdef USE_IMGUI
    ImGui::PushID(id);
#endif
}

void ImGuiHelper::PopID()
{
#ifdef USE_IMGUI
    ImGui::PopID();
#endif
}

void ImGuiHelper::PushChildWindowColor(Color col)
{
#ifdef USE_IMGUI
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)col);
#endif
}

void ImGuiHelper::PopChildWindowColor()
{
#ifdef USE_IMGUI
    ImGui::PopStyleColor();
#endif
}

void ImGuiHelper::PushTextColor(Color col)
{
#ifdef USE_IMGUI
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)col);
#endif
}

void ImGuiHelper::PushTextBackgroundColor(Color col)
{
#ifdef USE_IMGUI
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, (ImVec4)col);
#endif
}

void ImGuiHelper::PopColor()
{
#ifdef USE_IMGUI
    ImGui::PopStyleColor();
#endif
}

float ImGuiHelper::GetFontSize()
{
#ifdef USE_IMGUI
    return ImGui::GetFontSize();
#else
    return 1;
#endif
}

void ImGuiHelper::SetCursorPosY(float y)
{
#ifdef USE_IMGUI
    ImGui::SetCursorPosY(y);
#endif
}

void ImGuiHelper::SetCursorPosX(float x)
{
#ifdef USE_IMGUI
    ImGui::SetCursorPosY(x);
#endif
}

float ImGuiHelper::GetCursorPosX()
{
#ifdef USE_IMGUI
    return ImGui::GetCursorPosX();
#else
    return 0;
#endif
}

void ImGuiHelper::SetCursorScreenPos(float x, float y)
{
#ifdef USE_IMGUI
    ImGui::SetCursorScreenPos({x, y});
#endif
}

void ImGuiHelper::BeginGroup()
{
#ifdef USE_IMGUI
    ImGui::BeginGroup();
#endif
}

void ImGuiHelper::EndGroup()
{
#ifdef USE_IMGUI
    ImGui::EndGroup();
#endif
}

bool ImGuiHelper::Button(const char* label, Vector2 size)
{
#ifdef USE_IMGUI
    return ImGui::Button(label, {size.x, size.y});
#else
    return true;
#endif
}

void ImGuiHelper::SetItemTooltip(const AnsiString& tooltip)
{
#ifdef USE_IMGUI
    ImGui::SetItemTooltip(tooltip.c_str());
#endif
}

Vector2 ImGuiHelper::CalcTextSize(const AnsiString& str)
{
#ifdef USE_IMGUI
    return CalcTextSize(str.c_str());
#else
    return {};
#endif
}

Vector2 ImGuiHelper::CalcTextSize(const char* str)
{
#ifdef USE_IMGUI
    ImVec2 vec     = ImGui::CalcTextSize(str);
    auto   padding = GetFramePadding();
    return Vector2{vec.x, vec.y} + Math::Multiply(padding, 2);
#else
    return {};
#endif
}

Vector2 ImGuiHelper::GetFramePadding()
{
#ifdef USE_IMGUI
    auto& style = ImGui::GetStyle();
    return {style.FramePadding.x, style.FramePadding.y};
#else
    return {};
#endif
}

void ImGuiHelper::RemoveAllImGuiTextures()
{
#ifdef USE_IMGUI
    for (auto set: imgui_textuers_ | std::views::values)
    {
        ImGui_ImplVulkan_RemoveTexture(set);
    }
    imgui_textuers_.clear();
#endif
}

Vector2 ImGuiHelper::GetCursorScreenPos()
{
#ifdef USE_IMGUI
    auto pos = ImGui::GetCursorScreenPos();
    return {pos.x, pos.y};
#else
    return {};
#endif
}

void ImGuiHelper::DrawRectFilled(Vector2 min, Vector2 max, Color color, float rounding)
{
#ifdef USE_IMGUI
    ImGui::GetWindowDrawList()->AddRectFilled(min, max, static_cast<ImU32>(color), rounding);
#endif
}

bool ImGuiHelper::IsItemHovered()
{
#ifdef USE_IMGUI
    return ImGui::IsItemHovered();
#else
    return true;
#endif
}

bool ImGuiHelper::IsItemActive()
{
#ifdef USE_IMGUI
    return ImGui::IsItemActive();
#else
    return true;
#endif
}

Color ImGuiHelper::GetButtonNormalColor()
{
#ifdef USE_IMGUI
    return ImGui::GetStyle().Colors[ImGuiCol_Button];
#else
    return {};
#endif
}

Color ImGuiHelper::GetButtonHoveredColor()
{
#ifdef USE_IMGUI
    return ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
#else
    return {};
#endif
}

Color ImGuiHelper::GetButtonPressedColor()
{
#ifdef USE_IMGUI
    return ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
#else
    return {};
#endif
}

Color ImGuiHelper::GetWindowBackgroundColor()
{
#ifdef USE_IMGUI
    return ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
#else
    return {};
#endif
}
