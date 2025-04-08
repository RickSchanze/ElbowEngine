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
#include "Func/Render/Light/PointLightComponent.hpp"
#include "Func/Render/Pipeline/PBRRenderPipeline.hpp"
#include "Func/Render/RenderContext.hpp"
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
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "Resource/Assets/Texture/Texture2DMeta.hpp"


class Texture2D;
using namespace rhi;

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
    while (!(... && args.Completed())) {
        ThreadManager::PollGameThread(10);
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
                    Move(AssetDataBase::Import("Assets/Mesh/Suitcase/Vintage_Suitcase_Normal_OpenGL.png")), // ZL,
                    Move(AssetDataBase::Import("Assets/Shader/Error.slang")),
                    Move(AssetDataBase::Import("Assets/Texture/Default.png")), //
                    Move(AssetDataBase::Import("Assets/Mesh/Cube.fbx")), //
                    Move(AssetDataBase::Import("Assets/Shader/SimpleSampledShader.slang")),
                    Move(AssetDataBase::Import("Assets/Shader/PBR/BasePass.slang")),
                    Move(AssetDataBase::Import("Assets/Shader/PBR/SkyspherePass.slang")),
                    Move(AssetDataBase::Import("Assets/Shader/PBR/ColorTransformPass.slang")),
                    Move(AssetDataBase::Import("Assets/Texture/poly_haven_studio_1k.exr")),
                    Move(AssetDataBase::Import("Assets/Mesh/Suitcase/Vintage_Suitcase_LP.fbx")));

            // 测试Texture2D的Sprite Append功能 以及CreateAsset Texture的功能
            // Texture2DMeta new_meta;
            // new_meta.dynamic = true;
            // new_meta.width = 1024;
            // new_meta.height = 1024;
            // new_meta.format = rhi::Format::R8G8B8A8_SRGB;
            // Texture2D *new_tex = ObjectManager::CreateNewObject<Texture2D>();
            // new_tex->SetName("UIAtlas");
            // new_tex->Load(new_meta);
            // new_tex->AppendSprite(IconConstantName::PureWhite(), R"(C:/Users/Echo/Documents/Temp/Icons/白.png)");
            // new_tex->AppendSprite(IconConstantName::WindowFold(), R"(C:/Users/Echo/Documents/Temp/Icons/收起.png)");
            // new_tex->AppendSprite(IconConstantName::WindowUnfold(), R"(C:/Users/Echo/Documents/Temp/Icons/展开.png)");
            // new_tex->AppendSprite(IconConstantName::CheckBox_Checked(), R"(C:/Users/Echo/Documents/Temp/Icons/checkbox-checked.png)");
            // new_tex->AppendSprite(IconConstantName::CheckBox_UnChecked(), R"(C:/Users/Echo/Documents/Temp/Icons/checkbox-unchecked.png)");
            // new_tex->SetAssetPath("Assets/Texture/UIAtlas.png");
            // new_tex->Download();
            // AssetDataBase::CreateAsset(new_tex, new_tex->GetAssetPath());
            {
                auto t1 = AssetDataBase::Import("Assets/Texture/UIAtlas.png");
                auto t2 = AssetDataBase::Import("Assets/Mesh/Suitcase/Vintage_Suitcase_Roughness.png");
                auto t3 = AssetDataBase::Import("Assets/Mesh/Suitcase/Vintage_Suitcase_Metallic.png");
                auto t4 = AssetDataBase::Import("Assets/Mesh/Suitcase/Vintage_Suitcase_AO.png");
                auto t5 = AssetDataBase::Import("Assets/Mesh/Suitcase/Vintage_Suitcase_Colour.png");
                while (true) {
                    auto all_completed = t1.Completed() && t2.Completed() && t3.Completed() && t4.Completed() && t5.Completed();
                    if (!all_completed) {
                        ThreadManager::PollGameThread(100);
                    } else {
                        break;
                    }
                }
            }

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
        auto mesh_res = static_cast<Mesh *>(AssetDataBase::Load("Assets/Mesh/Suitcase/Vintage_Suitcase_LP.fbx"));
        mesh_res->SetImportScale(0.01f);
        mesh_res->SaveIfNeed();
        mesh->SetMesh(mesh_res);
        mesh->Rotate({90, 0, 0});
        mesh->SetLocation({0, 0, 0});

        Material *m = Material::CreateFromShader("Assets/Shader/PBR/BasePass.slang");
        auto color = static_cast<Texture2D *>(AssetDataBase::Load("Assets/Mesh/Suitcase/Vintage_Suitcase_Colour.png"));
        auto metallic = static_cast<Texture2D *>(AssetDataBase::Load("Assets/Mesh/Suitcase/Vintage_Suitcase_Metallic.png"));
        metallic->SetTextureUsage(TextureUsage::Metallic);
        metallic->SaveIfNeed();
        auto roughness = static_cast<Texture2D *>(AssetDataBase::Load("Assets/Mesh/Suitcase/Vintage_Suitcase_Roughness.png"));
        roughness->SetTextureUsage(TextureUsage::Roughness);
        roughness->SaveIfNeed();
        auto ao = static_cast<Texture2D *>(AssetDataBase::Load("Assets/Mesh/Suitcase/Vintage_Suitcase_AO.png"));
        ao->SetTextureUsage(TextureUsage::AO);
        ao->SaveIfNeed();
        auto normal = static_cast<Texture2D *>(AssetDataBase::Load("Assets/Mesh/Suitcase/Vintage_Suitcase_Normal_OpenGL.png"));
        normal->SetTextureUsage(TextureUsage::Normal);
        normal->SaveIfNeed();
        m->SetTexture2D("tex", color);
        m->SetTexture2D("tex_metallic", metallic);
        m->SetTexture2D("tex_roughness", roughness);
        m->SetTexture2D("tex_ao", ao);
        m->SetTexture2D("tex_normal", normal);
        m->SetFloat("float_param.roughness", 1.0f);
        m->SetFloat("float_param.metallic", 1.0f);
        m->SetFloat("float_param.ao", 1.0f);
        auto light = cam_holder->AddComponent<PointLightComponent>();
        light->SetColor(Color::White());
        mesh->SetMaterial(m);
        light->SetIntensity(5);
        while (true) {
            ProfileScope _("Tick");
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
