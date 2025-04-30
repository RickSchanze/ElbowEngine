//
// Created by Echo on 2025/4/2.
//

#include "PBRRenderPipeline.hpp"

#include "Core/Config/ConfigManager.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/Render/Light/LightManager.hpp"
#include "Func/Render/Pipeline/PBRRenderPipelineSettingWindow.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Func/Render/RenderTexture.hpp"
#include "Func/UI/ImGuiDemoWindow.hpp"
#include "Func/UI/UIManager.hpp"
#include "Func/UI/ViewportWindow.hpp"
#include "Func/World/Actor.hpp"
#include "Func/World/StaticMeshComponent.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/Misc.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"

using namespace RHI;

void PBRRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params)
{

}

ImGuiDrawWindow *PBRRenderPipeline::GetSettingWindow()
{
    PBRRenderPipelineSettingWindow *w = UIManager::CreateOrActivateWindow<PBRRenderPipelineSettingWindow>(true);
    w->SetRenderPipeline(this);
    return w;
}

void PBRRenderPipeline::Build()
{
}

void PBRRenderPipeline::Clean()
{
}

bool PBRRenderPipeline::IsReady() const
{
    return mReady;
}

void PBRRenderPipeline::OnWindowResized(PlatformWindow *window, Int32 width, Int32 height)
{
}