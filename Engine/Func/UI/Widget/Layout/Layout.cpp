//
// Created by Echo on 2025/3/30.
//

#include "Layout.hpp"

IMPL_REFLECTED(Layout) { return Type::Create<Layout>("Layout") | refl_helper::AddParents<Widget>(); }

IMPL_ENUM_REFL(EHorizontalLayout) {
    return Type::Create<EHorizontalLayout>("EHorizontalLayout") | refl_helper::AddEnumField("Left", EHorizontalLayout::Left) |
           refl_helper::AddEnumField("Middle", EHorizontalLayout::Middle) | refl_helper::AddEnumField("Right", EHorizontalLayout::Right);
}

IMPL_ENUM_REFL(EVerticalLayout) {
    return Type::Create<EVerticalLayout>("EVerticalLayout") | refl_helper::AddEnumField("Bottom", EVerticalLayout::Bottom) |
           refl_helper::AddEnumField("Middle", EVerticalLayout::Middle) | refl_helper::AddEnumField("Top", EVerticalLayout::Top);
}
