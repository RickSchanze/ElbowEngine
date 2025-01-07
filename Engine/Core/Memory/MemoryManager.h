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
    StringView                 GetName() const override { return "MemoryManager"; }

    static void RequestPool(Int32 id);

    static void ReleasePool(Int32 id);

private:
    HashMap<Int32, MemoryPool*> pools_;
};
}   // namespace core
