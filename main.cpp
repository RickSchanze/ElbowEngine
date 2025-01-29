#define GLFW_INCLUDE_VULKAN

#include "Core/Async/Execution/WhenAll.h"
#include "Core/Async/ThreadManager.h"
#include "Core/Config/ConfigManager.h"
#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Object/ObjectRegistry.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/YamlArchive.h"
#include "Func/Camera/ACameraHolder.h"
#include "Func/Camera/Camera.h"
#include "Func/Render/FixedBasicTestRenderPipeline.h"
#include "Func/Render/RenderContext.h"
#include "Func/World/Actor.h"
#include "Func/World/WorldClock.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/Window/WindowManager.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Shader/Shader.h"
#include "cpptrace/cpptrace.hpp"

using namespace core::exec;
using namespace func;

int main() {
  try {
    cpptrace::generate_trace(); // 这里需要先调用一次generate_trace 否则后面的无法生成trace
    SetRuntimeStage(RuntimeStage::Startup);
    LOGGER.Info(logcat::Engine, "Initializing Engine...");
    core::Assert::Require(logcat::Engine, ValidateFeatureState(), "Feature validation failed, abort program.");
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    if (!platform::Path::SetProjectPath("C:/Users/Echo/Documents/Projects/ElbowEngine/Content")) {
      LOGGER.Critical(logcat::Core, "Set project path failed, abort program.");
      return -1;
    }
    // 初始化内存
    {
      core::MemoryManager::Get();
    }
    // 窗口初始化
    {
      PROFILE_SCOPE("Window Initialize");
      CreateAWindow(platform::WindowLib::GLFW);
    }
    // 图形初始化
    {
      PROFILE_SCOPE("Graphics Initialize");
      const auto rhi_cfg = core::GetConfig<platform::PlatformConfig>();
      UseGraphicsAPI(rhi_cfg->GetGraphicsAPI());
      platform::rhi::GfxContextLifeTimeProxyManager::Get();
    }
    // 资产数据库初始化
    {
      PROFILE_SCOPE("AssetDataBase Initialize");
      resource::AssetDataBase::Get();
      core::Array results = {
          resource::AssetDataBase::Import("Assets/Shader/Error.slang"),
          resource::AssetDataBase::Import("Assets/Texture/Default.png"),
          resource::AssetDataBase::Import("Assets/Mesh/Cube.fbx"),
      };
      core::ThreadManager::Poll(INT_MAX);
      for (const auto &result : results) {
        result->Wait();
      }
#if WITH_EDITOR
      core::ObjectManager::SaveObjectRegistry();
#endif
    }
    // 其他初始化
    {
      PROFILE_SCOPE("Other Initialize");
      core::ThreadManager::Get();
      Camera::Get();
    }
    LOGGER.Info(logcat::Engine, "Engine initialized.");
    SetRuntimeStage(RuntimeStage::Running);
    LOGGER.Info(logcat::Engine, "Engine running...");
    platform::Window *main_window = platform::WindowManager::Get()->GetMainWindow();
    TickEvents::InputTickEvent.Bind(main_window, &platform::Window::PollInputs);
    RenderContext::GetByRef().SetRenderPipeline(core::MakeUnique<FixedBasicTestRenderPipeline>());
    Actor *a = core::NewObject<ACameraHolder>();
    while (GetRuntimeStage() != RuntimeStage::Shutdown) {
      MARK_FRAME_AUTO;
      GetWorldClock().TickAll();
      if (main_window->ShouldClose()) {
        main_window->Close();
        break;
      }
    }
    TickEvents::InputTickEvent.Unbind();
    LOGGER.Info(logcat::Engine, "Engine shutdown...");
    SetRuntimeStage(RuntimeStage::Shutdown);
    core::MManager::Get()->Shutdown();
  } catch (const core::Exception &e) {
    LOGGER.ErrorFast(logcat::Exception, "程序因为异常崩溃了!\n{}", e.what());
        return -1;
    }
}
