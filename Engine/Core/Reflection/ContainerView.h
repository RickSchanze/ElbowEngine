/**
 * @file ContainerView.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once

#include "Any.h"
#include <any>

namespace core
{
class ContainerView
{
public:
    virtual void BeginIterator() = 0;
    virtual bool HasNext()       = 0;
    virtual void Next();

    // 注意:
    // 这个获得的元素会是Ref<ElementType>
    // 但是这个Any对应的Type是ElementType的Type而不是Ref<ElementType>的Type
    Any GetCurrent() { return current_element_; }

private:
    std::any iterator_;
    Any      current_element_;
};
}   // namespace core
