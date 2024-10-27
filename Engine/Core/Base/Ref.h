/**
 * @file Ref.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include <type_traits>
namespace core
{
template<typename T>
struct Ref
{
    Ref(T& ref) : ptr_(&ref) {}
    Ref(const Ref& other) : ptr_(other.ptr_) {}
    Ref(Ref&& other) : ptr_(other.ptr_) {}

    T* operator->() { return ptr_; }

    T& operator*() { return *ptr_; }

    T* operator->() const { return ptr_; }

    T& operator*() const { return *ptr_; }

    bool operator==(const Ref& other) const { return ptr_ == other.ptr_; }

private:
    T* ptr_ = nullptr;
};

template<typename T>
Ref<T> MakeRef(T& ref)
{
    return Ref<T>(ref);
}

}   // namespace core
