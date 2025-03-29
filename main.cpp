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
#include "Func/Render/ElbowEngineRenderPipeline.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Func/UI/Widget/Window.hpp"
#include "Func/World/WorldClock.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/SyncPrimitives.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Platform/Window/Window.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"

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
            ResourceInitCreate( //
                    Move(AssetDataBase::Import("Assets/Shader/Error.slang")), Move(AssetDataBase::Import("Assets/Texture/Default.png")),
                    Move(AssetDataBase::Import("Assets/Mesh/Cube.fbx")), //
                    Move(AssetDataBase::Import("Assets/Font/MapleMono.ttf")), Move(AssetDataBase::Import("Assets/Shader/UIDefault.slang")),
                    Move(AssetDataBase::Import("Assets/Texture/UIAtlas.png")), //
                    Move(AssetDataBase::Import("Assets/Shader/SimpleSampledShader.slang")) //
            );

            // 测试Texture2D的Sprite Append功能 以及CreateAsset Texture的功能
            // Texture2DMeta new_meta;
            // new_meta.dynamic = true;
            // new_meta.width = 1024;
            // new_meta.height = 1024;
            // new_meta.format = rhi::Format::R8G8B8A8_SRGB;
            // Texture2D *new_tex = ObjectManager::CreateNewObject<Texture2D>();
            // new_tex->SetName("UIAtlas");
            // new_tex->Load(new_meta);
            // new_tex->AppendSprite(ui::IconID::Close(), R"(C:\Users\Echo\Documents\Temp\Icons\关闭.png)");
            // new_tex->AppendSprite(ui::IconID::White(), R"(C:\Users\Echo\Documents\Temp\Icons\白.png)");
            // new_tex->AppendSprite(ui::IconID::Folded(), R"(C:\Users\Echo\Documents\Temp\Icons\收起.png)");
            // new_tex->AppendSprite(ui::IconID::Expanded(), R"(C:\Users\Echo\Documents\Temp\Icons\展开.png)");
            // new_tex->SetAssetPath("Assets/Texture/UIAtlas.png");
            // new_tex->Download();
            // AssetDataBase::CreateAsset(new_tex, new_tex->GetAssetPath());
            // AssetDataBase::Import("Assets/Texture/UIAtlas.png");
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
        RenderContext::GetByRef().SetRenderPipeline(MakeUnique<ElbowEngineRenderPipeline>());
        NewObject<ACameraHolder>();
        auto w = NewObject<Window>();
        w->SetLocation({0, 0});
        w->SetSize({200, 200});
        const auto handle = TickEvents::Evt_WorldPostTick.AddBind(&TickManagerUpdate);
        while (true) {
            ProfileScope _("Tick");
            GetWorldClock().TickAll();
            if (main_window->ShouldClose()) {
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
