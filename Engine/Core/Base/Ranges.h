//
// Created by Echo on 25-1-26.
//

#pragma once

#include "CoreTypeDef.h"

#ifndef REFLECTION
#include "range/v3/all.hpp"
namespace core::range {
inline constexpr auto AnyOf = ranges::any_of;
inline constexpr auto Contains = ranges::contains;
inline constexpr auto Remove = ranges::remove;
} // namespace core::range

namespace core::range::view {
inline constexpr auto Concat = ranges::views::concat;
inline constexpr ranges::detail::to_container_fn<ranges::detail::from_range<Array>> ToArray{};
inline constexpr auto Transform = ranges::views::transform;
inline constexpr auto Enumerate = ranges::views::enumerate;
inline constexpr auto RemoveIf = ranges::views::remove_if;
inline constexpr auto Keys = ranges::views::keys;
} // namespace core::range::view

namespace core::range::action {
constexpr auto Unique = ranges::actions::unique;
}
#endif

namespace core::range {
template <typename T> void RemoveAt(Array<T> &container, UInt64 index, bool fast = true) {
  if (fast) {
    std::swap(container[index], container.back());
    container.pop_back();
  } else {
    container.erase(container.begin() + index);
  }
}

template <typename T, typename V> void RemoveByValue(HashMap<T, V> &container, const V &value) {
  auto it = container.begin();
  while (it != container.end()) {
    if (it->second == value) {
      it = container.erase(it);
      break;
    }
    ++it;
  }
}
} // namespace core::range
