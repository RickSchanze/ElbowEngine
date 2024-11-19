#define GLFW_INCLUDE_VULKAN
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Memory/FrameAllocator.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/YamlArchive.h"
#include "Platform/FileSystem/Path.h"

struct B
{
    B(int) {}
};

int main()
{
    SetRuntimeStage(RuntimeStage::Startup);
    LOGGER.Info(logcat::Engine, "Initializing Engine...");
    Assert(logcat::Engine, ValidateFeatureState(), "Feature validation failed, abort program.");
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    if (!platform::Path::SetProjectPath("C:/Users/Echo/SyncWork/Work/Projects/ElbowEngine/Content"))
    {
        LOGGER.Critical(logcat::Core, "Set project path failed, abort program.");
        return -1;
    }
    core::FrameAllocator::Startup();
    LOGGER.Info(logcat::Engine, "Engine initialized.");
    SetRuntimeStage(RuntimeStage::Running);
    LOGGER.Info(logcat::Engine, "Engine running...");
    while (GetRuntimeStage() != RuntimeStage::Shutdown)
    {
        MARK_FRAME_AUTO;
        core::FrameAllocator::Refresh();
    }
    LOGGER.Info(logcat::Engine, "Engine shutdown...");
    SetRuntimeStage(RuntimeStage::Shutdown);
    core::FrameAllocator::Shutdown();
}
