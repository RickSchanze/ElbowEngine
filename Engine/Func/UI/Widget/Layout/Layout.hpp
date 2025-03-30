//
// Created by Echo on 2025/3/30.
//

#pragma once
#include "Func/UI/Widget/Widget.hpp"

enum class EHorizontalLayout { Left, Middle, Right };
DECL_ENUM_REFL(EHorizontalLayout);

enum class EVerticalLayout { Top, Middle, Bottom };
DECL_ENUM_REFL(EVerticalLayout)

class Layout : public Widget {
    REFLECTED_CLASS(Layout)

public:
    virtual void RebuildHierarchy() {}
};

REGISTER_TYPE(Layout)

EXEC_BEFORE_MAIN() {
    ReflManager::GetByRef().Register<EHorizontalLayout>(&ConstructType_EHorizontalLayout);
    ReflManager::GetByRef().Register<EVerticalLayout>(&ConstructType_EVerticalLayout);
}
