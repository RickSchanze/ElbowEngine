//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiDemoWindow.hpp"


namespace rhi {
    class ImageView;
}
class RenderTexture;
namespace rhi {
    class DescriptorSet;
}

struct ImGuiImageData {
    ImTextureID id;
    rhi::ImageView *view;
};

enum class ImGuiItemLabelFlag {
    Left,
    Right,
};

// 只是一些便于本项目的imgui默认值wraper
class ImGuiDrawer : public Manager<ImGuiDrawer> {
public:
    Float GetLevel() const override { return 15.f; }
    StringView GetName() const override { return "ImGuiDrawer"; }

    static bool Begin(const char *title, bool *p_open = nullptr, ImGuiWindowFlags f = ImGuiWindowFlags_NoCollapse);
    static void End();
    static void Image(RenderTexture &tex, Vector2f size, Vector2f uv0 = {0, 0}, Vector2f uv1 = {1, 1});

    static void ItemLabel(StringView title, ImGuiItemLabelFlag flags);

    void Shutdown() override;

    static bool CheckBox(const char *label, bool *checked);
    static bool InputFloat3(const char *label, float *v, const char *format = "%.3f", ImGuiInputTextFlags extra_flags = 0);

private:
    Map<RenderTexture *, ImGuiImageData> texture_map_;
};
