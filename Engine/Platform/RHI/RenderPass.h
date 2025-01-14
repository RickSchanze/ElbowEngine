//
// Created by Echo on 24-12-28.
//

#pragma once
#include "Enums.h"
#include "IResource.h"

namespace platform::rhi
{

struct AttachmentDesc
{
    Format                   format;
    SampleCount              sample_count;
    AttachmentLoadOperation  load_op;
    AttachmentStoreOperation store_op;
    AttachmentLoadOperation  stencil_load_op;
    AttachmentStoreOperation stencil_store_op;
    ImageLayout              initial_layout;   // 初始Layout
    ImageLayout              final_layout;     // 最终Layout

    AttachmentDesc(
        Format fmt, ImageLayout final_layout,
        AttachmentLoadOperation  load_op          = AttachmentLoadOperation::Clear,       //
        AttachmentStoreOperation store_op         = AttachmentStoreOperation::Store,      //
        AttachmentLoadOperation  stencil_load_op  = AttachmentLoadOperation::DontCare,    //
        AttachmentStoreOperation stencil_store_op = AttachmentStoreOperation::DontCare,   //
        ImageLayout              initial_layout   = ImageLayout::Undefined,               //
        SampleCount              sample_count     = SampleCount::SC_1                     //
    ) :
        format(fmt), sample_count(sample_count), load_op(load_op), store_op(store_op), stencil_load_op(stencil_load_op),
        stencil_store_op(stencil_store_op), initial_layout(initial_layout), final_layout(final_layout)
    {
    }
};

struct AttachmentDescRef
{
    uint32_t    index;
    ImageLayout render_layout;   // 渲染时的Layout

    explicit AttachmentDescRef(uint32_t index = UINT32_MAX, ImageLayout render_layout = ImageLayout::Undefined) :
        index(index), render_layout(render_layout)
    {
    }
};

struct SubpassDesc
{
    core::Array<AttachmentDescRef> color_attachments;
    AttachmentDescRef              depth_attachment;
};

// TODO: 少个Dependency
struct RenderPassCreateInfo
{
    core::Array<AttachmentDesc> attachments;
    core::Array<SubpassDesc>    subpasses;
};

/**
 * D3D12没有实际的RenderPass
 * 但是可以模拟 因此也整一个RenderPass出来
 */
class RenderPass : public IResource
{
public:
    explicit RenderPass(const RenderPassCreateInfo& info) : info_(info) {}

    [[nodiscard]] const RenderPassCreateInfo& GetCreateInfo() const { return info_; }

protected:
    RenderPassCreateInfo info_{};
};

}
