//
// Created by Echo on 2025/3/22.
//
#pragma once

#include "Core/TypeAlias.hpp"
#include "Malloc.hpp"

class FrameAllocator {
public:
    explicit FrameAllocator(const Int32 frame_count, const UInt64 every_size = 1024 * 1024) :
        frame_count_(frame_count), current_frame_(0), frame_size_(every_size) {
        buffer_ = Malloc(frame_count * every_size);
    }

    ~FrameAllocator() { Free(buffer_); }

    void *Allocate(size_t size);

    void Refresh();

private:
    Int32 frame_count_ = 0;
    void *buffer_ = nullptr;
    Int32 current_frame_ = 0;
    Int32 frame_size_ = 0;
    UInt64 current_offset_ = 0;
};


template<typename T, typename... Args>
T *NewFrameTemp(FrameAllocator &allocator, Args &&...args) {
    size_t size = sizeof(T);
    T *pos = static_cast<T *>(allocator.Allocate(size));
    new (pos) T(Forward<Args>(args)...);
    return pos;
}
