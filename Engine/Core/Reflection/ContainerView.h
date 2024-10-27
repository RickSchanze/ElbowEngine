/**
 * @file ContainerView.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include <any>

#include "Any.h"

namespace core
{
class ContainerView
{
public:
    virtual void BeginIterate() = 0;
    virtual void Next() = 0;

    Any& GetCurrent() { return current_element_; }

    Any& begin() { return current_element_; };
    Any& end()   { return current_element_; };

private:
    std::any begin_;
    std::any end_;
    std::any iterator_;
    Any      current_element_;
};
}   // namespace core
