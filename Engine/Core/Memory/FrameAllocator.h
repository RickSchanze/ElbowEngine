//
// Created by Echo on 25-1-7.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"

namespace core
{

class FrameAllocator
{
public:
    explicit FrameAllocator(Int32 frame_count, size_t every_size = 1024 * 1024);

    ~FrameAllocator();

    void* Allocate(size_t size);

    void Refresh();

private:
    Int32 frame_count_    = 0;
    void* buffer_         = nullptr;
    Int32 current_frame_  = 0;
    Int32 frame_size_     = 0;
    Int32 current_offset_ = 0;
};

template <typename T, typename... Args>
T* NewFrameTemp(FrameAllocator& allocator, Args&&... args)
{
    size_t size = sizeof(T);
    T*     pos  = (T*)allocator.Allocate(size);
    new (pos) T(std::forward<Args>(args)...);
    return pos;
}

}   // namespace core
