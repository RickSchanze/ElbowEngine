/**
 * @file FrameAllocator.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include <cstdint>

namespace core
{
class FrameAllocator
{
public:
    static void* Malloc(size_t size);
    static bool IsValid() { return memory_ != nullptr; }

    static void Startup();
    static void Shutdown();
    static void Refresh();

private:
    // 栈底
    static inline uint8_t* memory_ = nullptr;
    // 栈指针
    static inline uint8_t* peek_   = nullptr;
    // 栈顶
    static inline uint8_t* top_    = nullptr;
};
}   // namespace core
