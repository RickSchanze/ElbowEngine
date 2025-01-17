//
// Created by Echo on 25-1-4.
//

#include "FixedBasicTestRenderPipeline.h"

#include "Core/Profiler/ProfileMacro.h"
#include "Func/Camera/CameraComponent.h"
#include "Misc.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Pipeline.h"
#include "Platform/Window/WindowManager.h"
#include "RenderContext.h"
#include "RenderTexture.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Mesh/Mesh.h"
#include "Resource/Assets/Shader/Shader.h"

#include <Func/Camera/Camera.h>

using namespace resource;
using namespace core;
using namespace platform::rhi;

void func::FixedBasicTestRenderPipeline::Render(CommandBuffer& cmd, const RenderParams& params)
{
    PROFILE_SCOPE_AUTO;
    auto view  = GetBackBufferView(params.current_image_index);
    auto image = GetBackBuffer(params.current_image_index);
    cmd.Begin();

    auto   w = platform::GetWindowManager().GetMainWindow();
    Rect2D rect{};
    rect.size = {w->GetWidth(), w->GetHeight()};
    cmd.Enqueue<Cmd_SetScissor>(rect);
    cmd.Enqueue<Cmd_SetViewport>(rect);
    cmd.Execute("WindowResize");

    ImageSubresourceRange range{};
    range.aspect_mask      = IA_Color;
    range.base_array_layer = 0;
    range.base_mip_level   = 0;
    range.layer_count      = 1;
    range.level_count      = 1;
    cmd.Enqueue<Cmd_ImagePipelineBarrier>(
        ImageLayout::Undefined,
        ImageLayout::ColorAttachment,
        image,
        range,
        0,
        AFB_ColorAttachmentWrite,
        PSFB_ColorAttachmentOutput,
        PSFB_ColorAttachmentOutput
    );
    RenderAttachment attachment{};
    attachment.clear_color                    = Color::DefaultClear();
    attachment.target                         = view;
    attachment.layout                         = ImageLayout::ColorAttachment;
    PooledArray<RenderAttachment> attachments = MakePooledArray<RenderAttachment>(MEMORY_POOL_ID_CMD);
    attachments.push_back(attachment);
    RenderAttachment depth_attachment{};
    depth_attachment.clear_color.r = 1.0f;
    depth_attachment.layout        = ImageLayout::DepthStencilAttachment;
    depth_attachment.target        = depth_target_->GetImageView();
    cmd.Enqueue<Cmd_BeginRender>(attachments, depth_attachment);
    cmd.Enqueue<Cmd_BindPipeline>(pipeline_.Get());
    cmd.Enqueue<Cmd_BindDescriptorSet>(pipeline_.Get(), descriptor_set_.get());
    BindAndDrawMesh(cmd, mesh_);
    cmd.Enqueue<Cmd_EndRender>();
    cmd.Enqueue<Cmd_ImagePipelineBarrier>(
        ImageLayout::ColorAttachment,
        ImageLayout::PresentSrc,
        image,
        range,
        AFB_ColorAttachmentWrite,
        0,
        PSFB_ColorAttachmentOutput,
        PSFB_BottomOfPipe
    );
    cmd.Execute("Draw Cube Mesh");
    cmd.End();
}

void func::FixedBasicTestRenderPipeline::Build()
{
    const auto shader = AssetDataBase::Load<Shader>("Assets/Shader/Error.slang");
    if (shader)
    {
        auto desc = GraphicsPipelineDesc{};
        if (shader->FillGraphicsPSODescFromShader(desc))
        {
            desc.attachments.depth_format = GetGfxContextRef().GetDefaultDepthStencilFormat();
            desc.attachments.color_formats.push_back(GetGfxContextRef().GetDefaultColorFormat());
            pipeline_       = GetGfxContextRef().CreateGraphicsPipeline(desc, nullptr);
            descriptor_set_ = RenderContext::AllocateDescriptorSet(desc.descriptor_set_layouts[0]);
            // TODO: Update 描述符集的操作本应交给材质来做
            DescriptorBufferUpdateInfo info{};
            info.buffer = Camera::GetViewBuffer();
            info.range  = sizeof(CameraShaderData);
            info.offset = 0;
            descriptor_set_->Update(0, info);
            depth_target_ = MakeShared<RenderTexture>(GetDepthImageDesc());
        }
    }
    mesh_ = AssetDataBase::Load<Mesh>("Assets/Mesh/Cube.fbx");
}

void func::FixedBasicTestRenderPipeline::Clean()
{
    pipeline_ = nullptr;
}

bool func::FixedBasicTestRenderPipeline::IsReady() const
{
    return pipeline_.IsSet();
}

void func::FixedBasicTestRenderPipeline::OnWindowResized(platform::Window* window, Int32 width, Int32 height)
{
    if (width == 0 || height == 0) return;
    depth_target_->Resize(width, height);
}