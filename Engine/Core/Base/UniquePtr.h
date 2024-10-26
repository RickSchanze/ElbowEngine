/**
 * @file UniquePtr.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include <utility>

namespace core
{
template<typename T>
class UniquePtr
{
public:
    // 构造函数
    template<typename... Args>
    static UniquePtr<T> Create(Args&&... args)
    {
#ifdef ENABLE_PROFILING
        MemoryTraceAllocator<T> allocator;
        T*                      ptr = allocator.allocate(1);
        try
        {
            new (ptr) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            allocator.deallocate(ptr, 1);
            throw;
        }
#else
        T* ptr = new T(std::forward<Args>(args)...);
#endif
        return UniquePtr<T>(ptr);
    }

    // 默认构造函数
    UniquePtr() noexcept = default;

    // 构造函数
    UniquePtr(T* ptr) noexcept : ptr_(ptr) {}

    // 移动构造函数
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }

    // 移动赋值运算符
    UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        if (this != &other)
        {
            Reset();
            std::swap(ptr_, other.ptr_);
        }
        return *this;
    }

    // 子类到父类的转换
    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    UniquePtr(UniquePtr<U>&& other) noexcept : ptr_(other.Release()) {}

    // 删除拷贝构造函数和赋值运算符
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&)  = delete;

    // 析构函数
    ~UniquePtr() { Reset(); }

    // 重置指针
    void Reset(T* ptr = nullptr) noexcept
    {
        if (ptr_)
        {
#ifdef ENABLE_PROFILING
            MemoryTraceDeleter<T>()(ptr_);
#else
            delete ptr_;
#endif
        }
        ptr_ = ptr;
    }

    // 释放指针
    T* Release() noexcept
    {
        T* temp = ptr_;
        ptr_    = nullptr;
        return temp;
    }

    // 获取指针
    T* Get() const noexcept { return ptr_; }

    // 解引用运算符
    T& operator*() const { return *ptr_; }

    // 成员访问运算符
    T* operator->() const noexcept { return ptr_; }

    // 检查指针是否为空
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    T* ptr_ = nullptr;
};

template<typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args)
{
    return UniquePtr<T>::Create(std::forward<Args>(args)...);
}
}
