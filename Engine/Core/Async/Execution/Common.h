/**
 * @file Common.h
 * @author Echo
 * @Date 24-12-4
 * @brief
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/TypeTraits.h"

namespace core::exec {
struct Sender {};
struct Receiver {};
struct Op {};
struct Scheduler {};

template <typename SenderType, typename ReceiverType>
auto Connect(SenderType &&s, ReceiverType &&r) -> decltype(s.Connect(Forward<ReceiverType>(r))) {
  return s.Connect(Forward<Pure<ReceiverType>>(r));
}
} // namespace core::exec
