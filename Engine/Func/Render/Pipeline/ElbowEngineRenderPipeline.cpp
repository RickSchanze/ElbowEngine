//
// Created by Echo on 2025/3/27.
//

#include "ElbowEngineRenderPipeline.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/Render/RenderTexture.hpp"
#include "Func/UI/UIManager.hpp"
#include "Func/UI/ViewportWindow.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"

using namespace RHI;
using namespace Helper;

void ElbowEngineRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params) {
    ProfileScope scope(__func__);
    const auto view = GetBackBufferView(params.current_image_index);
    auto image = GetBackBuffer(params.current_image_index);
    cmd.Begin();

    auto w = PlatformWindowManager::GetMainWindow();
    Rect2Df rect{};
    rect.size = {static_cast<Float>(w->GetWidth()), static_cast<Float>(w->GetHeight())};
    cmd.SetScissor(rect);
    cmd.SetViewport(rect);
    cmd.Execute();

    ImageSubresourceRange range{};
    range.aspect_mask = IA_Color;
    range.base_array_layer = 0;
    range.base_mip_level = 0;
    range.layer_count = 1;
    range.level_count = 1;
    cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, image, range, 0, AFB_ColorAttachmentWrite,
                             PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
    RenderAttachment attachment{};
    attachment.ClearColor = Color::Clear();
    attachment.Target = view;
    attachment.Layout = ImageLayout::ColorAttachment;
    Array<RenderAttachment> attachments{};
    attachments.Add(attachment);
    RenderAttachment depth_attachment{};
    depth_attachment.ClearColor.r = 1.0f;
    depth_attachment.Layout = ImageLayout::DepthStencilAttachment;
    depth_attachment.Target = depth_target_->GetImageView();
    cmd.BeginRender(attachments, depth_attachment);
    cmd.Execute();
    {
        cmd.BeginDebugLabel("DrawAMesh");
        BindMaterial(cmd, material_);
        BindAndDrawMesh(cmd, mesh_);
        cmd.EndDebugLabel();
        cmd.Execute();
    }

    cmd.EndRender();
    cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::PresentSrc, image, range, AFB_ColorAttachmentWrite, 0,
                             PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
    cmd.Execute();
    cmd.End();
}

void ElbowEngineRenderPipeline::Build() {
    auto obj_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/SimpleSampledShader.slang");
    auto fbx = AssetDataBase::LoadFromPathAsync("Assets/Mesh/Cube.fbx");
    ThreadManager::WhenAllExecFuturesCompleted(
            NamedThread::Game,
            [this](const ObjectHandle obj_shader_handle, const ObjectHandle fbx_handle) {
                this->mesh_ = ObjectManager::GetObjectByHandle<Mesh>(fbx_handle);
                const Shader *obj_shader_obj = ObjectManager::GetObjectByHandle<Shader>(obj_shader_handle);
                auto *obj_mat = ObjectManager::CreateNewObject<Material>();
                obj_mat->SetShader(obj_shader_obj);
                this->material_ = obj_mat;
                this->ready_ = mesh_;
                this->depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
            },
            Move(obj_shader), Move(fbx));
}

void ElbowEngineRenderPipeline::Clean() {
    depth_target_ = nullptr;
    ready_ = false;
    mesh_ = nullptr;
    material_ = nullptr;
}

bool ElbowEngineRenderPipeline::IsReady() const { return ready_; }

void ElbowEngineRenderPipeline::OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) {
    if (width == 0 || height == 0)
        return;
    depth_target_->Resize(width, height);
}
