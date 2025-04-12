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
            GeneratePrefilteredMapAndReplaceSkyBox();
        }
        if (ImGui::Button("生成辐照度图并替换天空盒")) {
            GenerateIrradianceMapAndReplaceSkyBox();
        }
    }
    ImGui::End();
}

void PBRRenderPipelineSettingWindow::SetRenderPipeline(PBRRenderPipeline *pipeline) { pipeline_ = pipeline; }

void PBRRenderPipelineSettingWindow::GeneratePrefilteredMapAndReplaceSkyBox() const {
    auto w = pipeline_->skybox_texture_->GetWidth();
    auto h = pipeline_->skybox_texture_->GetHeight();
    auto scale = w / h;
    Texture2D *tex = EnvironmentMapBaker::BakePrefilteredEnvironmentMap(pipeline_->skybox_texture_, 512 * scale, 512, 5);
    if (tex) {
        auto task = ThreadManager::ScheduleFutureAsync(
                exec::Just() | exec::Then([tex, this]() { pipeline_->skysphere_pass_material_->SetTexture2D("skybox_texture", tex); }),
                NamedThread::Game);
    }
}

void PBRRenderPipelineSettingWindow::GenerateIrradianceMapAndReplaceSkyBox() const {
    auto w = pipeline_->skybox_texture_->GetWidth();
    auto h = pipeline_->skybox_texture_->GetHeight();
    auto scale = w / h;
    Texture2D *tex = EnvironmentMapBaker::BakeIrradianceMap(pipeline_->skybox_texture_, Vector2f(512 * scale, 512), 0.01, 1);
    if (tex) {
        auto task = ThreadManager::ScheduleFutureAsync(
                exec::Just() | exec::Then([tex, this]() {
                    pipeline_->skysphere_pass_material_->SetTexture2D("skybox_texture", tex);
                }),
                NamedThread::Game);
    }
}
