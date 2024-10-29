/**
 * @file ContainerView.cpp
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#include "ContainerView.h"

namespace core
{
void SequentialContainerView::ForEach(const Function<void(Any)>& Func)
{
    DebugAssert(LogCat::Reflection, GetContainerType() == ContainerType::Sequential, "一个参数的ForEach只有持有顺序容器时才可以使用");
    if (BeginIterate())
    {
        Func(GetCurrentElement());
        while (HasNext())
        {
            Next();
            Func(GetCurrentElement());
        }
        EndIterate();
    }
}

void AssociativeContainerView::ForEach(const Function<void(Any, Any)>& Func)
{
    DebugAssert(LogCat::Reflection, GetContainerType() == ContainerType::Associative, "两个参数的ForEach只有持有关联容器时才可以使用");
    if (BeginIterate())
    {
        Func(GetCurrentKey(), GetCurrentValue());

        while (HasNext())
        {
            Func(GetCurrentKey(), GetCurrentValue());
            Next();
        }
        EndIterate();
    }
}
}
