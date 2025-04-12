//
// Created by Echo on 2025/4/2.
//

#include "PBRRenderPipeline.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/Render/Pipeline/PBRRenderPipelineSettingWindow.hpp"
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
        if (!sdr_color_ || active_viewport_size != sdr_color_->GetSize()) {
            OnWindowResized(nullptr, active_viewport_size.x, active_viewport_size.y);
        }
    }

    auto image = GetBackBuffer(params.current_image_index);
    cmd.Begin();

    ImageSubresourceRange range{};
    range.aspect_mask = IA_Color;
    range.base_array_layer = 0;
    range.base_mip_level = 0;
    range.layer_count = 1;
    range.level_count = 1;
    if (has_active_viewport) {
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
            PerformMeshPass(cmd);
            PerformSkyboxPass(cmd);
            cmd.Execute();
            cmd.EndRender();
            cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::ShaderReadOnly, hdr_color_->GetImage(), range,
                                     AFB_ColorAttachmentWrite, 0, PSFB_ColorAttachmentOutput, PSFB_BottomOfPipe);
            cmd.EndDebugLabel();
            cmd.Execute();
        }
        {
            cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, sdr_color_->GetImage(), range, 0, AFB_ColorAttachmentWrite,
                                     PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
            Rect2Df rect{};
            rect.size = UIManager::GetActiveViewportWindow()->GetSize();
            if (rect.size.x != 0 && rect.size.y != 0) {
                cmd.BeginDebugLabel("ColorTransformPass");
                cmd.SetViewport(rect);
                cmd.SetScissor(rect);
                PerformColorTransformPass(cmd, sdr_color_->GetImageView(), rect.size);
                cmd.EndDebugLabel();
            }
            cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::ShaderReadOnly, sdr_color_->GetImage(), range, 0,
                                     AFB_ColorAttachmentWrite, PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
            cmd.Execute();
        }
    }
    {
        cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, image, range, 0, AFB_ColorAttachmentWrite,
                                 PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
        cmd.Execute();
        PerformImGuiPass(cmd, params);
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

void PBRRenderPipeline::PerformSkyboxPass(rhi::CommandBuffer &cmd) {
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

void PBRRenderPipeline::PerformColorTransformPass(rhi::CommandBuffer &cmd, rhi::ImageView *target, Vector2f size) const {
    ProfileScope _(__func__);
    Array<RenderAttachment> attachments{};
    RenderAttachment attachment{};
    attachment.load_op = AttachmentLoadOperation::DontCare;
    attachment.store_op = AttachmentStoreOperation::Store;
    attachment.target = target;
    attachment.layout = ImageLayout::ColorAttachment;
    attachments.Add(attachment);
    cmd.BeginRender(attachments, {}, size);
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

ImGuiDrawWindow *PBRRenderPipeline::GetSettingWindow() {
    PBRRenderPipelineSettingWindow *w = UIManager::CreateOrActivateWindow<PBRRenderPipelineSettingWindow>(true);
    w->SetRenderPipeline(this);
    return w;
}


void PBRRenderPipeline::Build() {
    ProfileScope _("RenderPipeline::Build");
    ObjectManager::CreateNewObject<ImGuiDemoWindow>();
    auto basepass_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/BasePass.slang");
    auto skyspere_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/SkyspherePass.slang");
    auto colortransform_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/ColorTransformPass.slang");
    auto skysphere_texture = AssetDataBase::LoadOrImportAsync("Assets/Texture/NewportLoft.hdr");
    auto cube_mesh = AssetDataBase::LoadFromPathAsync("Assets/Mesh/Cube.fbx");
    ThreadManager::WhenAllExecFuturesCompleted(
            NamedThread::Game,
            [this](const ObjectHandle basepass_shader_handle, const ObjectHandle skysphere_shader_handle,
                   const ObjectHandle color_transform_shader_handle, const ObjectHandle skysphere_texture_handle,
                   const ObjectHandle cube_mesh_handle) {
                ProfileScope _("RenderPipeline::Build(OnAsyncCompleted)");
                const Shader *baspass_shader = ObjectManager::GetObjectByHandle<Shader>(basepass_shader_handle);
                const Shader *skysphere_pass_shader = ObjectManager::GetObjectByHandle<Shader>(skysphere_shader_handle);
                const Shader *color_transform_pass_shader = ObjectManager::GetObjectByHandle<Shader>(color_transform_shader_handle);
                Texture2D *skysphere_texture = ObjectManager::GetObjectByHandle<Texture2D>(skysphere_texture_handle);
                const Mesh *cube_mesh = ObjectManager::GetObjectByHandle<Mesh>(cube_mesh_handle);
                skybox_cube_ = ObjectManager::CreateNewObject<Actor>()->AddComponent<StaticMeshComponent>();
                skybox_cube_->SetMesh(cube_mesh);

                basepass_material_ = ObjectManager::CreateNewObject<Material>();
                skysphere_pass_material_ = ObjectManager::CreateNewObject<Material>();
                color_transform_pass_material_ = ObjectManager::CreateNewObject<Material>();

                basepass_material_->SetShader(baspass_shader);
                skysphere_pass_material_->SetShader(skysphere_pass_shader);
                color_transform_pass_material_->SetShader(color_transform_pass_shader);

                auto *w = PlatformWindowManager::GetMainWindow();
                UInt64 width = w->GetWidth();
                UInt64 height = w->GetHeight();
                ImageDesc desc{static_cast<UInt32>(width), static_cast<UInt32>(height), IUB_RenderTarget | IUB_ShaderRead,
                               Format::R32G32B32A32_Float};
                hdr_color_ = MakeShared<RenderTexture>(desc);
                depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
                hdr_color_->BindToMaterial("tex", color_transform_pass_material_);
                desc.format = Format::B8G8R8A8_UNorm;
                sdr_color_ = MakeShared<RenderTexture>(desc);

                skybox_texture_ = skysphere_texture;
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
    sdr_color_ = nullptr;
    hdr_color_ = nullptr;
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
    if (sdr_color_ && viewport_size == Vector2f{static_cast<Float>(sdr_color_->GetWidth()), static_cast<Float>(sdr_color_->GetHeight())}) {
        return;
    }
    GetGfxContextRef().WaitForQueueExecution(QueueFamilyType::Graphics);
    depth_target_->Resize(viewport_size.x, viewport_size.y);
    hdr_color_->Resize(viewport_size.x, viewport_size.y);
    hdr_color_->BindToMaterial("tex", color_transform_pass_material_);
    sdr_color_->Resize(viewport_size.x, viewport_size.y);
    UIManager::GetActiveViewportWindow()->BindRenderTexture(sdr_color_.get());
}
