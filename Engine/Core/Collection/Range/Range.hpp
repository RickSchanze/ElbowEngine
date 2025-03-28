
#pragma once
#include <algorithm>
#include <ranges>

namespace range {
    constexpr auto AnyOf = std::ranges::any_of;
    constexpr auto Sort = std::ranges::sort;
    constexpr auto Fill = std::ranges::fill;
    constexpr auto Contains = std::ranges::contains;

    template<typename T>
    auto To() {
        return std::ranges::to<T>();
    }

    namespace view {
        constexpr auto Where = std::ranges::views::filter;
        constexpr auto Select = std::ranges::views::transform;
        constexpr auto All = std::ranges::views::all;
        constexpr auto Keys = std::ranges::views::keys;
        constexpr auto Values = std::ranges::views::values;
        constexpr auto Enumerate = std::ranges::views::enumerate;
    } // namespace view

} // namespace range
