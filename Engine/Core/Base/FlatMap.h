//
// Created by Echo on 24-12-18.
//

#pragma once
#include <unordered_map>

namespace core
{
template <
    class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>,
    class Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
using FlatMap = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, Allocator>;
}
