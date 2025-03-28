//
// Created by Echo on 25-3-19.
//

#pragma once
#include "mimalloc.h"
#include "Core/TypeAlias.hpp"

inline void *Malloc(const UInt64 size) { return mi_malloc(size); }

inline void Free(void *ptr) { mi_free(ptr); }
