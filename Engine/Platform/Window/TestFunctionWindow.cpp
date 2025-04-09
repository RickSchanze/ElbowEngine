//
// Created by Echo on 2025/4/9.
//

#include "TestFunctionWindow.hpp"

#include "Func/Render/Offline/ImageTransformer.hpp"
#include "Func/UI/ImGuiDrawer.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"

IMPL_REFLECTED(TestFunctionWindow) { return Type::Create<TestFunctionWindow>("TestFunctionWindow") | refl_helper::AddParents<ImGuiDrawWindow>(); }

void TestFunctionWindow::Draw() {
    if (ImGuiDrawer::Begin("功能测试")) {
        if (ImGui::Button("将球面贴图转换为立方体贴图")) {
            auto *tex = AssetDataBase::LoadOrImportT<Texture2D>("Assets/Texture/poly_haven_studio_1k.exr");
            if (tex) {
                auto *cubemap = ImageTransformer::TransformSphereMapToCubeMap(tex, 512, rhi::Format::R32G32B32A32_Float);
                VLOG_INFO("创建cubemap!:", cubemap);
            }
        }
    }
    ImGui::End();
}
