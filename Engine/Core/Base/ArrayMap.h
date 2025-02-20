//
// Created by Echo on 25-2-20.
//

#pragma once
#include "Core/Assert.h"
#include "CoreTypeDef.h"
#include "Ranges.h"

namespace core {

// 注意: 此容器未有反射支持
template <typename K, typename V> class ArrayMap {
public:
  void Remove(const K &key) {
    if (map_.contains(key)) {
      auto index = map_[key];
      range::RemoveAt(values_, index);
      map_.erase(key);
    }
  }

  bool TryRemove(const K &key) {
    if (map_.contains(key)) {
      auto index = map_[key];
      range::RemoveAt(values_, index);
      map_.erase(key);
      return true;
    }
    return false;
  }

  const Array<V> &ToArray() const { return values_; }
  const HashMap<K, UInt64> &ToHashMap() const { return map_; }

  void Add(const K &key, const V &value) {
    if (map_.contains(key)) {
      auto index = map_[key];
      values_[index] = value;
    } else {
      map_[key] = values_.size();
      values_.push_back(value);
    }
  }

  bool TryAdd(const K &key, const V &value) {
    if (map_.contains(key)) {
      return false;
    } else {
      map_[key] = values_.size();
      values_.push_back(value);
      return true;
    }
  }

  UInt64 Size() const { return values_.size(); }

  V &At(const K &key) {
    auto index = map_[key];
    return values_[index];
  }

  V &operator[](const K &key) {
    if (!map_.contains(key)) {
      map_[key] = values_.size();
      values_.push_back(V{});
    }
    auto index = map_[key];
    return values_[index];
  }

  bool Contains(const K &key) const { return map_.contains(key); }

  void Clear() {
    map_.clear();
    values_.clear();
  }

private:
  HashMap<K, UInt64> map_;
  Array<V> values_;
};

} // namespace core
