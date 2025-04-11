//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Enums.hpp"


namespace rhi {
    class Buffer;
    class Image;
    class CommandBuffer;
    class GfxCommandHelper {
    public:
        static SharedPtr<CommandBuffer> BeginSingleCommand();

        static void EndSingleCommandTransfer(const SharedPtr<CommandBuffer> &command_buffer);
        static void EndSingleCommandGraphics(const SharedPtr<CommandBuffer> &command_buffer);
        static void EndSingleCommandCompute(const SharedPtr<CommandBuffer> &command_buffer);

        static void PipelineBarrier(ImageLayout old, ImageLayout new_, Image *target, const ImageSubresourceRange &range, AccessFlags src_mask,
                                    AccessFlags dst_mask, PipelineStageFlags src_stage, PipelineStageFlags dst_stage);

        /**
         * 将数据拷贝到buffer
         * @param data
         * @param target
         * @param size 大小 等于0 表示整个buffer
         * @param offset 偏移(尚未实现)
         */
        static void CopyDataToBuffer(const void *data, Buffer *target, UInt32 size = 0, UInt32 offset = 0);

        static void CopyDataToImage2D(const void *data, Image *target, UInt32 size, Vector3i offset = {}, Vector3i copy_range = {});
    };
} // namespace rhi
