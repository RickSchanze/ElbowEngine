//
// Created by Echo on 2025/4/8.
//

#include "ImGuiDrawer.hpp"

#include <imgui_impl_vulkan.h>
#include "imgui_internal.h"

#include "Func/Render/RenderTexture.hpp"
#include "Func/World/Actor.hpp"
#include "Func/World/SceneComponent.hpp"
#include "Func/World/Transform.hpp"
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

void ImGuiDrawer::ItemLabel(StringView title, ImGuiItemLabelFlag flags) {
    using namespace ImGui;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    const ImVec2 lineStart = ImGui::GetCursorScreenPos();
    const ImGuiStyle &style = ImGui::GetStyle();
    float fullWidth = ImGui::GetContentRegionAvail().x;
    float itemWidth = ImGui::CalcItemWidth() + style.ItemSpacing.x;
    const char *title_begin = *title;
    const char *title_end = title_begin + title.ByteCount();
    ImVec2 textSize = ImGui::CalcTextSize(title_begin, title_end);
    ImRect textRect;
    textRect.Min = ImGui::GetCursorScreenPos();
    if (flags == ImGuiItemLabelFlag::Right)
        textRect.Min.x = textRect.Min.x + itemWidth;
    textRect.Max = textRect.Min;
    textRect.Max.x += fullWidth - itemWidth;
    textRect.Max.y += textSize.y;

    SetCursorScreenPos(textRect.Min);

    AlignTextToFramePadding();
    // Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
    textRect.Min.y += window->DC.CurrLineTextBaseOffset;
    textRect.Max.y += window->DC.CurrLineTextBaseOffset;

    ItemSize(textRect);
    if (ItemAdd(textRect, window->GetID(title_begin, title_end))) {
        RenderTextEllipsis(ImGui::GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x, textRect.Max.x, title_begin, title_end, &textSize);

        if (textRect.GetWidth() < textSize.x && ImGui::IsItemHovered())
            ImGui::SetTooltip("%.*s", title.ByteCount(), *title);
    }
    if (flags == ImGuiItemLabelFlag::Left) {
        ImVec2 screen_pos = {textRect.Max.x - 0, textRect.Max.y - (textSize.y + window->DC.CurrLineTextBaseOffset)};
        ImGui::SetCursorScreenPos(screen_pos);
        ImGui::SameLine();
    } else if (flags == ImGuiItemLabelFlag::Right)
        ImGui::SetCursorScreenPos(lineStart);
}

void ImGuiDrawer::Shutdown() {
    for (auto &pair: texture_map_) {
        ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(pair.second.id));
    }
}

bool ImGuiDrawer::CheckBox(const char *label, bool *checked) {
    ItemLabel(label, ImGuiItemLabelFlag::Left);
    return ImGui::Checkbox("##", checked);
}

bool ImGuiDrawer::InputFloat3(const char *label, float *v, const char *format, ImGuiInputTextFlags extra_flags) {
    ItemLabel(label, ImGuiItemLabelFlag::Left);
    return ImGui::InputFloat3("##", v, format, extra_flags);
}

void ImGuiDrawer::DrawTransform(Actor *actor) {
    Transform trans = actor->GetTransform();
    Vector3f rotation = trans.GetRotationEuler();
    ImGui::PushID(actor);
    ImGui::PushID("位置");
    InputFloat3("位置", &trans.location.x);
    ImGui::PopID();
    ImGui::PushID("旋转");
    InputFloat3("旋转", &rotation.x);
    ImGui::PopID();
    ImGui::PushID("缩放");
    InputFloat3("缩放", &trans.scale.x);
    ImGui::PopID();
    ImGui::PopID();
    Transform old = actor->GetTransform();
    if (trans.location != old.location) {
        actor->SetLocation(trans.location);
    }
    if (trans.scale != old.scale) {
        actor->SetScale(trans.scale);
    }
    if (rotation != trans.GetRotationEuler()) {
        actor->SetRotation(rotation);
    }
}


void ImGuiDrawer::DrawTransform(SceneComponent *comp) {
    Transform trans = comp->GetTransform();
    Vector3f rotation = trans.GetRotationEuler();
    ImGui::PushID(comp);
    InputFloat3("位置", &trans.location.x);
    InputFloat3("旋转", &rotation.x);
    InputFloat3("缩放", &trans.scale.x);
    ImGui::PopID();
    Transform old = comp->GetTransform();
    if (trans.location != old.location) {
        comp->SetLocation(trans.location);
    }
    if (trans.scale != old.scale) {
        comp->SetScale(trans.scale);
    }
    if (rotation != trans.GetRotationEuler()) {
        comp->SetRotation(rotation);
    }
}
