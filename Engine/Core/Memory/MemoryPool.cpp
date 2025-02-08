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

#include "MemoryPool.h"

#include "Core/Base/Exception.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "mimalloc.h"

#include <sstream>

using namespace core;

void* operator new(size_t size, MemoryPool* mp)
{
    return mp->Allocate(size);
}

void* operator new[](const size_t size, MemoryPool* mp)
{
    return mp->Allocate(size);
}

MemoryPool::MemoryPool(const size_t block_size)
{
    // Add first block to memory pool
    this->first_block = this->current_block = nullptr;
    this->default_block_size                = block_size;
    this->current_scope                     = nullptr;
    this->CreateMemoryBlock(block_size);
}

MemoryPool::~MemoryPool()
{
    SMemoryBlockHeader* block_iterator = first_block;

    while (block_iterator != nullptr)
    {
        SMemoryBlockHeader* next_iterator = block_iterator->next;
        mi_free(block_iterator);
        block_iterator = next_iterator;
    }
}

void MemoryPool::CreateMemoryBlock(size_t block_size)
{
    // Create the block
    auto* block = static_cast<SMemoryBlockHeader*>(mi_malloc(sizeof(SMemoryBlockHeader) + block_size));
    if (block == nullptr) throw Exception("MemoryPool: 创建新内存块失败");

    // Initialize block data
    block->block_size          = block_size;
    block->offset              = 0;
    block->number_of_allocated = 0;
    block->number_of_deleted   = 0;

    if (this->first_block != nullptr)
    {
        block->next               = nullptr;
        block->prev               = this->current_block;
        this->current_block->next = block;
        this->current_block       = block;
    }
    else
    {
        block->next = block->prev = nullptr;
        this->first_block         = block;
        this->current_block       = block;
    }
}

void* MemoryPool::Allocate(size_t size)
{
    // If there is enough space in current block then use the current block
    if (size + sizeof(SMemoryUnitHeader) < this->current_block->block_size - this->current_block->offset)
    {
    }
    // Create new block if not enough space
    else if (size + sizeof(SMemoryUnitHeader) >= this->default_block_size)
        this->CreateMemoryBlock(size + sizeof(SMemoryUnitHeader));
    else
        this->CreateMemoryBlock(this->default_block_size);

    // Add unit
    const auto unit =
        reinterpret_cast<SMemoryUnitHeader*>(reinterpret_cast<char*>(this->current_block) + sizeof(SMemoryBlockHeader) + this->current_block->offset);
    unit->length    = size;
    unit->container = this->current_block;
    this->current_block->number_of_allocated++;
    this->current_block->offset += sizeof(SMemoryUnitHeader) + size;

    return reinterpret_cast<char*>(unit) + sizeof(SMemoryUnitHeader);
}

void* MemoryPool::Reallocate(void* unit_pointer_start, size_t new_size)
{
    if (unit_pointer_start == nullptr) return nullptr;

    // Find unit
    auto* unit = reinterpret_cast<SMemoryUnitHeader*>(static_cast<char*>(unit_pointer_start) - sizeof(SMemoryUnitHeader));

    // If last in block && enough space in block, then reset length
    if (SMemoryBlockHeader* block = unit->container; reinterpret_cast<char*>(block) + sizeof(SMemoryBlockHeader) + block->offset ==
                                                         reinterpret_cast<char*>(unit) + sizeof(SMemoryUnitHeader) + unit->length &&
                                                     block->block_size > block->offset + new_size - unit->length)
    {
        block->offset += new_size - unit->length;
        unit->length = new_size;

        return unit_pointer_start;
    }

    // Allocate new and free previous
    void* temp_point = this->Allocate(new_size);
    std::memcpy(temp_point, unit_pointer_start, unit->length);
    this->Free(unit_pointer_start);

    return temp_point;
}

void MemoryPool::Free(void* unit_pointer_start)
{
    if (unit_pointer_start == nullptr) return;

    // Find unit
    const auto          unit  = reinterpret_cast<SMemoryUnitHeader*>(static_cast<char*>(unit_pointer_start) - sizeof(SMemoryUnitHeader));
    SMemoryBlockHeader* block = unit->container;

    // If last in block, then reset offset
    if (reinterpret_cast<char*>(block) + sizeof(SMemoryBlockHeader) + block->offset ==
        reinterpret_cast<char*>(unit) + sizeof(SMemoryUnitHeader) + unit->length)
    {
        block->offset -= sizeof(SMemoryUnitHeader) + unit->length;
        block->number_of_allocated--;
    }
    else
        block->number_of_deleted++;

    // If block offset is 0 remove block if not the only one left
    if (this->current_block != this->first_block && (block->offset == 0 || block->number_of_allocated == block->number_of_deleted))
    {
        if (block == this->first_block)
        {
            this->first_block       = block->next;
            this->first_block->prev = nullptr;
        }
        else if (block == this->current_block)
        {
            this->current_block       = block->prev;
            this->current_block->next = nullptr;
        }
        else
        {
            block->prev->next = block->next;
            block->next->prev = block->prev;
        }
        mi_free(block);
    }
}


void MemoryPool::DumpPoolData() const
{
    SMemoryBlockHeader* block = this->first_block;

    size_t            block_counter = 1;
    size_t            unit_counter  = 1;
    std::stringstream ss;
    while (block != nullptr)
    {
        // Dump block data
        ss << "Block " << block_counter << ": " << std::endl;
        ss << "\t" << "Used: " << static_cast<float>(block->offset) / static_cast<float>(block->block_size) * 100 << "% " << "(" << block->offset
           << "/" << block->block_size << ")" << std::endl;

        if (block->offset == 0)
        {
            block = block->next;
            block_counter++;
            continue;
        }

        ss << "\t" << "Units: ========================" << std::endl;
        size_t current_unit_offset = 0;
        unit_counter               = 1;
        while (current_unit_offset < block->offset)
        {
            const auto* unit = reinterpret_cast<SMemoryUnitHeader*>(reinterpret_cast<char*>(block + 1) + current_unit_offset);
            ss << "\t\t" << "Unit " << unit_counter << ": " << unit->length + sizeof(SMemoryUnitHeader) << std::endl;
            current_unit_offset += sizeof(SMemoryUnitHeader) + unit->length;
            unit_counter++;
        }

        ss << "\t" << "===============================" << std::endl;

        block = block->next;
        block_counter++;
    }
    LOGGER.Info(logcat::Core_Memory, "Dump Memory Pool:\n {}", ss.str());
}

void MemoryPool::StartScope()
{
    // Create new scope, on top of previous if exists
    if (this->current_scope == nullptr)
    {
        this->current_scope             = new (this) SMemoryScopeHeader;
        this->current_scope->prev_scope = nullptr;
    }
    else
    {
        auto new_scope        = new (this) SMemoryScopeHeader;
        new_scope->prev_scope = this->current_scope;
        this->current_scope   = new_scope;
    }

    // Simply load the current offset & block to return to when scope ends
    this->current_scope->scopeOffset       = this->current_block->offset - sizeof(SMemoryScopeHeader) - sizeof(SMemoryUnitHeader);
    this->current_scope->first_scope_block = this->current_block;
}

void MemoryPool::EndScope()
{
    // Free all blocks until the start of scope
    while (this->current_block != this->current_scope->first_scope_block)
    {
        this->current_block = this->current_block->prev;
        mi_free(this->current_block->next);
        this->current_block->next = nullptr;
    }

    this->current_block->offset = this->current_scope->scopeOffset;
}
