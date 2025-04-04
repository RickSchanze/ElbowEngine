//
// Created by Echo on 2025/4/2.
//

#include "PBRRenderPipeline.hpp"

#include "Core/Math/Math.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Func/Render/RenderTexture.hpp"
#include "Func/UI/UiManager.hpp"
#include "Func/World/StaticMeshComponent.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/VertexLayout.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"

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
    {
        cmd.BeginDebugLabel("BasePass");
        cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, hdr_color_->GetImage(), range, 0, AFB_ColorAttachmentWrite,
                                 PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
        Array<RenderAttachment> attachments{};
        RenderAttachment attachment{};
        attachment.clear_color = Color::Clear();
        attachment.target = hdr_color_->GetImageView();
        attachment.layout = ImageLayout::ColorAttachment;
        attachments.Add(attachment);
        RenderAttachment depth_attachment{};
        depth_attachment.clear_color.r = 1.0f;
        depth_attachment.layout = ImageLayout::DepthStencilAttachment;
        depth_attachment.target = depth_target_->GetImageView();
        cmd.BeginRender(attachments, depth_attachment);
        {
            cmd.BeginDebugLabel("MeshDraw");
            helper::BindMaterial(cmd, basepass_material_);
            for (auto &mesh: RenderContext::GetDrawStaticMesh()) {
                auto first_instance_index = GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(mesh->GetHandle());
                auto index_count = mesh->GetIndexCount();
                cmd.BindVertexBuffer(mesh->GetVertexBuffer());
                cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), sizeof(InstancedData1) * first_instance_index, 1);
                cmd.BindIndexBuffer(mesh->GetIndexBuffer());
                cmd.DrawIndexed(index_count, 1, 0);
            }
            cmd.EndDebugLabel();
        }

        {
            cmd.BeginDebugLabel("SkyspherePass");
            helper::BindMaterial(cmd, skysphere_pass_material_);
            // draw skybox
            cmd.EndDebugLabel();
        }
        cmd.EndRender();
        cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::ShaderReadOnly, hdr_color_->GetImage(), range, AFB_ColorAttachmentWrite,
                                 0, PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
        cmd.EndDebugLabel();
        cmd.Execute();
    }
    {
        cmd.BeginDebugLabel("ColorTransformPass");
        cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, image, range, 0, AFB_ColorAttachmentWrite,
                                 PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
        Array<RenderAttachment> attachments{};
        RenderAttachment attachment{};
        attachment.clear_color = Color::Clear();
        attachment.target = view;
        attachment.layout = ImageLayout::ColorAttachment;
        attachments.Add(attachment);
        cmd.BeginRender(attachments);
        helper::BindMaterial(cmd, color_transform_pass_material_);
        cmd.BindVertexBuffer(screen_quad_vertex_buffer_);
        cmd.BindIndexBuffer(screen_quad_index_buffer_);
        cmd.DrawIndexed(6, 1, 0);
        cmd.EndRender();
        cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::PresentSrc, image, range, AFB_ColorAttachmentWrite, 0,
                                 PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
        cmd.EndDebugLabel();
        cmd.Execute();
    }
    cmd.End();
    cmd.Execute();
}

void PBRRenderPipeline::Build() {
    ProfileScope _("RenderPipeline::Build");
    auto basepass_shader = AssetDataBase::LoadAsync("Assets/Shader/PBR/BasePass.slang");
    auto skyspere_shader = AssetDataBase::LoadAsync("Assets/Shader/PBR/Sk"
                                                    "yspherePass.slang");
    auto colortransform_shader = AssetDataBase::LoadAsync("Assets/Shader/PBR/ColorTransformPass.slang");
    auto skysphere_texture = AssetDataBase::LoadAsync("Assets/Texture/poly_haven_studio_1k.exr");
    ThreadManager::WhenAllExecFuturesCompleted(
            NamedThread::Game,
            [this](const ObjectHandle basepass_shader_handle, const ObjectHandle skysphere_shader_handle,
                   const ObjectHandle color_transform_shader_handle, const ObjectHandle skysphere_texture_handle) {
                ProfileScope _("RenderPipeline::Build(OnAsyncCompleted)");
                const Shader *baspass_shader = ObjectManager::GetObjectByHandle<Shader>(basepass_shader_handle);
                const Shader *skysphere_pass_shader = ObjectManager::GetObjectByHandle<Shader>(skysphere_shader_handle);
                const Shader *color_transform_pass_shader = ObjectManager::GetObjectByHandle<Shader>(color_transform_shader_handle);
                const Texture2D *skysphere_texture = ObjectManager::GetObjectByHandle<Texture2D>(skysphere_texture_handle);

                basepass_material_ = ObjectManager::CreateNewObject<Material>();
                skysphere_pass_material_ = ObjectManager::CreateNewObject<Material>();
                color_transform_pass_material_ = ObjectManager::CreateNewObject<Material>();

                basepass_material_->SetShader(baspass_shader);
                skysphere_pass_material_->SetShader(skysphere_pass_shader);
                color_transform_pass_material_->SetShader(color_transform_pass_shader);

                depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
                auto *w = PlatformWindowManager::GetMainWindow();
                UInt64 width = w->GetWidth();
                UInt64 height = w->GetHeight();
                ImageDesc desc{width, height, IUB_RenderTarget | IUB_ShaderRead, Format::R32G32B32A32_Float};
                hdr_color_ = MakeShared<RenderTexture>(desc);

                hdr_color_->BindToMaterial("tex", color_transform_pass_material_);
                skysphere_pass_material_->SetTexture2D("skybox_texture", skysphere_texture);
                color_transform_pass_material_->SetFloat("param.exposure", 3);

                // 创建screen space quad
                BufferDesc buffer_desc{4 * sizeof(Vertex1), BUB_VertexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
                screen_quad_vertex_buffer_ = GetGfxContextRef().CreateBuffer(buffer_desc);
                // vulkan的屏幕空间的四个顶点 TODO: GL和这个不一样
                Vertex1 vertices[] = {
                    {{-1.0f, -1.0f, 0.0f}, {0, 0, 0}, {0, 0}},
                    {{1.0f, -1.0f, 0.0f}, {0, 0, 0}, {1, 0}},
                    {{-1.0f, 1.0f, 0.0f}, {0, 0, 0}, {0, 1}},
                    {{1.0f, 1.0f, 0.0f}, {0, 0, 0}, {1, 1}}
                };
                GfxCommandHelper::CopyDataToBuffer(&vertices, screen_quad_vertex_buffer_.get(), 4 * sizeof(Vertex1));
                BufferDesc index_buffer_desc{6 * sizeof(UInt32), BUB_IndexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
                screen_quad_index_buffer_ = GetGfxContextRef().CreateBuffer(index_buffer_desc);
                UInt32 indices[] = {0, 2, 1, 1, 2, 3};
                GfxCommandHelper::CopyDataToBuffer(&indices, screen_quad_index_buffer_.get(), 6 * sizeof(UInt32));
                ready_ = true;
            },
            Move(basepass_shader), Move(skyspere_shader), Move(colortransform_shader), Move(skysphere_texture));
}

void PBRRenderPipeline::Clean() {
    depth_target_ = nullptr;
    ready_ = false;
    basepass_material_ = nullptr;
}

bool PBRRenderPipeline::IsReady() const { return ready_; }

void PBRRenderPipeline::OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) {
    if (width == 0 || height == 0)
        return;
    depth_target_->Resize(width, height);
    hdr_color_->Resize(width, height);
}
