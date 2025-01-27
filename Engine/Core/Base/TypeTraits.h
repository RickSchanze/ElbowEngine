/**
 * 一些类型特征提取
 */

#pragma once
#include <tuple>
#include <type_traits>

namespace core {
template <typename F, typename... Vs> constexpr bool FunctionInvokable() { return std::is_invocable_v<F, Vs...>; }

template <typename F, typename... Vs> constexpr bool FunctionReturnVoid() {
  return std::is_void_v<std::invoke_result_t<F, Vs...>>;
}

template <typename F, typename... Vs> constexpr bool FunctionNoThrowInvokable() {
  return std::is_nothrow_invocable_v<F, Vs...>;
}

template <typename F, typename... Vs> constexpr bool FunctionThrowReturnInvokable() {
  return !std::is_nothrow_invocable_v<F, Vs...>;
}

template <typename... Args> struct ArgTypesAllNotVoid {
  static constexpr bool Value = (!std::is_same_v<Args, void> && ...);
};

template <typename... Args> struct ArgTypesAllNotVoid<std::tuple<Args...>> {
  static constexpr bool Value = (!std::is_same_v<Args, void> && ...);
};

template <typename T, typename... Args> struct IsAnyOf {
  static constexpr bool Value = (std::is_same_v<T, Args> || ...);
};

template <typename T> using Pure = std::remove_cvref_t<T>;

// 将多个std::tuple合为一个std::tuple 同时void不计入计算 对类型计算
// MergeTuples<std::tuple<int>, std::tuple<double>, void, std::tuple<char>>::type = std::tuple<int, double, char>
template <typename... Senders> struct MergeTuples;

// 偏特化：当第一个类型是 std::tuple 时
template <typename First, typename... Rest> struct MergeTuples<First, Rest...> {
  // 如果 First 是 std::tuple，递归合并
  using type = decltype(std::tuple_cat(std::declval<First>(),                              // 当前的 std::tuple
                                       std::declval<typename MergeTuples<Rest...>::type>() // 递归结果
                                       ));
};

// 偏特化：当第一个类型是 void 时
template <typename... Rest> struct MergeTuples<void, Rest...> {
  // 跳过 void，直接递归处理剩余类型
  using type = typename MergeTuples<Rest...>::type;
};

// 偏特化：终止条件（无更多类型时，返回空的 std::tuple）
template <> struct MergeTuples<> {
  using type = std::tuple<>;
};

// 辅助模板：判断是否为 std::tuple
template <typename T> struct is_tuple : std::false_type {};

template <typename... Args> struct is_tuple<std::tuple<Args...>> : std::true_type {};

// MergeTupleNested<std::tuple<int>, std::tuple<double>, void, std::tuple<char>>::type = std::tuple<std::tuple<int>,
// std::tuple<double>, std::tuple<>, std::tuple<char>> 对类型进行计算
template <typename... Senders> struct MergeTupleNested;

// 递归处理：当前类型是 std::tuple
template <typename First, typename... Rest> struct MergeTupleNested<First, Rest...> {
  // 如果 First 是 std::tuple，直接将它嵌套到 std::tuple 中
  using type = decltype(std::tuple_cat(std::declval<std::tuple<First>>(),                       // 包装 First
                                       std::declval<typename MergeTupleNested<Rest...>::type>() // 递归结果
                                       ));
};

// 递归处理：当前类型是 void
template <typename... Rest> struct MergeTupleNested<void, Rest...> {
  // 如果是 void，包装为 std::tuple<void>
  using type = decltype(std::tuple_cat(std::declval<std::tuple<std::tuple<>>>(),                // 包装 void
                                       std::declval<typename MergeTupleNested<Rest...>::type>() // 递归结果
                                       ));
};

// 递归终止条件：无更多类型时
template <> struct MergeTupleNested<> {
  using type = std::tuple<>; // 空 tuple
};

// std::tuple<std::tuple<int>, std::tuple<double>, std::tuple<>, std::tuple<char>> -> std::tuple<int, double, char>
// 对值进行计算
template <int Index = 0, typename Tuple> constexpr auto FlattenTuple(Tuple &&tuples) {
  if constexpr (Index < std::tuple_size_v<std::decay_t<Tuple>> - 1) {
    return std::tuple_cat(std::get<Index>(std::forward<Tuple>(tuples)), FlattenTuple<Index + 1>(std::forward<Tuple>(tuples)));
    }
    else
    {
        return std::get<Index>(std::forward<Tuple>(tuples));
    }
}
}
