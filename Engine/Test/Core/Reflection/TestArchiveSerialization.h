/**
 * @file TestArchiveSerialization.h
 * @author Echo 
 * @Date 24-11-7
 * @brief 
 */

#pragma once

#include "Reflection/MetaInfoMacro.h"
#include "Reflection/Reflection.h"

class CLASS() TestArchiveSerialization : public core::ITypeGetter
{
    GENERATED_BODY(TestArchiveSerialization)

    PROPERTY()
    int32_t a_ = 12;

    PROPERTY()
    float b_ = 3.14f;

    PROPERTY()
    bool c_ = true;

    PROPERTY()
    core::String d_ = "Hello";

    PROPERTY()
    core::StringView e_ = "World";

    PROPERTY()
    core::Array<int32_t> f_ = {1, 2, 3, 4, 5};

    PROPERTY()
    core::HashMap<int32_t, core::String> g_ = {{1, "one"}, {2, "two"}, {3, "three"}};
};
