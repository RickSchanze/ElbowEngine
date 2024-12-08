/**
 * @file TypeList.h
 * @author Echo
 * @Date 24-12-4
 * @brief 此文件提供了ElbowEngine自己的TypeList
 */

#pragma once
#include <tuple>
#include <type_traits>

namespace core
{
template <class... Ts>
struct TypeList
{
    template <template <typename...> class F>
    using Apply = F<Ts...>;
};

template <class... Ts>
struct TypeCount
{
    static constexpr size_t Value() { return sizeof...(Ts); }
};

namespace detail
{
template <class... TypeLists>
struct ConcatenatedTypeListsImpl;

template <>
struct ConcatenatedTypeListsImpl<>
{
    using TypeList = TypeList<>;
};

template <class... Ts, class... Us>
struct ConcatenatedTypeListsImpl<TypeList<Ts...>, TypeList<Us...>>
{
    using TypeList = TypeList<Ts..., Us...>;
};

template <class... Ts, class... Us, class... OtherLists>
struct ConcatenatedTypeListsImpl<TypeList<Ts...>, TypeList<Us...>, OtherLists...> : ConcatenatedTypeListsImpl<TypeList<Ts..., Us...>, OtherLists...>
{
};

template <class... TypeSets>
struct ConcatenatedTypeSetsImpl;

template <>
struct ConcatenatedTypeSetsImpl<>
{
    using TypeList = TypeList<>;
};

template <class... Ts>
struct ConcatenatedTypeSetsImpl<TypeList<Ts...>>
{
    using TypeList = TypeList<Ts...>;
};

template <class T, class... Ts>
constexpr bool IsOneOf()
{
    return (std::is_same_v<T, Ts> || ...);
}

template <class... Ts, class... Us, class... OtherLists>
struct ConcatenatedTypeSetsImpl<TypeList<Ts...>, TypeList<Us...>, OtherLists...>
    : ConcatenatedTypeSetsImpl<
          typename ConcatenatedTypeListsImpl<TypeList<Ts...>, std::conditional_t<IsOneOf<Us, Ts...>(), TypeList<>, TypeList<Us>>...>::TypeList,
          OtherLists...>
{
};

template <template <class...> class Outer, template <typename...> class Inner>
struct ZipApplyImpl
{
    template <class... TypeLists>
    using Apply = Outer<typename TypeLists::template Apply<Inner>...>;
};
}   // namespace detail

/**
 * 连接多个TypeList(没有去重)
 * @example ConcatenatedTypeList<TypeList<int, float>, TypeList<double>>::TypeList 等价于 TypeList<int, float, double>
 */
template <class... TypeLists>
using ConcatenatedTypeList = detail::ConcatenatedTypeListsImpl<TypeLists...>;
/**
 * 连接多个TypeList(去重)
 * @example ConcatenatedTypeSet<TypeList<int, float>, TypeList<double, int>>::TypeList 等价于 TypeList<int, float, double>
 */
template <class... TypeLists>
using ConcatenatedTypeSet = detail::ConcatenatedTypeSetsImpl<TypeLists...>;

/**
 * 对于T = TypeList<TypeList<Us...>, TypeList<Vs...>,...>，
 * ZipApply<T, Outer, Inner> 等价于 Outer<Inner<Us...>, Inner<Vs...>,...>
 */
template <class ListOfLists, template <typename...> class Outer, template <typename...> class Inner>
using ZipApply = typename ListOfLists::template Apply<detail::ZipApplyImpl<Outer, Inner>::template Apply>;
}
