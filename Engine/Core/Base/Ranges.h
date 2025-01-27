//
// Created by Echo on 25-1-26.
//

#pragma once

#include "CoreTypeDef.h"
#include "range/v3/all.hpp"

namespace core::range {
inline constexpr auto AnyOf = ranges::any_of;
}

namespace core::range::view {
inline constexpr auto Concat = ranges::views::concat;
inline constexpr ranges::detail::to_container_fn<ranges::detail::from_range<Array>> ToArray{};
inline constexpr auto Transform = ranges::views::transform;
inline constexpr auto Enumerate = ranges::views::enumerate;
inline constexpr auto RemoveIf = ranges::views::remove_if;
} // namespace core::range::view

namespace core::range::action {
constexpr auto Unique = ranges::actions::unique;
}