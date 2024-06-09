/**
 * @file ContainerUtils.h
 * @author Echo 
 * @Date 24-6-2
 * @brief 
 */

#pragma once

#include "CoreDef.h"


#include <algorithm>

class ContainerUtils {
public:
    template<typename ContainerT>
    static void Remove(ContainerT& Container, const typename ContainerT::value_type& Value);

    template<typename ContainerT, typename Lambda>
    static void RemoveIf(ContainerT& Container, const Lambda& Value);

    template<typename ContainerT, typename Lambda>
    static auto FindFirstIf(ContainerT& Container, const Lambda& Value) -> TOptional<decltype(Container.begin())>;

    template <typename ContainerT>
    static ContainerT Slice(const ContainerT& Container, Int32 Start = 0, Int32 End = 0);
};

template<typename ContainerT>
void ContainerUtils::Remove(ContainerT& Container, const typename ContainerT::value_type& Value) {
    if (auto Iter = std::find(Container.begin(), Container.end(), Value); Iter != Container.end()) {
        Container.erase(Iter);
    }
}

template<typename ContainerT, typename Lambda>
void ContainerUtils::RemoveIf(ContainerT& Container, const Lambda& Value) {
    if (auto Iter = std::find_if(Container.begin(), Container.end(), Value); Iter != Container.end()) {
        Container.erase(Iter);
    }
}

template<typename ContainerT, typename Lambda>
auto ContainerUtils::FindFirstIf(ContainerT& Container, const Lambda& Value) -> TOptional<decltype(Container.begin())> {
    auto Iter = std::find_if(Container.begin(), Container.end(), Value);
    if (Iter != Container.end()) {
        return Iter;
    }
    return std::nullopt;
}

template<typename ContainerT>
ContainerT ContainerUtils::Slice(const ContainerT& Container, Int32 Start, Int32 End) {
    ContainerT result;

    // 处理负数索引和默认参数
    auto containerSize = std::distance(Container.begin(), Container.end());
    if (Start < 0) {
        Start += containerSize;
    }
    if (End <= 0) {
        End += containerSize;
    }

    // 边界检查
    Start = std::max(0, Start);
    End = std::min(static_cast<Int32>(containerSize), End);

    // 切片操作
    auto startIter = std::next(Container.begin(), Start);
    auto endIter = std::next(Container.begin(), End);
    std::copy(startIter, endIter, std::back_inserter(result));

    return result;
}
