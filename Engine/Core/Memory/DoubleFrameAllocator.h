//
// Created by Echo on 24-12-22.
//

#pragma once
#include <cstdint>

namespace core
{
class DoubleFrameAllocator
{
public:
    static void* Malloc(size_t size);

    static void Startup();
    static void Shutdown();
    static void Refresh();

private:
    // 每个缓冲区的大小
    static inline size_t   size_;
    static inline uint8_t* buffers_[2];
    static inline size_t   offsets_[2];
    static inline int      current_ = 0;
};
}
