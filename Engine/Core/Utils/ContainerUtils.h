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
