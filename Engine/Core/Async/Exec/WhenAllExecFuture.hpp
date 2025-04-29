//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Common.hpp"
#include "Core/TypeAlias.hpp"
#include "Core/TypeTraits.hpp"

namespace exec {
    template<typename... ExecFutures>
    struct WhenAllExecFutureSender {
        using combined_type = Tuple<typename ExecFutures::value_type...>;
        using value_type = Traits::Flatten<combined_type>;

        Tuple<Pure<ExecFutures>...> futures;

        explicit WhenAllExecFutureSender(ExecFutures &&...futures) : futures(MakeTuple(Move(futures)...)) {}

        template<typename R>
        struct Operation : Op {
            Tuple<Pure<ExecFutures>...> op_futures;
            Pure<R> next;

            using value_type = Traits::Flatten<combined_type>;
            using combined_type = Tuple<Pure<ExecFutures>...>;

            Operation(Pure<R> &&r, Tuple<Pure<ExecFutures>...> &&futures) : op_futures(Move(futures)), next(Move(r)) {}

            template<size_t I>
            bool Check() {
                if constexpr (I < sizeof...(ExecFutures)) {
                    return std::get<I>(op_futures).IsCompleted() && Check<I + 1>();
                } else {
                    return true;
                }
            }

            template<size_t I, size_t Minus = 0>
            void CollectResultTuple(value_type &result) {
                if constexpr (I < std::tuple_size_v<combined_type>) {
                    if constexpr (Traits::SameAs<std::tuple_element_t<I, combined_type>, Tuple<>>) {
                        CollectResultTuple<I + 1, Minus + 1>(result);
                    } else {
                        // TODO: 如果第i个是个多个参数的tuple, 又要将这个tuple展开遍历不能直接Get
                        std::get<I - Minus>(result) = std::get<I>(op_futures).Get();
                        CollectResultTuple<I + 1, Minus>(result);
                    }
                }
            }

            bool Start() noexcept {
                if (Check<0>()) {
                    value_type tuple;
                    CollectResultTuple<0, 0>(tuple);
                    next.SetValue(Move(tuple));
                    return true;
                }
                return false;
            }
        };

        template<typename ReceiverType>
        auto Connect(ReceiverType &&r)
            requires Traits::SameAs<value_type, typename Pure<ReceiverType>::receive_type>
        {
            return Operation<Pure<ReceiverType>>(Move(r), Move(futures));
        }
    };

    template<typename... ExecFutures>
    auto WhenAllExecFuture(ExecFutures &&...futures) {
        return WhenAllExecFutureSender<Pure<ExecFutures>...>(Move(futures)...);
    }
} // namespace exec
