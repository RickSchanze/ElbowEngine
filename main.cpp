#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>
#include <windows.h>

#include "Core/Async/ThreadManager.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Math/MathExtensions.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Core/TypeAlias.hpp"
#include "Func/Input/Input.hpp"
#include "Func/Render/Camera/ACameraHolder.hpp"
#include "Func/Render/Camera/Camera.hpp"
#include "Func/Render/Camera/Rotating.hpp"
#include "Func/Render/Light/PointLightComponent.hpp"
#include "Func/Render/Pipeline/PBRRenderPipeline.hpp"
#include "Func/Render/Pipeline/PBRRenderPipelineSettingWindow.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Func/UI/DetailWindow.hpp"
#include "Func/UI/InspectorWindow.hpp"
#include "Func/UI/UIManager.hpp"
#include "Func/World/Scene/Scene.hpp"
#include "Func/World/StaticMeshComponent.hpp"
#include "Func/World/WorldClock.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/ImGuiContextProxy.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/SyncPrimitives.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Platform/Window/Window.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "Resource/Assets/Texture/Texture2DMeta.hpp"


class Texture2D;
using namespace RHI;

static void TickManagerUpdate(const MilliSeconds &sec) {
    static UInt32 interval = GetConfig<CoreConfig>()->GetTickFrameInterval();
    static UInt32 cnt = 0;
    cnt++;
    if (cnt >= interval) {
        SharedMaterialManager::GetByRef().UpdateSharedMaterialSet();
        DescriptorSetLayoutPool::GetByRef().Update();
        cnt = 0;
    }
}

template<typename... Args>
static void ResourceInitCreate(Args &&...args) {
    while (true) {
        const bool all_completed = (args.IsCompleted() && ...);
        if (!all_completed) {
            ThreadManager::PollGameThread(100);
        } else {
            break;
        }
    }
}

int main() {
    try {
        VLOG_INFO("正在初始化...");
        // 让std::wcout 顺利运行
        setlocale(LC_ALL, "zh_CN");
        // 让spdlog不产生乱码
        SetConsoleOutputCP(65001);
        if (!Path::SetProjectPath("C:/Users/Echo/Documents/Projects/ElbowEngine/Content")) {
            VLOG_FATAL("Set project path failed, Abort!!!");
            return -1;
        }

        {
            ThreadManager::Get();
        }
        // 窗口初始化
        {
            ProfileScope _("Window Initialize");
            CreatePlatformWindow(WindowLib::GLFW);
            Input::GetByRef();
        }
        // 图形初始化
        {
            ProfileScope _("Graphics Initialize");
            const auto rhi_cfg = GetConfig<PlatformConfig>();
            UseGraphicsAPI(rhi_cfg->GetGraphicsAPI());
            GfxContextLifeTimeProxyManager::Get();
            RenderContext::GetByRef();
            Camera::GetByRef();
        }
        // 资产数据库初始化
        {
            ProfileScope _("AssetDataBase Initialize");
            AssetDataBase::Get();
            // TODO: 这个函数并没有真正等待所有任务完成
            ResourceInitCreate( //
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/Error.slang")),
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Texture/Default.png")), //
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cube.fbx")), //
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/BasePass.slang")),
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/SkyspherePass.slang")),
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/ColorTransformPass.slang")), //
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/Environment/PrefilteredColor.slang")), //
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Texture/Black.png")), // Black
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Texture/White.png")), // White
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Texture/White.png")) // White
            );

            // PBR资产(测试用)
            ResourceInitCreate( //
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cerberus/Cerberus_LP.fbx")), // Mesh
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cerberus/Textures/Cerberus_A.png")), // Albedo,
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cerberus/Textures/Cerberus_M.png")), // Metallic
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cerberus/Textures/Cerberus_N.png")), // Normal
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cerberus/Textures/Cerberus_R.png")), // Roughness
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/Cerberus/Textures/Cerberus_AO.png")), // AO
                    Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/Environment/IntegrateBRDF.slang")) //
            );

#if WITH_EDITOR
            ObjectManager::SaveObjectRegistry();
#endif
        }
        // 其他初始化
        {
            ProfileScope _("Other Initialize");
            ThreadManager::Get();
            Camera::Get();
        }
        VLOG_INFO("初始化完成!");
        PlatformWindow *main_window = PlatformWindowManager::GetMainWindow();
        TickEvents::Evt_TickInput.Bind(main_window, &PlatformWindow::PollInputs);
        RenderContext::GetByRef().SetRenderPipeline(MakeUnique<PBRRenderPipeline>());

        auto cam_holder = Scene::GetByRef().CreateActor<ACameraHolder>();
        cam_holder->SetDisplayName("摄像机");
        const auto handle = TickEvents::Evt_WorldPostTick.AddBind(&TickManagerUpdate);
        auto mesh_actor = Scene::GetByRef().CreateActor<Actor>();
        mesh_actor->SetDisplayName("PBR物体");
        auto mesh = mesh_actor->AddComponent<StaticMeshComponent>();
        auto mesh_res = static_cast<Mesh *>(AssetDataBase::LoadFromPath("Assets/Mesh/Cerberus/Cerberus_LP.fbx"));
        mesh_res->SetImportScale(0.01f);
        mesh_res->SaveIfNeed();
        mesh->SetMesh(mesh_res);
        mesh_actor->SetRotation({90, 0, 0});
        mesh->SetLocation({0, 0, 0});

        Material *m = Material::CreateFromShader("Assets/Shader/PBR/BasePass.slang");
        auto color = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Mesh/Cerberus/Textures/Cerberus_A.png"));
        auto metallic = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Mesh/Cerberus/Textures/Cerberus_M.png"));
        metallic->SetTextureFormat(Format::R8_UNorm);
        metallic->SaveIfNeed();
        auto roughness = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Mesh/Cerberus/Textures/Cerberus_R.png"));
        roughness->SetTextureFormat(Format::R8_UNorm);
        roughness->SaveIfNeed();
        auto ao = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Mesh/Cerberus/Textures/Cerberus_AO.png"));
        ao->SetTextureFormat(Format::R8_UNorm);
        ao->SaveIfNeed();
        auto normal = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Mesh/Cerberus/Textures/Cerberus_N.png"));
        normal->SetTextureFormat(Format::R8_UNorm);
        normal->SaveIfNeed();
        auto Black = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Texture/Black.png"));
        m->SetTexture2D("tex", color);
        m->SetTexture2D("tex_metallic", metallic);
        m->SetTexture2D("tex_roughness", roughness);
        m->SetTexture2D("tex_ao", ao);
        m->SetTexture2D("tex_normal", normal);
        m->SetTexture2D("Tex_Irradiance", Black);
        m->SetTexture2D("Tex_Prefiltered", Black);
        m->SetTexture2D("Tex_BRDFLUT", Black);
        m->SetFloat("float_param.roughness", 1.0f);
        m->SetFloat("float_param.metallic", 1.0f);
        m->SetFloat("float_param.ao", 1.0f);
        auto light_holder = CreateNewObject<Rotating>();
        auto light = cam_holder->AddComponent<PointLightComponent>();
        light->SetColor(Color::White());
        mesh->SetMaterial(m);
        light->SetIntensity(5);
        UIManager::CreateOrActivateWindow(TypeOf<ViewportWindow>());
        UIManager::CreateOrActivateWindow(TypeOf<InspectorWindow>());
        UIManager::CreateOrActivateWindow(TypeOf<DetailWindow>());
        PBRRenderPipelineSettingWindow *PBRWindow =
                reinterpret_cast<PBRRenderPipelineSettingWindow *>(RenderContext::GetBoundRenderPipeline()->GetSettingWindow());
        PBRWindow->SetMeshMaterial(m);

        while (true) {
            ProfileScope _("Tick");
            // 构建normal_matrix
            glm::vec3 location = mesh->GetLocation() | ToGLMVec3;
            glm::vec3 rotation = mesh->GetRotation() | ToGLMVec3;
            glm::vec3 scale = mesh->GetScale() | ToGLMVec3;
            glm::mat4 model = glm::translate(glm::mat4(1.0f), location) * glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
                              glm::scale(glm::mat4(1.0f), scale);
            Matrix4x4f normal_matrix = glm::transpose(glm::inverse(glm::mat3(model))) | ToMatrix4x4f;
            m->SetMatrix4x4("martix_param.normal_matrix", normal_matrix);
            GetWorldClock().TickAll(main_window);
            if (main_window->ShouldClose()) {
                GetGfxContextRef().WaitForDeviceIdle();
                main_window->Close();
                break;
            }
        }
        TickEvents::Evt_WorldPostTick.RemoveBind(handle);
        GetGfxContext()->WaitForDeviceIdle();
        TickEvents::Evt_TickInput.Unbind();
        VLOG_INFO("关闭引擎, 清理资源...");
        MManager::Get()->Shutdown();
    } catch (const std::exception &e) {
        VLOG_FATAL("程序因为异常崩溃了! ", e.what());
        return -1;
    }
}
