#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>
#include <windows.h>

#include "Core/Async/ThreadManager.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Core/FileSystem/Path.hpp"
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

static void TickManagerUpdate(const MilliSeconds &sec)
{
    static UInt32 interval = GetConfig<CoreConfig>()->GetTickFrameInterval();
    static UInt32 cnt = 0;
    cnt++;
    if (cnt >= interval)
    {
        SharedMaterialManager::GetByRef().UpdateSharedMaterialSet();
        DescriptorSetLayoutPool::GetByRef().Update();
        cnt = 0;
    }
}

template <typename... Args>
static void ResourceInitCreate(Args &&... args)
{
    while (true)
    {
        const bool all_completed = (args.IsCompleted() && ...);
        if (!all_completed)
        {
            ThreadManager::PollGameThread(100);
        }
        else
        {
            break;
        }
    }
}

int main()
{
    try
    {
        VLOG_INFO("正在初始化...");
        // 让std::wcout 顺利运行
        setlocale(LC_ALL, "zh_CN");
        // 让spdlog不产生乱码
        SetConsoleOutputCP(65001);
        if (!Path::SetProjectPath("C:/Users/Echo/Documents/Projects/ElbowEngine/Content"))
        {
            VLOG_FATAL("Set project path failed, Abort!!!");
            return -1;
        } {
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
                Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/PBR/PBRSphere.obj")), // Mesh
                Move(AssetDataBase::LoadOrImportAsync("Assets/Mesh/PBR/PBRSphere_Albedo.jpg")), // Albedo,
                Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/Environment/IntegrateBRDF.slang")), // Albedo,
                Move(AssetDataBase::LoadOrImportAsync("Assets/Shader/PBR/ShadowPass.slang")) // Albedo,
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
        // cam_holder->SetLocation({0, 0, 5.5});
        // cam_holder->SetScale({0.8, 0.8, 0.8});
        cam_holder->SetDisplayName("摄像机");
        const auto handle = TickEvents::Evt_WorldPostTick.AddBind(&TickManagerUpdate);
        auto mesh_actor = Scene::GetByRef().CreateActor<Actor>();
        mesh_actor->SetDisplayName("PBR物体");
        auto mesh = mesh_actor->AddComponent<StaticMeshComponent>();
        auto mesh_res = static_cast<Mesh *>(AssetDataBase::LoadFromPath("Assets/Mesh/PBR/PBRSphere.obj"));
        mesh_res->SaveIfNeed();
        mesh->SetMesh(mesh_res);

        Material *ObjectMaterial = Material::CreateFromShader("Assets/Shader/PBR/BasePass.slang");
        auto Color = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Mesh/PBR/PBRSphere_Albedo.jpg"));
        auto White = static_cast<Texture2D *>(AssetDataBase::LoadFromPath("Assets/Texture/White.png"));
        ObjectMaterial->SetName("MeshMaterial");
        ObjectMaterial->SetTexture2D("Tex_Albedo", Color);
        ObjectMaterial->SetTexture2D("Tex_Metallic", White);
        ObjectMaterial->SetTexture2D("Tex_Roughness", White);
        ObjectMaterial->SetTexture2D("Tex_AO", White);
        ObjectMaterial->SetTexture2D("Tex_Normal", White);
        ObjectMaterial->SetTexture2D("Tex_Irradiance", White);
        ObjectMaterial->SetTexture2D("Tex_Prefiltered", White);
        ObjectMaterial->SetTexture2D("Tex_BRDFLUT", White);
        ObjectMaterial->SetFloat("InFloatParams.Metallic", 1.0f);
        ObjectMaterial->SetFloat("InFloatParams.Roughness", 1.0f);
        ObjectMaterial->SetFloat("InFloatParams.AO", 1.0f);
        auto* LightActor = Scene::GetByRef().CreateActor<Actor>();
        LightActor->SetDisplayName("光");
        auto light = LightActor->AddComponent<PointLightComponent>();
        light->SetColor(Color::White());
        mesh->SetMaterial(ObjectMaterial);
        light->SetIntensity(5);
        light->SetLocation({0, 5, 0});
        UIManager::CreateOrActivateWindow(TypeOf<ViewportWindow>());
        UIManager::CreateOrActivateWindow(TypeOf<InspectorWindow>());
        UIManager::CreateOrActivateWindow(TypeOf<DetailWindow>());
        PBRRenderPipelineSettingWindow *PBRWindow =
            reinterpret_cast<PBRRenderPipelineSettingWindow *>(RenderContext::GetBoundRenderPipeline()->GetSettingWindow());
        PBRWindow->SetMeshMaterial(ObjectMaterial);

        auto MeshComp = Scene::GetByRef().CreateActor<Actor>()->AddComponent<StaticMeshComponent>();
        MeshComp->GetOwner()->SetDisplayName("地板");
        MeshComp->SetMesh(AssetDataBase::LoadFromPath<Mesh>("Assets/Mesh/Cube.fbx"));
        MeshComp->SetMaterial(ObjectMaterial);
        MeshComp->SetLocation(Vector3f(0, -13, 0));
        MeshComp->SetScale({3, 0.1, 6});

        while (true)
        {
            ProfileScope _("Tick");
            GetWorldClock().TickAll(main_window);
            if (main_window->ShouldClose())
            {
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
    }
    catch (const std::exception &e)
    {
        VLOG_FATAL("程序因为异常崩溃了! ", e.what());
        return -1;
    }
}