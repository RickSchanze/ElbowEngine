//
// Created by Echo on 2025/3/20.
//
#pragma once

#include <unordered_map>

#include "Core/TypeAlias.hpp"

template<typename Key, typename Value>
class Map {
public:
    using key_type = typename std::unordered_map<Key, Value>::key_type;
    using mapped_type = typename std::unordered_map<Key, Value>::mapped_type;
    using value_type = typename std::unordered_map<Key, Value>::value_type;
    using iterator = typename std::unordered_map<Key, Value>::iterator;

    Map() = default;
    Map(const Map &) = default;
    Map(Map &&) = default;
    Map &operator=(const Map &) = default;
    Map &operator=(Map &&) = default;

    bool Add(const Key &key, const Value &value) {
        if (!data_.contains(key)) {
            data_[key] = value;
            return true;
        } else {
            return false;
        }
    }

    bool Remove(const Key &key) {
        if (data_.contains(key)) {
            data_.erase(key);
            return true;
        } else {
            return false;
        }
    }

    bool RemoveByValue(const Value &value) {
        for (auto it = data_.begin(); it != data_.end();) {
            if (it->second == value) {
                data_.erase(it);
                return true;
            }
        }
        return false;
    }

    Value Get(const Key &key, const Value &default_value = Value()) {
        if (data_.contains(key)) {
            return data_[key];
        } else {
            return default_value;
        }
    }

    bool TryGet(const Key &key, Value &value) {
        if (data_.contains(key)) {
            value = data_[key];
            return true;
        } else {
            return false;
        }
    }

    bool Contains(const Key &key) const { return data_.contains(key); }

    void Clear() { data_.clear(); }

    UInt64 Count() { return data_.size(); }

    Value &operator[](const Key &key) { return data_[key]; }

    Value operator[](const Key &key) const { return data_.at(key); }

    bool Empty() { return data_.empty(); }

    auto Find(const Key &key) { return data_.find(key); }

    template<typename Func>
        requires std::is_invocable_v<Func, Pair<const Key, Value> &>
    bool RemoveIf(Func &&func, const bool remove_all = false) {
        bool removed = false;
        for (auto it = data_.begin(); it != data_.end();) {
            if (func(*it)) {
                it = data_.erase(it);
                removed = true;
                if (!remove_all) {
                    return true;
                }
            } else {
                ++it;
            }
        }
        return removed;
    }

    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

private:
    std::unordered_map<Key, Value> data_;
};
