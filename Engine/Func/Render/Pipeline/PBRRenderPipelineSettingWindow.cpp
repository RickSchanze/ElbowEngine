//
// Created by Echo on 2025/4/12.
//

#include "PBRRenderPipelineSettingWindow.hpp"

#include "Core/Async/Exec/Just.hpp"
#include "Func/Render/Offline/EnvironmentMapBaker.hpp"
#include "Func/UI/ImGuiDrawer.hpp"
#include "PBRRenderPipeline.hpp"
#include "Func/Render/RenderTexture.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Resource/Assets/Material/Material.hpp"

void PBRRenderPipelineSettingWindow::Draw()
{
    if (ImGuiDrawer::Begin("PBR渲染管线设置"))
    {
        if (ImGui::Button("生成预过滤贴图并替换天空盒"))
        {
            GeneratePrefilteredMapAndApply();
        }
        if (ImGui::Button("生成辐照度图并替换天空盒"))
        {
            GenerateIrradianceMapAndApply();
        }
        if (ImGui::Button("生成积分BRDF图并显示"))
        {
            GenerateBRDFMapApply();
        }
        if (ImGui::Button("应用ShadowMap"))
        {
            ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([this]() {
                RHI::GetGfxContextRef().WaitForQueueExecution(RHI::QueueFamilyType::Graphics);
                mMeshMat->SetParamNativeImageView("Tex_ShadowMap", mPipeline->mShadowBox->GetImageView());
            }), NamedThread::Game);
        }
        if (mBRDFMap)
        {
            ImGuiDrawer::Image(mBRDFMap, Vector2f{200, 200});
        }
        ImGui::DragFloat("粗糙度", &mRoughness, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("金属度", &mMetallic, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("AO", &mAO, 0.01f, 0.0f, 1.0f);
        ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([this]() {
            mMeshMat->SetFloat("InFloatParams.Roughness", mRoughness);
            mMeshMat->SetFloat("InFloatParams.Metallic", mMetallic);
            mMeshMat->SetFloat("InFloatParams.AO", mAO);
        }), NamedThread::Game);
    }
    ImGui::End();
}

void PBRRenderPipelineSettingWindow::SetRenderPipeline(PBRRenderPipeline *pipeline)
{
    mPipeline = pipeline;
}

void PBRRenderPipelineSettingWindow::GeneratePrefilteredMapAndApply() const
{
    auto w = mPipeline->skybox_texture_->GetWidth();
    auto h = mPipeline->skybox_texture_->GetHeight();
    auto scale = w / h;
    Texture2D *tex = EnvironmentMapBaker::BakePrefilteredEnvironmentMap(mPipeline->skybox_texture_, 1024 * scale, 1024, 5);
    if (tex)
    {
        auto task = ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([tex, this]() {
                                                           mMeshMat->SetTexture2D("Tex_Prefiltered", tex);
                                                       }),
                                                       NamedThread::Game);
    }
}

void PBRRenderPipelineSettingWindow::GenerateIrradianceMapAndApply() const
{
    auto w = mPipeline->skybox_texture_->GetWidth();
    auto h = mPipeline->skybox_texture_->GetHeight();
    auto scale = w / h;
    Texture2D *tex = EnvironmentMapBaker::BakeIrradianceMap(mPipeline->skybox_texture_, Vector2f(1024 * scale, 1024), 0.05, 1);
    if (tex)
    {
        auto task = ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([tex, this]() {
                                                           mMeshMat->SetTexture2D("Tex_Irradiance", tex);
                                                       }),
                                                       NamedThread::Game);
    }
}

void PBRRenderPipelineSettingWindow::GenerateBRDFMapApply()
{
    mBRDFMap = EnvironmentMapBaker::BakeIntegrateBRDFLookUpMap(800);
    if (mBRDFMap)
    {
        auto task = ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([this]() {
                                                           mMeshMat->SetTexture2D("Tex_BRDFLUT", mBRDFMap);
                                                       }),
                                                       NamedThread::Game);
    }
}