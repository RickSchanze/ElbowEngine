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
#include "Func/Config/FuncConfig.hpp"
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
using namespace NRHI;

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
    // try
    // {
        VLOG_INFO("正在初始化...");
        // 让std::wcout 顺利运行
        setlocale(LC_ALL, "zh_CN");
        // 让spdlog不产生乱码
        SetConsoleOutputCP(65001);
        if (!Path::SetProjectPath("C:/Users/kita/Documents/Projects/ElbowEngine/Content"))
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
        }
        // 资产数据库初始化
        {
            ProfileScope _("AssetDataBase Initialize");
            AssetDataBase::Get();

#if WITH_EDITOR
            ObjectManager::SaveObjectRegistry();
#endif
        }
        // 其他初始化
        {
            ProfileScope _("Other Initialize");
            ThreadManager::Get();
        }
        VLOG_INFO("初始化完成!");
        const auto ManagerUpdateHandle = TickEvents::Evt_WorldPostTick.AddBind(&TickManagerUpdate);
        PlatformWindow *MainWindow = PlatformWindowManager::GetMainWindow();
        TickEvents::Evt_TickInput.Bind(MainWindow, &PlatformWindow::PollInputs);
        const auto* MyFuncConfig = GetConfig<::FuncConfig>();
        RenderContext::GetByRef().SetRenderPipeline(MyFuncConfig->GetRenderPipelineName());
        while (true)
        {
            CPU_PROFILING_SCOPE_N("Tick");
            GetWorldClock().TickAll(MainWindow);
            if (MainWindow->ShouldClose())
            {
                GetGfxContextRef().WaitForDeviceIdle();
                MainWindow->Close();
                break;
            }
        }
        TickEvents::Evt_WorldPostTick.RemoveBind(ManagerUpdateHandle);
        GetGfxContext()->WaitForDeviceIdle();
        TickEvents::Evt_TickInput.Unbind();
        VLOG_INFO("关闭引擎, 清理资源...");
        MManager::Get()->Shutdown();
    // }
    // catch (const std::exception &e)
    // {
    //     VLOG_FATAL("程序因为异常崩溃了! ", e.what());
    //     return -1;
    // }
}