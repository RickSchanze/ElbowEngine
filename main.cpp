#define GLFW_INCLUDE_VULKAN

#include "Core/Async/Execution/FutureReceiver.h"
#include "Core/Async/Execution/Just.h"
#include "Core/Async/ThreadManager.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Object/ObjectRegistry.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/YamlArchive.h"
#include "Func/Camera/ACameraHolder.h"
#include "Func/Camera/Camera.h"
#include "Func/Input/Input.h"
#include "Func/Render/ElbowRenderPipeline.h"
#include "Func/Render/RenderContext.h"
#include "Func/UI/IconID.h"
#include "Func/UI/Widget/Panel.h"
#include "Func/UI/Widget/WindowPanel.h"
#include "Func/World/Actor.h"
#include "Func/World/WorldClock.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/Window/WindowManager.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Font/Font.h"
#include "Resource/Assets/Material/MaterialMeta.h"
#include "Resource/Assets/Material/SharedMaterial.h"
#include "Resource/Assets/Shader/Shader.h"
#include "Resource/Assets/Texture/Texture2DMeta.h"
#include "cpptrace/cpptrace.hpp"

namespace resource {
class MaterialMeta;
}
using namespace core;
using namespace exec;
using namespace func;
using namespace resource;

static void TickManagerUpdate(const Millisecond &sec) {
  static UInt32 interval = GetConfig<CoreConfig>()->GetTickFrameInterval();
  static UInt32 cnt = 0;
  cnt++;
  if (cnt >= interval) {
    SharedMaterialManager::GetByRef().UpdateSharedMaterialSet();
    platform::rhi::DescriptorSetLayoutPool::GetByRef().Update();
    cnt = 0;
  }
}

int main() {
  auto j = Just() | Then([] { return 12; }) | Then([](Int32) {});
  //   try {
  //     cpptrace::generate_trace(); // 这里需要先调用一次generate_trace 否则后面的无法生成trace
  //     SetRuntimeStage(RuntimeStage::Startup);
  //     LOGGER.Info(logcat::Engine, "Initializing Engine...");
  //     Assert::Require(logcat::Engine, ValidateFeatureState(), "Feature validation failed, abort program.");
  //     // 让std::wcout 顺利运行
  //     setlocale(LC_ALL, "zh_CN");
  //     // 让spdlog不产生乱码
  //     SetConsoleOutputCP(65001);
  //     if (!platform::Path::SetProjectPath("C:/Users/Echo/Documents/Projects/ElbowEngine/Content")) {
  //       LOGGER.Critical(logcat::Core, "Set project path failed, abort program.");
  //       return -1;
  //     }
  //     {
  //       ThreadManager::Get();
  //     }
  //     // 初始化内存
  //     {
  //       MemoryManager::Get();
  //     }
  //     // 窗口初始化
  //     {
  //       PROFILE_SCOPE("Window Initialize");
  //       CreateAWindow(platform::WindowLib::GLFW);
  //       Input::GetByRef();
  //     }
  //     // 图形初始化
  //     {
  //       PROFILE_SCOPE("Graphics Initialize");
  //       const auto rhi_cfg = GetConfig<platform::PlatformConfig>();
  //       UseGraphicsAPI(rhi_cfg->GetGraphicsAPI());
  //       platform::rhi::GfxContextLifeTimeProxyManager::Get();
  //       RenderContext::GetByRef();
  //       Camera::GetByRef();
  //     }
  //     // 资产数据库初始化
  //     {
  //       PROFILE_SCOPE("AssetDataBase Initialize");
  //       AssetDataBase::Get();
  //       Array results = {
  //           AssetDataBase::Import("Assets/Shader/Error.slang"),
  //           AssetDataBase::Import("Assets/Texture/Default.png"),
  //           AssetDataBase::Import("Assets/Mesh/Cube.fbx"),
  //           AssetDataBase::Import("Assets/Font/MapleMono.ttf"),
  //           AssetDataBase::Import("Assets/Shader/UIPanel.slang"),
  //           AssetDataBase::Import("Assets/Shader/Text.slang"),
  //           AssetDataBase::Import("Assets/Shader/SimpleSampledShader.slang"),
  //       };
  //       ThreadManager::Poll(INT_MAX);
  //       for (const auto &result : results) {
  //         ThreadManager::Poll(INT_MAX);
  //         result->Wait();
  //         ThreadManager::Poll(INT_MAX);
  //       }
  //       // 测试Texture2D的Sprite Append功能 以及CreateAsset Texture的功能
  //       // Texture2DMeta new_meta;
  //       // new_meta.dynamic = true;
  //       // new_meta.width = 1024;
  //       // new_meta.height = 1024;
  //       // new_meta.format = platform::rhi::Format::R8G8B8A8_SRGB;
  //       // Texture2D *new_tex = ObjectManager::CreateNewObject<Texture2D>();
  //       // new_tex->SetName("UIAtlas");
  //       // new_tex->Load(new_meta);
  //       // new_tex->AppendSprite(ui::IconID::Close(), R"(C:\Users\Echo\Documents\Temp\Icons\关闭.png)");
  //       // new_tex->AppendSprite(ui::IconID::White(), R"(C:\Users\Echo\Documents\Temp\Icons\白.png)");
  //       // new_tex->AppendSprite(ui::IconID::Folded(), R"(C:\Users\Echo\Documents\Temp\Icons\收起.png)");
  //       // new_tex->AppendSprite(ui::IconID::Expanded(), R"(C:\Users\Echo\Documents\Temp\Icons\展开.png)");
  //       // new_tex->SetAssetPath("Assets/Texture/UIAtlas.png");
  //       // new_tex->Download();
  //       // AssetDataBase::CreateAsset(new_tex, new_tex->GetAssetPath());
  //       AssetDataBase::Import("Assets/Texture/UIAtlas.png");
  // #if WITH_EDITOR
  //       ObjectManager::SaveObjectRegistry();
  // #endif
  //     }
  //     // 其他初始化
  //     {
  //       PROFILE_SCOPE("Other Initialize");
  //       ThreadManager::Get();
  //       Camera::Get();
  //     }
  //     LOGGER.Info(logcat::Engine, "Engine initialized.");
  //     SetRuntimeStage(RuntimeStage::Running);
  //     LOGGER.Info(logcat::Engine, "Engine running...");
  //     platform::Window *main_window = platform::WindowManager::GetMainWindow();
  //     TickEvents::InputTickEvent.Bind(main_window, &platform::Window::PollInputs);
  //     RenderContext::GetByRef().SetRenderPipeline(MakeUnique<ElbowRenderPipeline>());
  //     Actor *a = NewObject<ACameraHolder>();
  //     auto handle = TickEvents::WorldPostTickEvent.AddBind(&TickManagerUpdate);
  //     while (GetRuntimeStage() != RuntimeStage::Shutdown) {
  //       MARK_FRAME_AUTO;
  //       GetWorldClock().TickAll();
  //       if (main_window->ShouldClose()) {
  //         main_window->Close();
  //         break;
  //       }
  //     }
  //     TickEvents::WorldPostTickEvent.RemoveBind(handle);
  //     platform::rhi::GetGfxContext()->WaitForDeviceIdle();
  //     TickEvents::InputTickEvent.Unbind();
  //     LOGGER.Info(logcat::Engine, "Engine shutdown...");
  //     SetRuntimeStage(RuntimeStage::Shutdown);
  //     MManager::Get()->Shutdown();
  //   } catch (const Exception &e) {
  //     LOGGER.ErrorFast(logcat::Exception, "程序因为异常崩溃了!\n{}", e.what());
  //     return -1;
  //   }
}
