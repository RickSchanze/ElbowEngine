/**
 * @file MemoryUtils.h
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#pragma once

class MemoryUtils {
public:
    /**
     * 分配一段对齐的内存
     * @param size 要求的内存大小
     * @param alignment 需要的对齐值
     * @return 指向内存的指针 这个指针地址 % alignment == 0
     */
    static void* AlignedAlloc(size_t size, size_t alignment);

    static void AlignedFree(void* ptr);

    /**
     * 重新分配一段对齐的内存
     * @param ptr 原本的内存
     * @param size 要求的大小
     * @param alignment 需要的对齐值
     * @return
     */
    static void* AlignedRealloc(void* ptr, size_t size, size_t alignment);
};
