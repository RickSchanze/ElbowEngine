//
// Created by RickS on 24-12-8.
//

#pragma once
#include "Common.h"

#include <exception>

namespace core::exec
{
template <class T, class E = std::exception_ptr>
concept CReceiver = requires(std::remove_cvref_t<T>&& t, E&& e) {
    { SetDone(std::move(t)) } noexcept;
    { SetError(std::move(t), static_cast<E&&>(e)) } noexcept;
};

template <class ReceiverT, class... Args>
concept CReceiverOf = CReceiver<ReceiverT> && requires(std::remove_cvref_t<ReceiverT>&& t, Args&&... args) {
    { SetValue(std::move(t), static_cast<Args&&>(args)...) } noexcept;
};
}   // namespace core::exec