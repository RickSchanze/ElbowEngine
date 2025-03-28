//
// Created by Echo on 2025/3/24.
//

#include "ContainerView.hpp"

void SequentialContainerView::ForEach(const Function<void(Any)> &Func) {
    if (BeginIterate()) {
        Func(GetCurrentElement());
        while (HasNext()) {
            Next();
            Func(GetCurrentElement());
        }
        EndIterate();
    }
}

void SequentialContainerView::ForEach(const Function<void(int, Any)> &Func) {
    if (BeginIterate()) {
        int index = 0;
        Func(index, GetCurrentElement());
        while (HasNext()) {
            Next();
            Func(++index, GetCurrentElement());
        }
        EndIterate();
    }
}

void AssociativeContainerView::ForEach(const Function<void(Any, Any)> &Func) {
    if (BeginIterate()) {
        Func(GetCurrentKey(), GetCurrentValue());
        while (HasNext()) {
            Next();
            Func(GetCurrentKey(), GetCurrentValue());
        }
        EndIterate();
    }
}
