//
// Created by Echo on 2025/3/22.
//

#pragma once

#include "Common.hpp"
#include "Core/Misc/Other.hpp"

namespace exec {
    template<typename T>
    struct EmptyReceiver : Receiver {
        using receive_type = T;

        void SetValue(T &&t) {}
        void SetError(const ExceptionPtr &ptr) { RethrowException(ptr); }
    };

    template<>
    struct EmptyReceiver<void> : Receiver {
        using receive_type = Tuple<>;

        void SetValue(Tuple<> &&t) {}
        void SetError(const ExceptionPtr &ptr) { RethrowException(ptr); }
    };
} // namespace exec
