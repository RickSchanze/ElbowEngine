/**
 * @file ImGuiHelper.cpp
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#include "ImGuiHelper.h"

#include "CachedString.h"
#include "IconsMaterialDesign.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Texture.h"

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

void ImGuiHelper::Image(Resource::Texture* texture, int32_t width, int32_t height)
{
    if (texture == nullptr)
    {
        return;
    }
    if (imgui_textuers_.empty())
    {
        RHI::Vulkan::VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&ImGuiHelper::RemoveAllImGuiTextures);
    }
    VkDescriptorSet set;
    if (imgui_textuers_.contains(texture))
    {
        set = imgui_textuers_[texture];
    }
    else
    {
        set = ImGui_ImplVulkan_AddTexture(
            texture->GetSampler()->GetHandle(), texture->GetTextureView()->GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        imgui_textuers_[texture] = set;
    }
    ImGui::Image(set, {static_cast<float>(width), static_cast<float>(height)});
}

void ImGuiHelper::Image(Resource::Texture* texture)
{
    float ratio = (float)texture->GetWidth() / (float)texture->GetHeight();
    float width = ImGui::GetContentRegionAvail().x;
    Image(texture, static_cast<int32_t>(width), static_cast<int32_t>(width / ratio));
}

void ImGuiHelper::SameLine()
{
    ImGui::SameLine();
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
    ImGui::PushStyleColor(ImGuiCol_Border, Color::Warning());
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

void ImGuiHelper::RemoveAllImGuiTextures()
{
    for (auto set: imgui_textuers_ | std::views::values)
    {
        ImGui_ImplVulkan_RemoveTexture(set);
    }
    imgui_textuers_.clear();
}
