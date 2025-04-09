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

// 只是一些便于本项目的imgui默认值wraper
class ImGuiDrawer : public Manager<ImGuiDrawer> {
public:
    Float GetLevel() const override { return 15.f; }
    StringView GetName() const override { return "ImGuiDrawer"; }

    static bool Begin(const char *title, bool *p_open = nullptr, ImGuiWindowFlags f = ImGuiWindowFlags_NoCollapse);
    static void End();
    static void Image(RenderTexture &tex, Vector2f size, Vector2f uv0 = {0, 0}, Vector2f uv1 = {1, 1});

    void Shutdown() override;

private:
    Map<RenderTexture *, ImGuiImageData> texture_map_;
};
