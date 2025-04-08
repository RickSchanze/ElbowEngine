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
#include "Func/UI/ImGuiDemoWindow.hpp"
#include "Func/UI/UIManager.hpp"
#include "Func/UI/ViewportWindow.hpp"
#include "Func/World/Actor.hpp"
#include "Func/World/StaticMeshComponent.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/Misc.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"

using namespace rhi;

void PBRRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params) {
    ProfileScope scope(__func__);
    const bool has_active_viewport = UIManager::HasActiveViewportWindow();
    if (has_active_viewport) {
        Vector2f active_viewport_size = UIManager::GetActiveViewportWindow()->GetSize();
        if (active_viewport_size != hdr_color_->GetSize()) {
            OnWindowResized(nullptr, active_viewport_size.x, active_viewport_size.y);
        }
    }
    const auto view = GetBackBufferView(params.current_image_index);
    auto image = GetBackBuffer(params.current_image_index);
    cmd.Begin();

    ImageSubresourceRange range{};
    range.aspect_mask = IA_Color;
    range.base_array_layer = 0;
    range.base_mip_level = 0;
    range.layer_count = 1;
    range.level_count = 1;
    {
        Rect2Df rect{};
        rect.size = hdr_color_->GetSize();
        cmd.SetScissor(rect);
        cmd.SetViewport(rect);
        cmd.Execute();
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
        cmd.BeginRender(attachments, depth_attachment, hdr_color_->GetSize());
        if (has_active_viewport) {
            PerformMeshPass(cmd);
            PerformSkyboxPass(cmd);
            cmd.Execute();
        }
        cmd.EndRender();
        cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::ShaderReadOnly, hdr_color_->GetImage(), range, AFB_ColorAttachmentWrite,
                                 0, PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
        cmd.EndDebugLabel();
        cmd.Execute();
    }
    {
        cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, image, range, 0, AFB_ColorAttachmentWrite,
                                 PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
        cmd.Execute();

        PerformImGuiPass(cmd, params);

        if (has_active_viewport) {
            auto *active = UIManager::GetActiveViewportWindow();
            Rect2Df rect;
            rect.size = active->GetSize();
            rect.pos = active->GetPosition();
            if (rect.size.x <= 0 || rect.size.y <= 0) {
            } else {
                cmd.BeginDebugLabel("ColorTransformPass");
                cmd.SetViewport(rect);
                rect.pos.x = rect.pos.x <= 0 ? 0 : rect.pos.x;
                rect.pos.y = rect.pos.y <= 0 ? 0 : rect.pos.y;
                cmd.SetScissor(rect);
                PerformColorTransformPass(cmd, view);
                cmd.EndDebugLabel();
                cmd.Execute();
            }
        }
        cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::PresentSrc, image, range, AFB_ColorAttachmentWrite, 0,
                                 PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
        cmd.Execute();
    }

    cmd.End();
    cmd.Execute();
}

void PBRRenderPipeline::PerformMeshPass(rhi::CommandBuffer &cmd) const {
    ProfileScope _(__func__);
    cmd.BeginDebugLabel("MeshDraw");
    for (auto &mesh: RenderContext::GetDrawStaticMesh()) {
        if (mesh->GetHandle() == skybox_cube_.GetHandle())
            continue;
        auto first_instance_index = GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(mesh->GetHandle());
        auto index_count = mesh->GetIndexCount();
        helper::BindMaterial(cmd, mesh->GetMaterial());
        cmd.BindVertexBuffer(mesh->GetVertexBuffer());
        cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), sizeof(InstancedData1) * first_instance_index, 1);
        cmd.BindIndexBuffer(mesh->GetIndexBuffer());
        cmd.DrawIndexed(index_count, 1, 0);
    }
    cmd.EndDebugLabel();
}

void PBRRenderPipeline::PerformSkyboxPass(rhi::CommandBuffer &cmd) const {
    ProfileScope _(__func__);
    cmd.BeginDebugLabel("SkyspherePass");
    helper::BindMaterial(cmd, skysphere_pass_material_);
    // draw skybox
    StaticMeshComponent *mesh = skybox_cube_;
    auto first_instance_index = GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(mesh->GetHandle());
    auto index_count = mesh->GetIndexCount();
    cmd.BindVertexBuffer(mesh->GetVertexBuffer());
    cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), sizeof(InstancedData1) * first_instance_index, 1);
    cmd.BindIndexBuffer(mesh->GetIndexBuffer());
    cmd.DrawIndexed(index_count, 1, 0);
    cmd.EndDebugLabel();
}

void PBRRenderPipeline::PerformColorTransformPass(rhi::CommandBuffer &cmd, rhi::ImageView *target) const {
    ProfileScope _(__func__);
    Array<RenderAttachment> attachments{};
    RenderAttachment attachment{};
    attachment.load_op = AttachmentLoadOperation::DontCare;
    attachment.store_op = AttachmentStoreOperation::Store;
    attachment.target = target;
    attachment.layout = ImageLayout::ColorAttachment;
    attachments.Add(attachment);
    cmd.BeginRender(attachments);
    helper::BindMaterial(cmd, color_transform_pass_material_);
    cmd.BindVertexBuffer(screen_quad_vertex_buffer_);
    cmd.BindIndexBuffer(screen_quad_index_buffer_);
    cmd.DrawIndexed(6, 1, 0);
    cmd.EndRender();
}

void PBRRenderPipeline::PerformImGuiPass(rhi::CommandBuffer &cmd, const RenderParams &params) {
    ProfileScope _(__func__);
    BeginImGuiFrame(cmd, params);
    UIManager::DrawAll();
    EndImGuiFrame(cmd);
}

void PBRRenderPipeline::Build() {
    ProfileScope _("RenderPipeline::Build");
    ObjectManager::CreateNewObject<ImGuiDemoWindow>();
    auto basepass_shader = AssetDataBase::LoadAsync("Assets/Shader/PBR/BasePass.slang");
    auto skyspere_shader = AssetDataBase::LoadAsync("Assets/Shader/PBR/SkyspherePass.slang");
    auto colortransform_shader = AssetDataBase::LoadAsync("Assets/Shader/PBR/ColorTransformPass.slang");
    auto skysphere_texture = AssetDataBase::LoadAsync("Assets/Texture/poly_haven_studio_1k.exr");
    auto cube_mesh = AssetDataBase::LoadAsync("Assets/Mesh/Cube.fbx");
    ThreadManager::WhenAllExecFuturesCompleted(
            NamedThread::Game,
            [this](const ObjectHandle basepass_shader_handle, const ObjectHandle skysphere_shader_handle,
                   const ObjectHandle color_transform_shader_handle, const ObjectHandle skysphere_texture_handle,
                   const ObjectHandle cube_mesh_handle) {
                ProfileScope _("RenderPipeline::Build(OnAsyncCompleted)");
                const Shader *baspass_shader = ObjectManager::GetObjectByHandle<Shader>(basepass_shader_handle);
                const Shader *skysphere_pass_shader = ObjectManager::GetObjectByHandle<Shader>(skysphere_shader_handle);
                const Shader *color_transform_pass_shader = ObjectManager::GetObjectByHandle<Shader>(color_transform_shader_handle);
                const Texture2D *skysphere_texture = ObjectManager::GetObjectByHandle<Texture2D>(skysphere_texture_handle);
                const Mesh *cube_mesh = ObjectManager::GetObjectByHandle<Mesh>(cube_mesh_handle);
                skybox_cube_ = ObjectManager::CreateNewObject<Actor>()->AddComponent<StaticMeshComponent>();
                skybox_cube_->SetMesh(cube_mesh);

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
                Vertex1 vertices[] = {{{-1.0f, -1.0f, 0.0f}, {0, 0, 0}, {0, 0}},
                                      {{1.0f, -1.0f, 0.0f}, {0, 0, 0}, {1, 0}},
                                      {{-1.0f, 1.0f, 0.0f}, {0, 0, 0}, {0, 1}},
                                      {{1.0f, 1.0f, 0.0f}, {0, 0, 0}, {1, 1}}};
                GfxCommandHelper::CopyDataToBuffer(&vertices, screen_quad_vertex_buffer_.get(), 4 * sizeof(Vertex1));
                BufferDesc index_buffer_desc{6 * sizeof(UInt32), BUB_IndexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
                screen_quad_index_buffer_ = GetGfxContextRef().CreateBuffer(index_buffer_desc);
                UInt32 indices[] = {0, 2, 1, 1, 2, 3};
                GfxCommandHelper::CopyDataToBuffer(&indices, screen_quad_index_buffer_.get(), 6 * sizeof(UInt32));
                ready_ = true;
            },
            Move(basepass_shader), Move(skyspere_shader), Move(colortransform_shader), Move(skysphere_texture), Move(cube_mesh));
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
    auto *viewport = UIManager::GetActiveViewportWindow();
    if (viewport == nullptr)
        return;
    Vector2f viewport_size = viewport->GetSize();
    if (viewport_size == Vector2f{static_cast<Float>(depth_target_->GetWidth()), static_cast<Float>(depth_target_->GetHeight())}) {
        return;
    }
    GetGfxContextRef().WaitForQueueExecution(QueueFamilyType::Graphics);
    depth_target_->Resize(viewport_size.x, viewport_size.y);
    hdr_color_->Resize(viewport_size.x, viewport_size.y);
    hdr_color_->BindToMaterial("tex", color_transform_pass_material_);
}
