/**
 * @file ContainerUtils.h
 * @author Echo 
 * @Date 24-6-2
 * @brief 
 */

#pragma once

#include "Core/CoreDef.h"
#include "Core/CoreGlobal.h"

#include <algorithm>

class ContainerUtils
{
public:
#if REGION(容器通用操作)
    template<typename ContainerT>
    static void Remove(ContainerT& Container, const typename ContainerT::value_type& Value);

    template<typename ContainerT, typename Lambda>
    static void RemoveIf(ContainerT& Container, const Lambda& Value);

    template<typename ContainerT>
    static ContainerT Slice(const ContainerT& Container, int32_t Start = 0, int32_t End = 0);

    template<typename ContainerT>
    static ContainerT Concat(const ContainerT& ContainerA, const ContainerT& ContainerB);

    template<typename ContainerT>
    static void AddUnique(ContainerT& container, const typename ContainerT::value_type& value);

    template<typename KeyType, typename ValueType>
    static bool ContainsValue(const core::HashMap<KeyType, ValueType> map, const ValueType& value)
    {
        for (const auto& [key, val]: map)
        {
            if (val == value)
            {
                return true;
            }
        }
        return false;
    }

    template<typename ContainerT>
    static bool Contains(const ContainerT& container, const typename ContainerT::value_type& value)
    {
        for (const auto& item: container)
        {
            if (item == value) return true;
        }
        return false;
    }

    template<typename ContainerT, typename FuncT>
    static bool ContainsIf(const ContainerT& container, FuncT func)
    {
        for (const auto& item: container)
        {
            if (func(item)) return true;
        }
        return false;
    }

    template<typename ContainerT>
        requires std::random_access_iterator<typename ContainerT::iterator>
    static bool RemoveAt(ContainerT& container, int32_t index)
    {
        if (index < 0 || index >= container.size())
        {
            return false;
        }
        container.erase(container.begin() + index);
        return true;
    }

    template<typename ContainerT>
    static core::Optional<typename ContainerT::value_type> First(
        const ContainerT&                                             container,
        const core::Function<bool(const typename ContainerT::value_type&)>& predicate = [](const typename ContainerT::value_type&) { return true; }
    )
    {
        for (auto& item: container)
        {
            if (predicate(item))
            {
                return item;
            }
        }
        return {};
    }
#endif

#if REGION(容器特化操作.Array)
    /**
     * 快速删除一个元素,但是只是把它移动到最后
     * @tparam T 类型
     * @param array
     * @param index
     * @return
     */
    template<typename T>
    static void FastRemoveAt(core::Array<T>& array, int32_t index)
    {
        // DebugAssert(Container, index >= 0 && index < array.size(), "Index out of range");
        std::swap(array[index], array[array.size() - 1]);
        array.pop_back();
    }

#endif
};

template<typename ContainerT>
void ContainerUtils::Remove(ContainerT& Container, const typename ContainerT::value_type& Value)
{
    if (auto Iter = std::find(Container.begin(), Container.end(), Value); Iter != Container.end())
    {
        Container.erase(Iter);
    }
}

template<typename ContainerT, typename Lambda>
void ContainerUtils::RemoveIf(ContainerT& Container, const Lambda& Value)
{
    if (auto Iter = std::find_if(Container.begin(), Container.end(), Value); Iter != Container.end())
    {
        Container.erase(Iter);
    }
}

template<typename ContainerT>
ContainerT ContainerUtils::Slice(const ContainerT& Container, int32_t Start, int32_t End)
{
    ContainerT result;

    // 处理负数索引和默认参数
    auto containerSize = std::distance(Container.begin(), Container.end());
    if (Start < 0)
    {
        Start += containerSize;
    }
    if (End <= 0)
    {
        End += containerSize;
    }

    // 边界检查
    Start = std::max(0, Start);
    End   = std::min(static_cast<int32_t>(containerSize), End);

    // 切片操作
    auto startIter = std::next(Container.begin(), Start);
    auto endIter   = std::next(Container.begin(), End);
    std::copy(startIter, endIter, std::back_inserter(result));

    return result;
}

template<typename ContainerT>
ContainerT ContainerUtils::Concat(const ContainerT& ContainerA, const ContainerT& ContainerB)
{
    ContainerT result;

    // 将容器A的元素添加到结果容器中
    result.insert(result.end(), ContainerA.begin(), ContainerA.end());

    // 将容器B的元素添加到结果容器中
    result.insert(result.end(), ContainerB.begin(), ContainerB.end());

    return result;
}

template<typename ContainerT>
void ContainerUtils::AddUnique(ContainerT& container, const typename ContainerT::value_type& value)
{
    if (std::count(container.begin(), container.end(), value) == 0)
    {
        container.push_back(value);
    }
}
