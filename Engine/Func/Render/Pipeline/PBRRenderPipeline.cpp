//
// Created by Echo on 2025/4/2.
//

#include "PBRRenderPipeline.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Func/Render/RenderTexture.hpp"
#include "Func/UI/UiManager.hpp"
#include "Func/World/StaticMeshComponent.hpp"
#include "Platform/RHI/VertexLayout.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Shader/Shader.hpp"

using namespace rhi;

void PBRRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params) {
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
    attachment.clear_color = Color::Clear();
    attachment.target = view;
    attachment.layout = ImageLayout::ColorAttachment;
    Array<RenderAttachment> attachments{};
    attachments.Add(attachment);
    RenderAttachment depth_attachment{};
    depth_attachment.clear_color.r = 1.0f;
    depth_attachment.layout = ImageLayout::DepthStencilAttachment;
    depth_attachment.target = depth_target_->GetImageView();
    cmd.BeginRender(attachments, depth_attachment);
    cmd.Execute();
    {
        cmd.BeginDebugLabel("DrawAMesh");
        // draw mesh

        helper::BindMaterial(cmd, material_);
        for (auto &mesh: RenderContext::GetDrawStaticMesh()) {
            auto first_instance_index = GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(mesh->GetHandle());
            auto index_count = mesh->GetIndexCount();
            cmd.BindVertexBuffer(mesh->GetVertexBuffer());
            cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), sizeof(InstancedData1) * first_instance_index, 1);
            cmd.BindIndexBuffer(mesh->GetIndexBuffer());
            cmd.DrawIndexed(index_count, 1, 0);
        }
        cmd.EndDebugLabel();
        cmd.Execute();
    }

    cmd.EndRender();
    cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::PresentSrc, image, range, AFB_ColorAttachmentWrite, 0,
                             PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
    cmd.Execute();
    cmd.End();
}

void PBRRenderPipeline::Build() {
    auto obj_shader = AssetDataBase::LoadAsync("Assets/Shader/SimpleSampledShader.slang");
    ThreadManager::WhenAllExecFuturesCompleted(
            NamedThread::Game,
            [this](const ObjectHandle obj_shader_handle) {
                const Shader *obj_shader_obj = ObjectManager::GetObjectByHandle<Shader>(obj_shader_handle);
                auto *obj_mat = ObjectManager::CreateNewObject<Material>();
                obj_mat->SetShader(obj_shader_obj);
                this->material_ = obj_mat;
                this->depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
                this->ready_ = true;
            },
            Move(obj_shader));
}

void PBRRenderPipeline::Clean() {
    depth_target_ = nullptr;
    ready_ = false;
    material_ = nullptr;
}

bool PBRRenderPipeline::IsReady() const { return ready_; }

void PBRRenderPipeline::OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) {
    if (width == 0 || height == 0)
        return;
    depth_target_->Resize(width, height);
}
