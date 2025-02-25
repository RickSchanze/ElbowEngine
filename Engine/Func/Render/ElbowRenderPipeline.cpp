//
// Created by Echo on 25-1-4.
//

#include "ElbowRenderPipeline.h"

#include "Core/Async/Execution/SyncGroup.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Func/UI/UIManager.h"
#include "Func/UI/Widget/Panel.h"
#include "Func/UI/Widget/WindowPanel.h"
#include "Misc.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/Window/WindowManager.h"
#include "RenderContext.h"
#include "RenderTexture.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Mesh/Mesh.h"

using namespace resource;
using namespace core;
using namespace platform::rhi;
using namespace exec;

void func::ElbowRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params) {
  PROFILE_SCOPE_AUTO;
  auto view = GetBackBufferView(params.current_image_index);
  auto image = GetBackBuffer(params.current_image_index);
  cmd.Begin();

  auto w = platform::WindowManager::GetMainWindow();
  Rect2D rect{};
  rect.size = {w->GetWidth(), w->GetHeight()};
  cmd.Enqueue<Cmd_SetScissor>(rect);
  cmd.Enqueue<Cmd_SetViewport>(rect);
  cmd.Execute("WindowResize");

  ImageSubresourceRange range{};
  range.aspect_mask = IA_Color;
  range.base_array_layer = 0;
  range.base_mip_level = 0;
  range.layer_count = 1;
  range.level_count = 1;
  cmd.Enqueue<Cmd_ImagePipelineBarrier>(ImageLayout::Undefined, ImageLayout::ColorAttachment, image, range, 0,
                                        AFB_ColorAttachmentWrite, PSFB_ColorAttachmentOutput,
                                        PSFB_ColorAttachmentOutput);
  RenderAttachment attachment{};
  attachment.clear_color = Color::DefaultClear();
  attachment.target = view;
  attachment.layout = ImageLayout::ColorAttachment;
  PooledArray<RenderAttachment> attachments = MakePooledArray<RenderAttachment>(MEMORY_POOL_ID_CMD);
  attachments.push_back(attachment);
  RenderAttachment depth_attachment{};
  depth_attachment.clear_color.r = 1.0f;
  depth_attachment.layout = ImageLayout::DepthStencilAttachment;
  depth_attachment.target = depth_target_->GetImageView();
  cmd.Enqueue<Cmd_BeginRender>(attachments, depth_attachment);
  BindMaterial(cmd, material_);
  BindAndDrawMesh(cmd, mesh_);
  ui::UIManager::Draw(cmd);
  cmd.Enqueue<Cmd_EndRender>();
  cmd.Enqueue<Cmd_ImagePipelineBarrier>(ImageLayout::ColorAttachment, ImageLayout::PresentSrc, image, range,
                                        AFB_ColorAttachmentWrite, 0, PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
  cmd.Execute("Draw Cube Mesh");
  cmd.End();
}

void func::ElbowRenderPipeline::Build() {
  auto obj_shader = AssetDataBase::LoadAsync("Assets/Shader/SimpleSampledShader.slang");
  auto fbx = AssetDataBase::LoadAsync("Assets/Mesh/Cube.fbx");
  auto ui_shader = AssetDataBase::LoadAsync("Assets/Shader/UIPanel.slang");
  auto ui_atlas = AssetDataBase::LoadAsync("Assets/Texture/UIAtlas.png");
  MakeSyncGroup(obj_shader, fbx, ui_shader, ui_atlas)->OnCompleted([this](const auto &res) {
    auto &[obj_shader_handle, fbx_handle, ui_shader_handle, ui_atlas_panel] = res;
    this->mesh_ = ObjectManager::GetObjectByHandle<Mesh>(fbx_handle);
    auto ui_shader_obj = ObjectManager::GetObjectByHandle<Shader>(ui_shader_handle);
    auto obj_shader_obj = ObjectManager::GetObjectByHandle<Shader>(obj_shader_handle);
    auto obj_mat = ObjectManager::CreateNewObjectAsync<Material>()->GetValue().GetValue() | First;
    obj_mat->SetShader(obj_shader_obj);
    this->material_ = obj_mat;
    auto panel_widget = ObjectManager::CreateNewObject<ui::widget::WindowPanel>();
    auto panel_mat = ObjectManager::CreateNewObject<Material>();
    auto ui_atlas_png = ObjectManager::GetObjectByHandle<Texture2D>(ui_atlas_panel);
    panel_mat->SetShader(ui_shader_obj);
    panel_mat->SetTexture2D("atlas", ui_atlas_png);
    panel_widget->SetMaterial(panel_mat);
    this->ready_ = material_ && mesh_;
  });
  depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
}

void func::ElbowRenderPipeline::Clean() {
  depth_target_ = nullptr;
  ready_ = false;
  mesh_ = nullptr;
  material_ = nullptr;
}

bool func::ElbowRenderPipeline::IsReady() const { return ready_; }

void func::ElbowRenderPipeline::OnWindowResized(platform::Window *window, Int32 width, Int32 height) {
  if (width == 0 || height == 0)
    return;
  depth_target_->Resize(width, height);
}