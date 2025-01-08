//
// Created by Echo on 25-1-7.
//

#pragma once
#include "Core/Base/Base.h"
#include "Core/Base/CoreTypeDef.h"

constexpr auto MEMORY_POOL_ID_GLOBAL = 0;

namespace core
{
class MemoryPool;

class MemoryManager : public Manager<MemoryManager>
{
public:
    constexpr static Int32 DEFAULT_MEMORY_BLOCK_SIZE = 1024 * 1024;

    static MemoryPool* GetPool(Int32 id);

    static void* Allocate(Int32 size);
    static void  Free(void* ptr);

    void Startup() override;
    void Shutdown() override;

    [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::Fourth; }
    [[nodiscard]] StringView   GetName() const override { return "MemoryManager"; }

    static void RequestPool(Int32 id);

    static void ReleasePool(Int32 id);

private:
    HashMap<Int32, MemoryPool*> pools_;
};

void* _Allocate(MemoryPool* pool, size_t size);
void  _Free(MemoryPool* pool, void* ptr);

template <typename T>
struct PooledMemoryAllocator
{
    using value_type = T;

    explicit PooledMemoryAllocator(MemoryPool* pool) noexcept : pool_(pool) {}

    template <typename U>
    constexpr explicit PooledMemoryAllocator(const PooledMemoryAllocator<U>& u) noexcept
    {
        pool_ = u.pool_;
    }

    [[nodiscard]] T* allocate(std::size_t n)
    {
        if (n > static_cast<std::size_t>(-1) / sizeof(T))
        {
            throw std::bad_alloc();
        }
        T* p = (T*)_Allocate(pool_, n * sizeof(T));
        if (!p)
        {
            throw std::bad_alloc();
        }
        return p;
    }

    void deallocate(T* p, std::size_t n) noexcept { _Free(pool_, p); }

    [[no_unique_address]] MemoryPool* pool_;
};

template <typename T, typename... Args>
SharedPtr<T> MakeSharedPooled(MemoryPool* pool, Args&&... args)
{
    return std::allocate_shared<T>(PooledMemoryAllocator<T>{pool}, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
SharedPtr<T> MakeSharedPooled(Int32 id, Args&&... args)
{
    return std::allocate_shared<T>(PooledMemoryAllocator<T>{MemoryManager::GetPool(id)}, std::forward<Args>(args)...);
}

template <typename T>
using PooledArray = std::vector<T, PooledMemoryAllocator<T>>;

template <typename T>
PooledArray<T> MakePooledArray(Int32 id)
{
    return PooledArray<T>(PooledMemoryAllocator<T>{MemoryManager::GetPool(id)});
}

}   // namespace core
