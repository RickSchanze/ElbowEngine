//
// Created by Echo on 2025/3/25.
//

#include "FrameAllocator.hpp"

#include <cstdlib>
#include <iostream>


void *FrameAllocator::Allocate(const UInt64 size) {
  if (size > frame_size_ || current_offset_ + size > frame_size_) {
    std::cerr << "FrameAllocator::Allocate: size too large" << std::endl;
    exit(1);
  }
  char *rtn = static_cast<char *>(buffer_) + current_frame_ * frame_size_ + current_offset_;
  current_offset_ += size;
  return rtn;
}

void FrameAllocator::Refresh()  {
  current_frame_ = (current_frame_ + 1) % frame_count_;
  current_offset_ = 0;
}
