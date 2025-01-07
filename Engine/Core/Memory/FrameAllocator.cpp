//
// Created by Echo on 25-1-7.
//

#include "FrameAllocator.h"

#include "Core/Base/Exception.h"

using namespace core;

FrameAllocator::FrameAllocator(Int32 frame_count, size_t every_size) : frame_count_(frame_count), frame_size_(every_size), current_frame_(0)
{
    buffer_ = malloc(frame_count * every_size);
}

FrameAllocator::~FrameAllocator()
{
    free(buffer_);
}

void* FrameAllocator::Allocate(size_t size)
{
    if (size > frame_size_ || current_offset_ + size > frame_size_)
    {
        throw core::Exception("请求尺寸溢出!");
    }
    char* rtn = (char*)buffer_ + current_frame_ * frame_size_ + current_offset_;
    current_offset_ += size;
    return rtn;
}

void FrameAllocator::Refresh()
{
    current_frame_  = (current_frame_ + 1) % frame_count_;
    current_offset_ = 0;
}