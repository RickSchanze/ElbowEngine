//
// Created by Echo on 25-3-19.
//

#pragma once
#include <algorithm>
#include <ranges>
#include <vector>
#include "Core/TypeAlias.hpp"

template<typename T>
class Array {
public:
    using iterator = typename std::vector<T>::iterator;
    Array() = default;
    Array(const Array &other) : data_(other.data_) {}
    Array(Array &&other) noexcept : data_(std::move(other.data_)) {}
    Array &operator=(const Array &other) = default;
    Array &operator=(Array &&other) noexcept {
        data_ = std::move(other.data_);
        return *this;
    }
    Array(const std::initializer_list<T> &values) : data_(values) {}

    template<std::ranges::input_range R>
    Array(R &&values) {
        for (auto &&value: values) {
            Add(std::move(value));
        }
    }

    void Add(T &&value) { data_.push_back(std::move(value)); }

    void Add(const T &value) { data_.push_back(value); }

    T &Emplace() { return data_.emplace_back(); }
    void Emplace(const T &value) { data_.emplace_back(value); }
    void Emplace(T &&value) { data_.emplace_back(std::move(value)); }

    void AddRange(const std::initializer_list<T> &values) { data_.insert(data_.end(), values.begin(), values.end()); }

    void RemoveAt(size_t index) { data_.erase(data_.begin() + index); }
    void FastRemoveAt(size_t index) {
        std::swap(data_[index], data_.back());
        data_.pop_back();
    }

    UInt64 Count() const { return data_.size(); }
    UInt64 Capacity() const { return data_.capacity(); }

    void Resize(UInt64 size) { data_.resize(size); }
    void Reserve(UInt64 size) { data_.reserve(size); }

    T &Last() { return data_.back(); }
    T &Front() { return data_.front(); }

    UInt64 IndexOf(const T &value) {
        for (size_t i = 0; i < data_.size(); ++i) {
            if (data_[i] == value) {
                return i;
            }
        }
        return UINT64_MAX;
    }

    template<typename Func>
        requires std::is_invocable_v<Func, T &>
    UInt64 IndexOf(Func &&pred) {
        for (size_t i = 0; i < data_.size(); ++i) {
            if (pred(data_[i])) {
                return i;
            }
        }
        return UINT64_MAX;
    }

    void Remove(const T &value) {
        for (auto it = data_.begin(); it != data_.end(); ++it) {
            if (*it == value) {
                data_.erase(it);
                break;
            }
        }
    }

    void RemoveLast() { RemoveAt(data_.size() - 1); }

    T &operator[](UInt64 index) { return data_[index]; }
    const T &operator[](UInt64 index) const { return data_[index]; }

    void Clear() { data_.clear(); }

    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    bool Contains(const T &value) {
        for (auto &item: data_) {
            if (item == value) {
                return true;
            }
        }
        return false;
    }

    template<typename Func>
        requires std::is_invocable_v<Func, T &>
    bool Contains(Func &&func) {
        for (auto &item: data_) {
            if (func(item)) {
                return true;
            }
        }
        return false;
    }

    bool Empty() const { return data_.empty(); }

    const T *Data() const { return data_.data(); }
    T *Data() { return data_.data(); }

    bool operator==(const Array &o) const { return data_ == o.data_; }

    Array Unique() {
        Array result;
        for (auto &item: data_) {
            if (!result.Contains(item)) {
                result.Add(item);
            }
        }
        return result;
    }

    template<typename Func>
    Array Sort(Func &&func) const {
        Array result = *this;
        std::sort(result.begin(), result.end(), func);
        return result;
    }

    Array Sort() const {
        Array result = *this;
        std::sort(result.begin(), result.end());
        return result;
    }

    template<typename Func>
    void SortInplace(Func &&func) {
        std::sort(data_.begin(), data_.end(), func);
    }

    void SortInplace() { std::sort(data_.begin(), data_.end()); }

private:
    std::vector<T> data_;
};
