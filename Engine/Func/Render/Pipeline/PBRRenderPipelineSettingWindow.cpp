//
// Created by Echo on 2025/4/12.
//

#include "PBRRenderPipelineSettingWindow.hpp"

#include "Core/Async/Exec/Just.hpp"
#include "Func/Render/Offline/EnvironmentMapBaker.hpp"
#include "Func/UI/ImGuiDrawer.hpp"
#include "PBRRenderPipeline.hpp"
#include "Resource/Assets/Material/Material.hpp"

IMPL_REFLECTED(PBRRenderPipelineSettingWindow) {
    return Type::Create<PBRRenderPipelineSettingWindow>("PBRRenderPipelineSettingWindow") | refl_helper::AddParents<ImGuiDrawWindow>();
}

void PBRRenderPipelineSettingWindow::Draw() {
    if (ImGuiDrawer::Begin("PBR渲染管线设置")) {
        if (ImGui::Button("生成预过滤贴图并替换天空盒")) {
            GeneratePrefilteredEnvironmentMapAndReplaceSkyBox();
        }
    }
    ImGui::End();
}

void PBRRenderPipelineSettingWindow::SetRenderPipeline(PBRRenderPipeline *pipeline) { pipeline_ = pipeline; }

void PBRRenderPipelineSettingWindow::GeneratePrefilteredEnvironmentMapAndReplaceSkyBox() const {
    auto w = pipeline_->skybox_texture_->GetWidth();
    auto h = pipeline_->skybox_texture_->GetHeight();
    auto scale = w / h;
    Texture2D *tex = EnvironmentMapBaker::BakePrefilteredEnvironmentMap(pipeline_->skybox_texture_, h * scale, 512, 5);
    if (tex) {
        auto task = ThreadManager::ScheduleFutureAsync(
                exec::Just() | exec::Then([tex, this]() { pipeline_->skysphere_pass_material_->SetTexture2D("skybox_texture", tex); }),
                NamedThread::Game);
    }
}
