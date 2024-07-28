/**
 * @file ImGuiHelper.cpp
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#include "ImGuiHelper.h"

#include "CachedString.h"
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

void ImGuiHelper::SameLine()
{
    ImGui::SameLine();
}

void ImGuiHelper::SeparatorText(const char* label)
{
    ImGui::SeparatorText(label);
}

void ImGuiHelper::RemoveAllImGuiTextures()
{
    for (auto set: imgui_textuers_ | std::views::values)
    {
        ImGui_ImplVulkan_RemoveTexture(set);
    }
    imgui_textuers_.clear();
}
