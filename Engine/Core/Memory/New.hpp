//
// Created by Echo on 2025/3/21.
//
#pragma once

#include "Malloc.hpp"

template<typename T, typename... Args>
T *New(Args &&...args) {
    void *t = Malloc(sizeof(T));
    return new (t) T(Forward<Args>(args)...);
}

template<typename T>
void Delete(T *t) {
    if (t) {
        std::destroy_at(t);
        Free(t);
    }
}
