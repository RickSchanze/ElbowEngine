/**
 * @file CoreTypeDef.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "TypeTraits.h"
#include <array>
#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tl/expected.hpp"

namespace core
{
// std::function -> Function
template <typename T>
using Function = std::function<T>;

using TimePoint = std::chrono::steady_clock::time_point;

// std::vector -> Array
template <typename T, typename Allocator = std::allocator<T>>
using Array = std::vector<T, Allocator>;

// std::array -> StaticArray
template <typename T, size_t Size>
using StaticArray = std::array<T, Size>;

// std::set -> Set
template <class KeyType, class Comparator = std::less<KeyType>, class Allocator = std::allocator<KeyType>>
using Set = std::set<KeyType, Comparator, Allocator>;

// std::map -> Map
template <
    class KeyType, class ValueType, class Comparator = std::less<KeyType>, class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using Map = std::map<KeyType, ValueType, Comparator, Allocator>;

template <typename T, typename Allocator = std::allocator<T>>
using List = std::list<T, Allocator>;

inline constexpr auto NullOpt = std::nullopt;

// std::shared_ptr -> SharedPtr
template <typename T>
using SharedPtr = std::shared_ptr<T>;
// std::weak_ptr -> WeakPtr
template <typename T>
using WeakPtr = std::weak_ptr<T>;
// std::make_shared -> MakeShared
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args)
{
#if ENABLE_PROFILING
    return std::allocate_shared<T>(MemoryTraceAllocator<T>(), std::forward<Args>(args)...);
#else
    return std::make_shared<T>(std::forward<Args>(args)...);
#endif
}

// std::forward -> Forward
template <typename T>
constexpr T&& Forward(std::remove_reference_t<T>& Arg) noexcept
{
    return static_cast<T&&>(Arg);
}

// std::move -> Move
template <typename T>
constexpr std::remove_reference_t<T>&& Move(T&& Arg) noexcept
{
    return static_cast<std::remove_reference_t<T>&&>(Arg);
}

// std::unordered_map -> HashMap
template <
    class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>,
    class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using HashMap = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, Allocator>;

// std::unordered_set -> HashSet
template <class KeyType, class Hasher = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>, class Alloc = std::allocator<KeyType>>
using HashSet = std::unordered_set<KeyType, Hasher, KeyEqual, Alloc>;

template <typename T>
constexpr T&& Forward(std::remove_reference_t<T>&& Arg) noexcept
{
    static_assert(!std::is_lvalue_reference_v<T>, "Can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(Arg);
}

// std::dynamic_pointer_cast -> StaticPointerCast
template <typename T, typename U>
SharedPtr<T> StaticPointerCast(const SharedPtr<U>& InSharedPtr)
{
    return std::static_pointer_cast<T>(InSharedPtr);
}

template <typename... T>
using Tuple = std::tuple<T...>;

template <typename T, typename E>
using Expected = tl::expected<T, E>;

template <typename T>
tl::unexpected<T> Unexpected(T&& value)
{
    return tl::unexpected(std::forward<T>(value));
}
}   // namespace core
using Int8   = int8_t;
using Int16  = int16_t;
using Int32  = int32_t;
using Int64  = int64_t;
using UInt8  = uint8_t;
using UInt16 = uint16_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;
using Float  = float;
using Double = double;
using Bool   = bool;
