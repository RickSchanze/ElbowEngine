/**
 * @file EngineApplication.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "EngineApplication.h"

#include "CoreEvents.h"
#include "GameObject/GameObject.h"
#include "GLFW/glfw3.h"
#include "Path/Path.h"
#include "Render/RenderContext.h"
#include "RHI/Vulkan/Application.h"

#include "Utils/ContainerUtils.h"

#include "Component/Camera.h"
#include "Component/Light/Light.h"
#include "Component/Mesh/StaticMesh.h"
#include "EditorStyle/ImGuiStyle.h"
#include "Render/Material.h"
#include "ResourceManager.h"
#include "UI/Window/DebugWindow.h"
#include "UI/Window/DetailWindow.h"
#include "UI/Window/OutlineWindow.h"
#include "UI/Window/WindowBase.h"
#include "UI/Window/WindowManager.h"

#include "Window/GLFWWindow.h"

#include <imgui.h>


namespace Function::Comp
{
class StaticMesh;
}
TOOL_NAMESPACE_BEGIN

EngineApplication::EngineApplication(const String& project_path, const String& window_title)
{
    Path::SetProjectWorkPath(project_path);
    if (instance_ != nullptr)
    {
        throw Exception(L"窗口实例已存在");
    }
    window_title_ = window_title;
    instance_     = this;
    editor_style_ = new DeepDarkStyle();
}

EngineApplication::~EngineApplication()
{
    delete editor_style_;
    Finitialize();
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
    // TODO: Log for init
    LOG_INFO_CATEGORY(Engine, L"初始化引擎...");
    LOG_INFO_CATEGORY(Engine.Graphics, L"最大DynamicUniformBuffer数量 = {}", g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count);
    LOG_INFO_CATEGORY(Engine.Graphics, L"交换链图像数量 = {}", g_engine_statistics.graphics.swapchain_image_count);
    LOG_INFO_CATEGORY(Engine.Graphics, L"并行渲染帧数量 = {}", g_engine_statistics.graphics.parallel_render_frame_count);
    LOG_INFO_CATEGORY(Engine.Graphics, L"最大直射光源数量 = {}", g_engine_statistics.graphics.max_point_light_count);
}

void EngineApplication::LogEndInit()
{
    LOG_INFO_CATEGORY(Engine, L"引擎初始化完成");
}

void EngineApplication::Initialize()
{
    // 创建并初始化GlfwWindow
    LogBeginInit();
    window_ = MakeUnique<Platform::Window::GlfwWindow>(window_title_, 1920, 1080);
    window_->Initialize();
    window_->SetFrameBufferResizedCallback(&ThisClass::FrameBufferResizeCallback);
    // 创建并初始化VulkanApplication
    // 设置初始化RenderApplication需要的值
    render_application_ = MakeUnique<RHI::Vulkan::VulkanApplication>();
    auto Surface        = window_->GetWindowSurface();
    render_application_->SetWindowSurface(Move(Surface));
    render_application_->SetExtensions(window_->GetRequiredExtensions());

    render_application_->Initialize();
    window_->InitImGui(render_application_->GetContext());
    editor_style_->SetStyle();
    render_context_ = new Function::RenderContext();

    camera_object_ = New<Function::GameObject>(L"摄像机", nullptr);
    camera_object_->BeginPlay();
    camera_object_->AddComponent<Function::Comp::Camera>();
    New<Function::GameObject>(L"Dummy对象", camera_object_);
    auto mesh_obj  = New<Function::GameObject>(L"AK-47_1");
    auto mesh_comp = mesh_obj->AddComponent<Function::Comp::StaticMesh>();
    mesh_comp->SetMesh(L"Models/AK47/AK47_CS2.fbx");

    auto mesh_obj2  = New<Function::GameObject>(L"AK-47_2");
    auto mesh_comp2 = mesh_obj2->AddComponent<Function::Comp::StaticMesh>();
    mesh_comp2->SetMesh(L"Models/AK47/AK47_CS2.fbx");

    auto* mat = Function::MaterialManager::CreateMaterials(L"Shaders/vert.spv", L"Shaders/frag.spv", L"AK-47材质");

    mesh_comp->SetMaterial(mat);
    mesh_comp2->SetMaterial(mat);

    mat->SetTexture("texSampler", L"Models/AK47/ak47_default_color_psd_5b66a23b.png");

    New<Function::GameObject>(L"对象3", New<Function::GameObject>(L"对象4", New<Function::GameObject>(L"对象5")));

    auto* light_obj = New<Function::GameObject>(L"点光源");
    light_obj->GetTransform().position = Vector3(0, 0, 10);
    light_obj->AddComponent<Function::Comp::Light>();
    LogEndInit();
}

void EngineApplication::Finitialize() const
{
    OnAppExit.Broadcast();
    if (!IsValid()) return;
    delete render_context_;
    RHI::Vulkan::VulkanContext::Get()->PreVulkanDeviceDestroyed.Broadcast();
    window_->ShutdownImGui();
    // vulkan device失效前释放所有资产
    Resource::ResourceManager::Get()->DestroyAllResources();
    if (render_application_->IsValid()) render_application_->Finalize();
    if (window_->IsValid()) window_->Finalize();
}

void EngineApplication::Run()
{
    while (!glfwWindowShouldClose(window_->GetGLFWWindowHandle()))
    {
        // Tick逻辑

        InternalTick();
        window_->Tick(g_engine_statistics.time_delta);
        camera_object_->Tick(g_engine_statistics.time_delta);

        // Tick渲染
        ASSERT_CATEGORY(Vulkan.Render, render_context_ != nullptr, "RenderContext未初始化");
        render_context_->PrepareFrameRender();
        window_->BeginImGuiFrame();

        for (auto& sub_window: sub_windows_)
        {
            sub_window->Tick(g_engine_statistics.time_delta);
        }

        DrawAppUI();
        g_engine_statistics.ResetDrawCalls();
        render_context_->Draw();

        render_context_->PostFrameRender();
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

void EngineApplication::RemoveWindow(Window::WindowBase* window)
{
    ContainerUtils::Remove(sub_windows_, window);
}

void EngineApplication::RemoveWindow(Type type)
{
    ContainerUtils::RemoveIf(sub_windows_, [type](const Window::WindowBase* window) {
        if (window->GetType() == type)
        {
            return true;
        }
        return false;
    });
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
        ImGui::EndMenu();
    }
}

template<typename T>
    requires std::is_base_of_v<Window::WindowBase, T>
void OpenWindow()
{
    auto window = WindowManager::GetWindow<T>();
    if (window)
    {
        window->SetVisible(Window::EWindowVisiable::Visiable);
    }
}

void EngineApplication::OnOpenStatisticsWindow()
{
    OpenWindow<Window::StatisticsWindow>();
}

void EngineApplication::OnOpenOutlineWindow()
{
    OpenWindow<Window::OutlineWindow>();
}

void EngineApplication::OnOpenDetailWindow()
{
    OpenWindow<Window::DetailWindow>();
}

TOOL_NAMESPACE_END
