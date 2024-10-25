/**
 * @file EngineApplication.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "EngineApplication.h"
#include "Profiler/ProfileMacro.h"

#include "CoreEvents.h"
#include "GameObject/GameObject.h"
#include "GLFW/glfw3.h"
#include "Path/Path.h"
#include "Render/RenderContext.h"
#include "RHI/Vulkan/Application.h"

#include "Utils/ContainerUtils.h"

#include "Async/Coroutine/CoroutineExecutorManager.h"
#include "Async/Coroutine/MainThreadExecutor.h"
#include "Component/Camera.h"
#include "Component/FunctionTestComponent.h"
#include "Component/Light/Light.h"
#include "Component/Mesh/StaticMesh.h"
#include "Editor/Window/ConsoleWindow.h"
#include "Editor/Window/DebugWindow.h"
#include "Editor/Window/DetailWindow.h"
#include "Editor/Window/ImGuiDemoWindow.h"
#include "Editor/Window/LightSettingWindow.h"
#include "Editor/Window/OutlineWindow.h"
#include "Editor/Window/SceneViewportWindow.h"
#include "Editor/Window/WindowBase.h"
#include "Editor/Window/WindowManager.h"
#include "EditorStyle/ImGuiStyle.h"
#include "Render/Materials/Material.h"
#include "ResourceManager.h"

#include "Window/GLFWWindow.h"
#include "World/TickManager.h"

#include <imgui.h>


namespace tool
{
EngineApplication::EngineApplication(const String& project_path, const String& window_title)
{
    Path::SetProjectWorkPath(project_path);
    if (instance_ != nullptr)
    {
        throw Exception(L"窗口实例已存在");
    }
    window_title_ = window_title;
    instance_     = this;
    editor_style_ = New<DeepDarkStyle>();
}

EngineApplication::~EngineApplication()
{
    Delete(editor_style_);
    DeInitialize();
}

EngineApplication& EngineApplication::Instance()
{
    if (!instance_)
    {
        throw Exception(L"EngineApplication未初始化");
    }
    return *instance_;
}

Size2D EngineApplication::GetWindowSize() const
{
    return window_->GetWindowSize();
}

void EngineApplication::LogBeginInit()
{
    LOG_INFO_CATEGORY(Engine, L"初始化引擎...");
    LOG_INFO_CATEGORY(Engine.Graphics, L"最大DynamicUniformBuffer数量 = {}", g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count);
    LOG_INFO_CATEGORY(Engine.Graphics, L"交换链图像数量 = {}", g_engine_statistics.graphics.swapchain_image_count);
    LOG_INFO_CATEGORY(Engine.Graphics, L"并行渲染帧数量 = {}", g_engine_statistics.graphics.parallel_render_frame_count);
    LOG_INFO_CATEGORY(Engine.Graphics, L"最大直射光源数量 = {}", g_engine_statistics.graphics.max_point_light_count);
}

void EngineApplication::LogEndInit()
{
#ifdef ENABLE_TEST
    LOG_INFO_ANSI_CATEGORY(Engine, "测试:启用");
#endif
#ifdef ENABLE_PROFILING
    LOG_INFO_ANSI_CATEGORY(Engine, "Profiling: 启用");
#endif
#if WITH_EDITOR
    LOG_INFO_ANSI_CATEGORY(Engine, "Editor: 启用");
#endif
    LOG_INFO_CATEGORY(Engine, L"引擎初始化完成");
}

static void OnAppRequiredWindowSizeCallback(int* w, int* h)
{
    auto window = EngineApplication::Get().GetWindowSize();
    *w          = window.width;
    *h          = window.height;
}

static void RegisterEvents()
{
    OnGetAppWindowSize.Bind(OnAppRequiredWindowSizeCallback);
}

void EngineApplication::Initialize()
{
    // 创建并初始化GlfwWindow
    LogBeginInit();
    InitializeCoroutineContext();
    window_ = MakeUnique<platform::window::GlfwWindow>(window_title_, 1920, 1080);
    window_->Initialize();
    window_->SetFrameBufferResizedCallback(&ThisClass::FrameBufferResizeCallback);
    // 创建并初始化VulkanApplication
    // 设置初始化RenderApplication需要的值
    RegisterEvents();
    render_application_ = MakeUnique<rhi::vulkan::VulkanApplication>();
    auto surface        = window_->GetWindowSurface();
    render_application_->SetWindowSurface(Move(surface));
    render_application_->SetExtensions(window_->GetRequiredExtensions());

    render_application_->Initialize();
#ifdef USE_IMGUI
    window_->InitImGui(render_application_->GetContext());
#endif
    editor_style_->SetStyle();
    render_context_ = New<function::RenderContext>();

    camera_object_ = NewObject<function::GameObject>(L"摄像机", nullptr);

    camera_object_->AddComponent<function::comp::Camera>()->draw_skybox = false;

    auto* light_obj = NewObject<function::GameObject>(L"点光源");
    light_obj->GetTransform().SetPosition(Vector3(0, 100, 0));
    light_obj->AddComponent<function::comp::Light>();

    NewObject<function::GameObject>(L"AK")
        ->AddComponent<function::comp::StaticMesh>()
        ->SetMesh(L"Models/AK47/AK47_CS2.fbx")
        .SetMaterial(&function::MaterialManager::CreateMaterial(L"Shaders/Shader.vert", L"Shaders/Shader.frag", L"AK47Mat")
                          ->SetTexture("texSampler", L"Models/AK47/ak47_default_color_psd_5b66a23b.png"));

#ifdef ENABLE_TEST
    FunctionalityTest();
#endif

    LogEndInit();
}

void EngineApplication::DeInitialize() const
{
    OnAppExit.InvokeOnce();
    if (!IsValid()) return;
    Delete(render_context_);
    rhi::vulkan::VulkanContext::Get()->OnPreVulkanDeviceDestroyed.InvokeOnce();
#ifdef USE_IMGUI
    window_->ShutdownImGui();
#endif
    // vulkan device失效前释放所有资产
    res::ResourceManager::Get()->DestroyAllResources();
    if (render_application_->IsValid()) render_application_->Finalize();
    if (window_->IsValid()) window_->Finalize();
}

void EngineApplication::InitializeCoroutineContext()
{
    using namespace async::coro;
    auto* executor = New<MainThreadExecutor>();
    CoroutineExecutorManager::Get()->RegisterExecutor(EExecutorType::MainThread, executor);
    LOG_INFO_ANSI_CATEGORY(Engine, "初始化主线程协程运行环境");
}

void EngineApplication::Run()
{
    while (!glfwWindowShouldClose(window_->GetGLFWWindowHandle()))
    {
        MARK_FRAME_AUTO;
        // FrameMarkNamed("ElbowEngine Frame");
        PROFILE_SCOPE_AUTO;
        // TODO: Tick完全由TickManager管理
        // Tick逻辑
        {
            PROFILE_SCOPE("Tick Logic");
            InternalTick();
            window_->Tick(g_engine_statistics.time_delta);
            // Tick逻辑
            function::TickManager::Get()->PerformTickLogic();
        }

        // Tick渲染
        Assert(Vulkan.Render, render_context_ != nullptr, "RenderContext未初始化");
        if (render_context_->CanRender())
        {
            PROFILE_SCOPE("Tick Render");
            render_context_->PrepareFrameRender();
#ifdef USE_IMGUI
            window_->BeginImGuiFrame();
#endif
            {
                PROFILE_SCOPE("Tick Editor Window");
                window::WindowManager::Get()->DrawVisibleWindows(g_engine_statistics.time_delta);
                DrawAppUI();
            }
            g_engine_statistics.ResetDrawCalls();
            {
                PROFILE_SCOPE("Tick Scene Object");
                render_context_->Draw(render_context_->CanRenderBackbuffer());
                render_context_->PostFrameRender();
            }
        }
    }
}

bool EngineApplication::IsValid() const
{
    if (!(render_application_ && render_application_->IsValid())) return false;
    if (!(window_ && window_->IsValid())) return false;
    return true;
}

void EngineApplication::InternalTick()
{
    using namespace std::chrono;
    glfwSetInputMode(window_->GetGLFWWindowHandle(), GLFW_CURSOR, g_engine_statistics.is_hide_mouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    {
        auto current_frame_time        = steady_clock::now();
        g_engine_statistics.time_delta = duration<float>(current_frame_time - last_frame_time_).count();
        last_frame_time_               = current_frame_time;
        if (g_engine_statistics.time_delta > 0)   //
            g_engine_statistics.fps = static_cast<int32_t>(1.f / g_engine_statistics.time_delta);
    }
    g_engine_statistics.frame_count++;

    g_engine_statistics.object_count = ObjectManager::Get()->GetObjectCount();
}

// TODO: 更加可拓展的主窗口形式
void EngineApplication::DrawAppUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawWindowMenu();
        ImGui::EndMainMenuBar();
    }
}

void EngineApplication::DrawWindowMenu()
{
    if (ImGui::BeginMenu(U8("窗口")))
    {
        if (ImGui::MenuItem(U8("引擎数据统计")))
        {
            OnOpenStatisticsWindow();
        }
        if (ImGui::MenuItem(U8("大纲")))
        {
            OnOpenOutlineWindow();
        }
        if (ImGui::MenuItem(U8("细节")))
        {
            OnOpenDetailWindow();
        }
        if (ImGui::MenuItem(U8("ImGuiDemo")))
        {
            OnOpenImGuiDemoWindow();
        }
        if (ImGui::MenuItem(U8("场景")))
        {
            OnOpenSceneWindow();
        }
        if (ImGui::MenuItem(U8("光照设置")))
        {
            OnOpenLightSettingWindow();
        }
        if (ImGui::MenuItem(U8("控制台")))
        {
            OnOpenConsoleWindow();
        }
        ImGui::EndMenu();
    }
}

template<typename T>
    requires std::is_base_of_v<window::WindowBase, T>
void OpenWindow()
{
    auto window = window::WindowManager::GetOrCreateWindow<T>();
    if (window)
    {
        window->SetVisible(window::EWindowVisibility::Visible);
    }
}

void EngineApplication::OnOpenStatisticsWindow()
{
    OpenWindow<window::StatisticsWindow>();
}

void EngineApplication::OnOpenOutlineWindow()
{
    OpenWindow<window::OutlineWindow>();
}

void EngineApplication::OnOpenDetailWindow()
{
    OpenWindow<window::DetailWindow>();
}

void EngineApplication::OnOpenImGuiDemoWindow()
{
    OpenWindow<window::ImGuiDemoWindow>();
}

void EngineApplication::OnOpenSceneWindow()
{
    OpenWindow<window::SceneViewportWindow>();
}

void EngineApplication::OnOpenLightSettingWindow()
{
    OpenWindow<window::LightSettingWindow>();
}

void EngineApplication::OnOpenConsoleWindow()
{
    OpenWindow<window::ConsoleWindow>();
}
#ifdef ENABLE_TEST
void EngineApplication::FunctionalityTest()
{
    NewObject<function::GameObject>(L"功能测试用GameObject")->AddComponent<function::comp::FunctionTestComponent>();
}
#endif

}
