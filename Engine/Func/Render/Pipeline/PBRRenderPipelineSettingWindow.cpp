//
// Created by Echo on 2025/4/12.
//

#include "PBRRenderPipelineSettingWindow.hpp"

#include "Core/Async/Exec/Just.hpp"
#include "Func/Render/Offline/EnvironmentMapBaker.hpp"
#include "Func/UI/ImGuiDrawer.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Resource/Assets/Material/Material.hpp"

void PBRRenderPipelineSettingWindow::Draw()
{
    if (ImGuiDrawer::Begin("PBR渲染管线设置"))
    {

    }
    ImGui::End();
}

void PBRRenderPipelineSettingWindow::SetRenderPipeline(PBRRenderPipeline *pipeline)
{
    mPipeline = pipeline;
}

void PBRRenderPipelineSettingWindow::GeneratePrefilteredMapAndApply() const
{

}

void PBRRenderPipelineSettingWindow::GenerateIrradianceMapAndApply() const
{

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