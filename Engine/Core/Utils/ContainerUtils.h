/**
 * @file ContainerUtils.h
 * @author Echo 
 * @Date 24-6-2
 * @brief 
 */

#pragma once

#include "CoreDef.h"


#include <algorithm>

class ContainerUtils
{
public:
    template<typename ContainerT>
    static void Remove(ContainerT& Container, const typename ContainerT::value_type& Value);

    template<typename ContainerT, typename Lambda>
    static void RemoveIf(ContainerT& Container, const Lambda& Value);

    template<typename ContainerT, typename Lambda>
    static auto FindFirstIf(ContainerT& Container, const Lambda& Value) -> TOptional<decltype(Container.begin())>;

    template<typename ContainerT>
    static ContainerT Slice(const ContainerT& Container, int32_t Start = 0, int32_t End = 0);

    template<typename ContainerT>
    static ContainerT Concat(const ContainerT& ContainerA, const ContainerT& ContainerB);

    template <typename ContainerT>
    static void AddUnique(ContainerT& container, const typename ContainerT::value_type& value);
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

template<typename ContainerT, typename Lambda>
auto ContainerUtils::FindFirstIf(ContainerT& Container, const Lambda& Value) -> TOptional<decltype(Container.begin())>
{
    auto Iter = std::find_if(Container.begin(), Container.end(), Value);
    if (Iter != Container.end())
    {
        return Iter;
    }
    return std::nullopt;
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
void ContainerUtils::AddUnique(ContainerT& container, const typename ContainerT::value_type& value){
    if (std::count(container.begin(), container.end(), value) == 0)
    {
        container.push_back(value);
    }
}
