//
// Created by Echo on 2025/3/25.
//
#pragma once
#include <array>
#include "Core/TypeAlias.hpp"

template<typename T, UInt64 I>
struct StaticArray {
    auto begin() { return data_.begin(); }
    auto begin() const { return data_.begin(); }
    auto end() { return data_.end(); }
    auto end() const { return data_.end(); }

    T &operator[](UInt64 idx) { return data_[idx]; }

    const T &operator[](UInt64 idx) const { return data_[idx]; }

    bool Contains(const T &v) const {
        for (UInt64 i = 0; i < I; i++) {
            if (v == data_[i])
                return true;
        }
        return false;
    }

private:
    std::array<T, I> data_;
};
