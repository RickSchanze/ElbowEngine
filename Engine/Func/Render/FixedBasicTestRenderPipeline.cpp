//
// Created by Echo on 25-1-4.
//

#include "FixedBasicTestRenderPipeline.h"

#include "Core/Async/Execution/SyncGroup.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Func/UI/Overlay.h"
#include "Misc.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/Window/WindowManager.h"
#include "RenderContext.h"
#include "RenderTexture.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Font/Font.h"
#include "Resource/Assets/Mesh/Mesh.h"

using namespace resource;
using namespace core;
using namespace platform::rhi;
using namespace exec;

void func::FixedBasicTestRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params) {
  PROFILE_SCOPE_AUTO;
  auto view = GetBackBufferView(params.current_image_index);
  auto image = GetBackBuffer(params.current_image_index);
  cmd.Begin();

  auto w = platform::GetWindowManager().GetMainWindow();
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
  test_text_->Draw(cmd);
  cmd.Enqueue<Cmd_EndRender>();
  cmd.Enqueue<Cmd_ImagePipelineBarrier>(ImageLayout::ColorAttachment, ImageLayout::PresentSrc, image, range,
                                        AFB_ColorAttachmentWrite, 0, PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
  cmd.Execute("Draw Cube Mesh");
  cmd.End();
}

void func::FixedBasicTestRenderPipeline::Build() {
  auto a2 = AssetDataBase::LoadAsync("Assets/Mesh/Cube.fbx");
  auto s1 = AssetDataBase::LoadAsync("Assets/Shader/SimpleSampledShader.slang");
  auto s2 = AssetDataBase::LoadAsync("Assets/Shader/Text.slang");
  MakeSyncGroup(Move(a2), Move(s1), Move(s2))->OnCompleted([this](const auto &res) {
    auto &[mesh_handle, s1, s2] = res;
    mesh_ = static_cast<Mesh *>(ObjectManager::GetObjectByHandle(mesh_handle));
    material_ = New<Material>();
    font_material = ObjectManager::CreateNewObject<Material>()->GetValue().GetValue() | First;
    auto *s2_obj = static_cast<Shader *>(ObjectManager::GetObjectByHandle(s2));
    font_material->SetShader(s2_obj);
    auto *s1_obj = static_cast<Shader *>(ObjectManager::GetObjectByHandle(s1));
    material_->SetShader(s1_obj);

    test_text_ = New<ui::Overlay>();
    test_text_->SetPosition({0, 0, 0});
    test_text_->SetSize({1920, 1080});
    auto text_wdg = ObjectManager::CreateNewObject<ui::widget::Text>()->GetValue().GetValue() | First;
    test_text_->SetSlot(text_wdg);
    text_wdg->SetText("Hello World");
    if (mesh_) {
      ready_ = font_material && material_;
    }
  });
  depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
}

void func::FixedBasicTestRenderPipeline::Clean() {
  depth_target_ = nullptr;
  ready_ = false;
  mesh_ = nullptr;
  material_ = nullptr;
}

bool func::FixedBasicTestRenderPipeline::IsReady() const { return ready_; }

void func::FixedBasicTestRenderPipeline::OnWindowResized(platform::Window *window, Int32 width, Int32 height) {
  if (width == 0 || height == 0)
    return;
  depth_target_->Resize(width, height);
}