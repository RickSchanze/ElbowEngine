//
// Created by Echo on 2025/4/2.
//

#include "PBRRenderPipeline.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Core/Math/MathExtensions.hpp"
#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/Render/Pipeline/PBRRenderPipelineSettingWindow.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Func/Render/RenderTexture.hpp"
#include "Func/Render/Light/LightManager.hpp"
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
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"

using namespace RHI;

void PBRRenderPipeline::Render(CommandBuffer &cmd, const RenderParams &params)
{
    ProfileScope scope(__func__);
    const bool has_active_viewport = UIManager::HasActiveViewportWindow();
    if (has_active_viewport)
    {
        Vector2f active_viewport_size = UIManager::GetActiveViewportWindow()->GetSize();
        if (!sdr_color_ || active_viewport_size != sdr_color_->GetSize())
        {
            OnWindowResized(nullptr, active_viewport_size.X, active_viewport_size.Y);
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
    if (has_active_viewport)
    {
        {
            {
                Rect2Df Rect{};
                Rect.size = mShadowBox->GetSize();
                cmd.SetScissor(Rect);
                cmd.SetViewport(Rect);
                cmd.Execute();
                PerformShadowPass(cmd);
            }
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
        } {
            cmd.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::ColorAttachment, sdr_color_->GetImage(), range, 0, AFB_ColorAttachmentWrite,
                                     PSFB_ColorAttachmentOutput, PSFB_ColorAttachmentOutput);
            Rect2Df rect{};
            rect.size = UIManager::GetActiveViewportWindow()->GetSize();
            if (rect.size.X != 0 && rect.size.Y != 0)
            {
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
    } {
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

void PBRRenderPipeline::PerformMeshPass(RHI::CommandBuffer &cmd) const
{
    ProfileScope _(__func__);
    cmd.BeginDebugLabel("MeshDraw");
    for (auto &mesh : RenderContext::GetDrawStaticMesh())
    {
        if (mesh->GetHandle() == skybox_cube_.GetHandle())
            continue;
        auto first_instance_index = GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(mesh->GetHandle());
        auto index_count = mesh->GetIndexCount();
        Helper::BindMaterial(cmd, mesh->GetMaterial());
        cmd.BindVertexBuffer(mesh->GetVertexBuffer());
        cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), sizeof(InstancedData1) * first_instance_index, 1);
        cmd.BindIndexBuffer(mesh->GetIndexBuffer());
        cmd.DrawIndexed(index_count, 1, 0);
    }
    cmd.EndDebugLabel();
}

void PBRRenderPipeline::PerformSkyboxPass(RHI::CommandBuffer &cmd)
{
    ProfileScope _(__func__);
    cmd.BeginDebugLabel("SkyspherePass");
    Helper::BindMaterial(cmd, skysphere_pass_material_);
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

void PBRRenderPipeline::PerformColorTransformPass(RHI::CommandBuffer &cmd, RHI::ImageView *target, Vector2f size) const
{
    ProfileScope _(__func__);
    Array<RenderAttachment> attachments{};
    RenderAttachment attachment{};
    attachment.load_op = AttachmentLoadOperation::DontCare;
    attachment.store_op = AttachmentStoreOperation::Store;
    attachment.target = target;
    attachment.layout = ImageLayout::ColorAttachment;
    attachments.Add(attachment);
    cmd.BeginRender(attachments, {}, size);
    Helper::BindMaterial(cmd, color_transform_pass_material_);
    cmd.BindVertexBuffer(screen_quad_vertex_buffer_);
    cmd.BindIndexBuffer(screen_quad_index_buffer_);
    cmd.DrawIndexed(6, 1, 0);
    cmd.EndRender();
}

void PBRRenderPipeline::PerformImGuiPass(RHI::CommandBuffer &cmd, const RenderParams &params)
{
    ProfileScope _(__func__);
    BeginImGuiFrame(cmd, params);
    UIManager::DrawAll();
    EndImGuiFrame(cmd);
}

glm::mat4 GetViewMatrix(Int32 Face)
{
    glm::mat4 ViewMatrix = glm::mat4(1.0f);
    switch (Face)
    {
    case 0: // POSITIVE_X
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    case 1: // NEGATIVE_X
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    case 2: // POSITIVE_Y
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    case 3: // NEGATIVE_Y
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    case 4: // POSITIVE_Z
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;
    case 5: // NEGATIVE_Z
        ViewMatrix = glm::rotate(ViewMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        break;
    }
    return ViewMatrix;
}

void PBRRenderPipeline::PerformShadowPass(RHI::CommandBuffer &Cmd)
{
    Vector3f LightLocation = LightManager::GetLightPositions(nullptr);
    glm::mat4 Projection = glm::perspective(static_cast<float>(3.1415926535 / 2), 1.f, 0.1f, 1024.f);
    glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(-LightLocation.X, -LightLocation.Y, -LightLocation.Z));
    ThreadManager::ScheduleFutureAsync(exec::Just() | exec::Then([this, Model, Projection, LightLocation] {
        mShadowPassMaterial->SetMatrix4x4("uMatrix.Projection", Projection | ToMatrix4x4f);
        mShadowPassMaterial->SetFloat3("uFloat.LightPos", LightLocation);
    }), NamedThread::Game);
    Cmd.BeginDebugLabel("ShadowPass");
    Cmd.ImagePipelineBarrier(ImageLayout::ShaderReadOnly, ImageLayout::ColorAttachment, mShadowBox->GetImage(), {IA_Color, 0, 1, 0, 6}, 0,
                             AFB_ColorAttachmentWrite,
                             PSFB_FragmentShader, PSFB_ColorAttachmentOutput);

    for (Int32 I = 0; I < 6; I++)
    {
        RenderAttachment Attachment;
        Attachment.layout = ImageLayout::ColorAttachment;
        Attachment.target = mCubeViews[I].get();
        Attachment.load_op = AttachmentLoadOperation::Clear;
        Attachment.store_op = AttachmentStoreOperation::Store;
        Array<RenderAttachment> Attachments;
        Attachments.Add(Attachment);
        RenderAttachment Depth;
        Depth.layout = ImageLayout::DepthStencilAttachment;
        Depth.target = mShadowDepthCubeViews[I].get();
        Depth.store_op = AttachmentStoreOperation::Store;
        Depth.load_op = AttachmentLoadOperation::Clear;
        Depth.clear_color.r = 1.0f;
        Cmd.BeginRender(Attachments, Depth, {mShadowBox->GetSize().X, mShadowBox->GetSize().Y});
        auto View = GetViewMatrix(I);
        mShadowPassMaterial->GetSharedMaterial()->PushConstant(Cmd, View);
        for (auto &mesh : RenderContext::GetDrawStaticMesh())
        {
            if (mesh->GetHandle() == skybox_cube_.GetHandle())
                continue;
            auto first_instance_index = GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(mesh->GetHandle());
            auto index_count = mesh->GetIndexCount();
            Helper::BindMaterial(Cmd, mShadowPassMaterial);
            Cmd.BindVertexBuffer(mesh->GetVertexBuffer());
            Cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), sizeof(InstancedData1) * first_instance_index, 1);
            Cmd.BindIndexBuffer(mesh->GetIndexBuffer());
            Cmd.DrawIndexed(index_count, 1, 0);
        }
        Cmd.EndRender();
    }
    Cmd.ImagePipelineBarrier(ImageLayout::ColorAttachment, ImageLayout::ShaderReadOnly, mShadowBox->GetImage(), {IA_Color, 0, 1, 0, 6}, 0,
                             AFB_ShaderRead,
                             PSFB_ColorAttachmentOutput, PSFB_FragmentShader);
    Cmd.EndDebugLabel();
    Cmd.Execute();
}

ImGuiDrawWindow *PBRRenderPipeline::GetSettingWindow()
{
    PBRRenderPipelineSettingWindow *w = UIManager::CreateOrActivateWindow<PBRRenderPipelineSettingWindow>(true);
    w->SetRenderPipeline(this);
    return w;
}


void PBRRenderPipeline::Build()
{
    ProfileScope _("RenderPipeline::Build");
    ObjectManager::CreateNewObject<ImGuiDemoWindow>();
    auto basepass_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/BasePass.slang");
    auto skyspere_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/SkyspherePass.slang");
    auto colortransform_shader = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/ColorTransformPass.slang");
    auto skysphere_texture = AssetDataBase::LoadOrImportAsync("Assets/Texture/NewportLoft.hdr");
    auto cube_mesh = AssetDataBase::LoadFromPathAsync("Assets/Mesh/Cube.fbx");
    auto ShadowPass = AssetDataBase::LoadFromPathAsync("Assets/Shader/PBR/ShadowPass.slang");
    ThreadManager::WhenAllExecFuturesCompleted(
        NamedThread::Game,
        [this](const ObjectHandle basepass_shader_handle, const ObjectHandle skysphere_shader_handle,
               const ObjectHandle color_transform_shader_handle, const ObjectHandle skysphere_texture_handle,
               const ObjectHandle cube_mesh_handle, const ObjectHandle ShadowPassHandle) {
            ProfileScope _("RenderPipeline::Build(OnAsyncCompleted)");
            const Shader *baspass_shader = ObjectManager::GetObjectByHandle<Shader>(basepass_shader_handle);
            const Shader *skysphere_pass_shader = ObjectManager::GetObjectByHandle<Shader>(skysphere_shader_handle);
            const Shader *color_transform_pass_shader = ObjectManager::GetObjectByHandle<Shader>(color_transform_shader_handle);
            Texture2D *SkyBoxTexture = ObjectManager::GetObjectByHandle<Texture2D>(skysphere_texture_handle);
            const Mesh *cube_mesh = ObjectManager::GetObjectByHandle<Mesh>(cube_mesh_handle);
            const Shader *ShadowPass = ObjectManager::GetObjectByHandle<Shader>(ShadowPassHandle);
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
            desc.Format = Format::B8G8R8A8_UNorm;
            sdr_color_ = MakeShared<RenderTexture>(desc);

            skybox_texture_ = SkyBoxTexture;
            skysphere_pass_material_->SetTexture2D("skybox_texture", SkyBoxTexture);
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
            desc.DepthOrLayers = 6;
            desc.Format = Format::R32_Float;
            desc.Width = 512;
            desc.Height = 512;
            mShadowBox = MakeShared<RenderTexture>(desc);
            mShadowPassMaterial = ObjectManager::CreateNewObject<Material>();
            mShadowPassMaterial->SetShader(ShadowPass);
            desc.Format = GetGfxContextRef().GetDefaultDepthStencilFormat();
            desc.Usage = IUB_DepthStencil;
            mShadowPassDepth = MakeShared<RenderTexture>(desc);
            mShadowDepthCubeViews = mShadowPassDepth->CreateCubeViews();
            mCubeViews = mShadowBox->CreateCubeViews();
            ready_ = true;
        },
        Move(basepass_shader), Move(skyspere_shader), Move(colortransform_shader), Move(skysphere_texture), Move(cube_mesh), Move(ShadowPass));
}

void PBRRenderPipeline::Clean()
{
    depth_target_ = nullptr;
    sdr_color_ = nullptr;
    hdr_color_ = nullptr;
    ready_ = false;
    basepass_material_ = nullptr;
}

bool PBRRenderPipeline::IsReady() const
{
    return ready_;
}

void PBRRenderPipeline::OnWindowResized(PlatformWindow *window, Int32 width, Int32 height)
{
    if (width == 0 || height == 0)
        return;
    auto *viewport = UIManager::GetActiveViewportWindow();
    if (viewport == nullptr)
        return;
    Vector2f viewport_size = viewport->GetSize();
    if (sdr_color_ && viewport_size == Vector2f{static_cast<Float>(sdr_color_->GetWidth()), static_cast<Float>(sdr_color_->GetHeight())})
    {
        return;
    }
    GetGfxContextRef().WaitForQueueExecution(QueueFamilyType::Graphics);
    depth_target_->Resize(viewport_size.X, viewport_size.Y);
    hdr_color_->Resize(viewport_size.X, viewport_size.Y);
    hdr_color_->BindToMaterial("tex", color_transform_pass_material_);
    sdr_color_->Resize(viewport_size.X, viewport_size.Y);
    UIManager::GetActiveViewportWindow()->BindRenderTexture(sdr_color_.get());
}