//
// Created by Echo on 2025/4/8.
//

#include "ImGuiDrawer.hpp"

#include <imgui_impl_vulkan.h>

#include "Func/Render/RenderTexture.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/ImageView.hpp"

bool ImGuiDrawer::Begin(const char *title, bool *p_open, ImGuiWindowFlags f) { return ImGui::Begin(title, p_open, f); }
void ImGuiDrawer::End() { ImGui::End(); }

void ImGuiDrawer::Image(RenderTexture &tex, Vector2f size, Vector2f uv0, Vector2f uv1) {
    auto &self = GetByRef();
    RenderTexture *p_tex = &tex;
    if (self.texture_map_.Contains(p_tex)) {
        auto &image_data = self.texture_map_[p_tex];
        if (image_data.view == tex.GetImageView()) {
            ImGui::Image(self.texture_map_[p_tex].id, ImVec2{size.x, size.y}, ImVec2{uv0.x, uv0.y}, ImVec2{uv1.x, uv1.y});
            return;
        }
        ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(image_data.id));
        self.texture_map_.Remove(p_tex);
    }
    VkDescriptorSet set = ImGui_ImplVulkan_AddTexture(rhi::GetGfxContextRef().GetSampler(rhi::SamplerDesc{})->GetNativeHandleT<VkSampler>(),
                                                      tex.GetImageView()->GetNativeHandleT<VkImageView>(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    self.texture_map_[p_tex] = ImGuiImageData{reinterpret_cast<ImTextureID>(set), tex.GetImageView()};
    ImGui::Image(reinterpret_cast<ImTextureID>(set), ImVec2{size.x, size.y}, ImVec2{uv0.x, uv0.y}, ImVec2{uv1.x, uv1.y});
}

void ImGuiDrawer::Shutdown() {
    for (auto &pair: texture_map_) {
        ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(pair.second.id));
    }
}
