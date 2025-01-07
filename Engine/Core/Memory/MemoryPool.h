/**
 * AppShift Memory Pool v2.0.0
 *
 * Copyright 2020-present Sapir Shemer, DevShift (devshift.biz)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author Sapir Shemer
 */
/**
 * 这文件是抄的 见https://github.com/DevShiftTeam/AppShift-MemoryPool 在这上面改进
 */
#pragma once

#include <cstddef>

namespace core
{
// Simple error collection for memory pool
enum class EMemoryErrors
{
    CANNOT_CREATE_MEMORY_POOL,
    CANNOT_CREATE_BLOCK,
    OUT_OF_POOL,
    EXCEEDS_MAX_SIZE,
    CANNOT_CREATE_BLOCK_CHAIN
};

// Header for a single memory block
struct SMemoryBlockHeader
{
    // Block data
    size_t block_size;
    size_t offset;

    // Movement to other blocks
    SMemoryBlockHeader* next;
    SMemoryBlockHeader* prev;

    // Garbage management data
    size_t number_of_allocated;
    size_t number_of_deleted;
};

// Header of a memory unit in the pool holding important metadata
struct SMemoryUnitHeader
{
    size_t              length;
    SMemoryBlockHeader* container;
};

// Header for a scope in memory
struct SMemoryScopeHeader
{
    size_t              scopeOffset;
    SMemoryBlockHeader* first_scope_block;
    SMemoryScopeHeader* prev_scope;
};

class MemoryPool
{
public:
    /**
     * Creates a memory pool structure and initializes it
	 *
	 * @param  block_size Defines the default size of a block in the pool, by default uses MEMORYPOOL_DEFAULT_BLOCK_SIZE
	 */
    explicit MemoryPool(size_t block_size);
    // Destructor
    ~MemoryPool();

    // Data about the memory pool blocks
    SMemoryBlockHeader* first_block;
    SMemoryBlockHeader* current_block;
    size_t              default_block_size;

    // Data about memory scopes
    SMemoryScopeHeader* current_scope;

    /**
		 * Create a new standalone memory block unattached to any memory pool
		 *
		 * @param block_size Defines the default size of a block in the pool
		 *
		 * @returns SMemoryBlockHeader* Pointer to the header of the memory block
		 */
    void CreateMemoryBlock(size_t block_size);

    /**
		 * Allocates memory in a pool
		 *
		 * @param size Size to allocate in memory pool
		 *
		 * @returns void* Pointer to the newly allocate space
		 */
    void* Allocate(size_t size);

    // Templated allocation
    template <typename T>
    T* Allocate(size_t instances);

    /**
		 * Re-allocates memory in a pool
		 *
		 * @param unit_pointer_start Pointer to the object to re-allocate
		 * @param new_size New size to allocate in memory pool
		 *
		 * @returns void* Pointer to the newly allocate space
		 */
    void* Reallocate(void* unit_pointer_start, size_t new_size);

    // Templated re-allocation
    template <typename T>
    T* Reallocate(T* unit_pointer_start, size_t instances);

    /**
		 * Frees memory in a pool
		 *
		 * @param unit_pointer_start Pointer to the object to free
		 */
    void Free(void* unit_pointer_start);

    /**
		 * Dump memory pool metadata of blocks unit to stream.
		 * Might be useful for debugging and analyzing memory usage
		 *
		 */
    void DumpPoolData() const;

    /**
		 * Start a scope in the memory pool.
		 * All the allocations between startScope and andScope will be freed.
		 * It is a very efficient way to free multiple allocations
		 *
		 */
    void StartScope();

    /**
		 *
		 */
    void EndScope();
};

template <typename T>
T* MemoryPool::Allocate(const size_t instances)
{
    return static_cast<T*>(this->Allocate(instances * sizeof(T)));
}

template <typename T>
T* MemoryPool::Reallocate(T* unit_pointer_start, size_t instances)
{
    return static_cast<T*>(this->Reallocate(reinterpret_cast<void*>(unit_pointer_start), instances * sizeof(T)));
	}
}
