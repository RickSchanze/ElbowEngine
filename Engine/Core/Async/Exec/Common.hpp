//
// Created by Echo on 2025/3/22.
//

#pragma once

#include "Core/TypeTraits.hpp"

namespace NExec
{
struct Sender
{
};
struct Receiver
{
};
struct Op
{
};
struct Scheduler
{
};

template <typename SenderType, typename ReceiverType>
auto Connect(SenderType&& s, ReceiverType&& r) -> decltype(s.Connect(Forward<ReceiverType>(r)))
{
    return s.Connect(Forward<Pure<ReceiverType>>(r));
}
} // namespace NExec
