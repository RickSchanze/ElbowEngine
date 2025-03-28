//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Enums.hpp"
#include "IResource.hpp"

namespace rhi {
    struct AttachmentDesc {
        Format format;
        SampleCount sample_count;
        AttachmentLoadOperation load_op;
        AttachmentStoreOperation store_op;
        AttachmentLoadOperation stencil_load_op;
        AttachmentStoreOperation stencil_store_op;
        ImageLayout initial_layout; // 初始Layout
        ImageLayout final_layout; // 最终Layout

        AttachmentDesc(const Format fmt, const ImageLayout final_layout,
                       const AttachmentLoadOperation load_op = AttachmentLoadOperation::Clear, //
                       const AttachmentStoreOperation store_op = AttachmentStoreOperation::Store, //
                       const AttachmentLoadOperation stencil_load_op = AttachmentLoadOperation::DontCare, //
                       const AttachmentStoreOperation stencil_store_op = AttachmentStoreOperation::DontCare, //
                       const ImageLayout initial_layout = ImageLayout::Undefined, //
                       const SampleCount sample_count = SampleCount::SC_1 //
                       ) :
            format(fmt), sample_count(sample_count), load_op(load_op), store_op(store_op), stencil_load_op(stencil_load_op),
            stencil_store_op(stencil_store_op), initial_layout(initial_layout), final_layout(final_layout) {}
    };

    struct AttachmentDescRef {
        UInt32 index;
        ImageLayout render_layout; // 渲染时的Layout

        explicit AttachmentDescRef(const UInt32 index = NumberMax<UInt32>(), const ImageLayout render_layout = ImageLayout::Undefined) :
            index(index), render_layout(render_layout) {}
    };

    struct SubpassDesc {
        Array<AttachmentDescRef> color_attachments;
        AttachmentDescRef depth_attachment;
    };

    // TODO: 少个Dependency
    struct RenderPassCreateInfo {
        Array<AttachmentDesc> attachments;
        Array<SubpassDesc> subpasses;
    };

    /**
     * D3D12没有实际的RenderPass
     * 但是可以模拟 因此也整一个RenderPass出来
     */
    class RenderPass : public IResource {
    public:
        explicit RenderPass(const RenderPassCreateInfo &info) : info_(info) {}

        [[nodiscard]] const RenderPassCreateInfo &GetCreateInfo() const { return info_; }

    protected:
        RenderPassCreateInfo info_{};
    };
} // namespace rhi
