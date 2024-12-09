#define GLFW_INCLUDE_VULKAN
#include "Core/Async/Execution/Common.h"
#include "Core/Async/Execution/Just.h"
#include "Core/Async/Execution/Then.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Memory/FrameAllocator.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/YamlArchive.h"
#include "cpptrace/cpptrace.hpp"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/Window/WindowManager.h"

int main()
{
    cpptrace::generate_trace();   // 这里需要先调用一次generate_trace 否则后面的无法生成trace
    SetRuntimeStage(RuntimeStage::Startup);
    LOGGER.Info(logcat::Engine, "Initializing Engine...");
    Assert(logcat::Engine, ValidateFeatureState(), "Feature validation failed, abort program.");
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);

    auto a =
        core::exec::Just() | core::exec::Then([]() { return 15; }) | core::exec::Then([](int num) { LOGGER.Info(logcat::Test, "num: {}", num); });

    if (!platform::Path::SetProjectPath("C:/Users/Echo/SyncWork/Work/Projects/ElbowEngine/Content"))
    {
        LOGGER.Critical(logcat::Core, "Set project path failed, abort program.");
        return -1;
    }
    // 读取项目的基本配置
    core::FrameAllocator::Startup();
    // 资产数据库初始化
    {
        PROFILE_SCOPE("AssetDataBase Initialize");
    }
    // 窗口初始化
    {
        PROFILE_SCOPE("Window Initialize");
        CreateAWindow(platform::WindowLib::GLFW);
    }
    // 图形初始化
    {
        PROFILE_SCOPE("Graphics Initialize")
        auto rhi_cfg = core::GetConfig<platform::PlatformConfig>();
        platform::rhi::UseGraphicsAPI(rhi_cfg->GetGraphicsAPI());
    }
    LOGGER.Info(logcat::Engine, "Engine initialized.");
    SetRuntimeStage(RuntimeStage::Running);
    LOGGER.Info(logcat::Engine, "Engine running...");
    while (GetRuntimeStage() != RuntimeStage::Shutdown)
    {
        MARK_FRAME_AUTO;
        core::FrameAllocator::Refresh();
        platform::Window* main_window = platform::WindowManager::Get()->GetMainWindow();
        if (main_window->ShouldClose())
        {
            main_window->Close();
            break;
        }
        main_window->PollInputs();
    }
    LOGGER.Info(logcat::Engine, "Engine shutdown...");
    SetRuntimeStage(RuntimeStage::Shutdown);
    core::FrameAllocator::Shutdown();
    core::MManager::Get()->Shutdown();
}
